#include "pic24_all.h"
#include "pcb_version.h"
#include "i2c.h"
#include "timers.h"
#include "sensor.h"
#include "audio.h"
#include "filters.h"
#include "vario.h"
#include "button.h"
#include <stdio.h>
#include "uart.h"
#include "chars.h"
#if PCB_VERSION == 1
#include "MMA8452.h"
#endif

#define GPS_2Hz
#define DEBUG
//#define ELLE_AIME_LA_BITE

#ifdef DEBUG
    char dbgBuf[50];
#endif

// Transmit buffer to Kobo
char koboTxBuf[82];
char *txBufPtr;

u8 timer1Cnt = 0;
const u8 ctimer1Max_kobo = 3;

#define NBVALUES 10
s32 values[NBVALUES];
u8 values_i = 0;

// mutexes for variable protection
u8 mutex_8bits = 0;
// bit 0: NEW DATA available for NMEA sentence
// bit 1: Begin of time window for data sending

#define MUTEX_NEW_DATA_BIT     0
#define MUTEX_TIMETOSEND_BIT   1
#define MUTEX_TRANSMITTING_BIT 2

// bufferized variables
s16 varioBuf;
s8 temperatureBuf;
u32 pressureBuf;

_FBS( BSS_OFF & BWRP_OFF )
_FGS( GSS0_OFF & GWRP_OFF )
_FOSCSEL( FNOSC_FRCDIV  & IESO_OFF )
_FOSC( FCKSM_CSECMD & POSCFREQ_100KHZ & OSCIOFNC_ON & POSCMOD_NONE)
_FWDT( FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )
_FPOR( MCLRE_ON & BORV_1_8V & AI2C1SEL_PRI & PWRTEN_OFF & BOREN_BOR3)
_FICD( BKBUG_OFF & COE_OFF & ICS_PGx3 )
_FDS( DSWDTEN_OFF & DSBOR_OFF & RTCCREF_OFF & DSWDTLPRC_OFF & DSWDTPS_DSWDTPS_0 )


volatile u8 tmr1_waiting = 0;
u8 sns_status = 0;
u32 Praw = 0;

#define INTRO_LENGTH 7
u32 intro_freqs[INTRO_LENGTH] = {440, 0,  698, 0,  659, 0,  587};
u32 intro_times[INTRO_LENGTH] = {600, 40, 600, 40, 600, 40, 1200};


void startElapsedTime(void) {
    tmr1_Start(100000);
    _T1IE = 0;
}

void setMutexBit (u8 *mutex, const u8 bitnb) {
    // protect critical section
    asm("DISI #55"); // disable interrupts for the next 55 instruction cycles (intendedly too much)
    *mutex = (*mutex)|(1 << bitnb);
	asm("DISI #0"); // re-enable interrupts immediately
}

void resetMutexBit (u8 *mutex, const u8 bitnb) {
    // protect critical section
    asm("DISI #55"); // disable interrupts for the next 55 instruction cycles (intendedly too much)
    *mutex = (*mutex) & (~(1 << bitnb));
	asm("DISI #0"); // re-enable interrupts immediately
}

u8 getMutexBit (u8 mutex, const u8 bitnb) {
    return (mutex & (1 << bitnb)) >> bitnb;
}

void printElapsedTime(void) {
    sprintf(dbgBuf, "Elapsed Time = %u ms\r\n", TMR1);
    tmr1_Stop();
    uart_Print(dbgBuf);
    sns_status = 0;
    sns_TriggerTemperatureSample();
    tmr1_Start(MEASURE_PERIOD_T);
}

void mcu_Init(void) {
    AD1PCFG = 0xFFFF;            // set all analog-input enabled pins (ANx) to digital mode
    CLKDIVbits.RCDIV = 3;  	 // FRC oscillator 8MHz divided by 8
    CONFIG_RA6_AS_DIG_OUTPUT();  // 14 : Audio output
    // Uart Trigger for transmission timing (pin 17, INT1)
    CONFIG_RB14_AS_DIG_INPUT();

    // Outputs for volume control
    CONFIG_RA3_AS_DIG_OUTPUT();
    CONFIG_RA2_AS_DIG_OUTPUT();


    _DSSR = 0;      // clear RELEASE bit
    _INT0IE = 0;    // only enabled when entering deep sleep mode
    _INT0 = 0;      // Clear Deep Sleep on INT0 edge flag
    _INT1IE = 0;    // only enabled after vario initialisation
    _INT1EP = 0;
    _INT1IP = 1;

    
    uart2_Config();
    uart1_Config();

    i2c_Config();
    tmr1_Config();
    tmr2_Config();
    tmr3_Config();

    button_Config();
    audio_Config();
#if PCB_VERSION == 1
    MMA8452_Init();
#endif
}

void INT1_callback(void) {
    //_INT1IE = 0; not needed
    timer1Cnt = 0;
}

void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void){
    _INT1IF = 0;
    INT1_callback();
}

void U1TX_callback(void){
    //check if the whole buffer has been sent (i.e.: character = '\0')
    if(*txBufPtr) {
        U1TXREG = *(txBufPtr++);   // transfer data byte to TX reg
    }
    else {
        // disable interrupt
        IEC0bits.U1TXIE = 0;
        // disable UART
        U1STAbits.UTXEN = 0;
        resetMutexBit (&mutex_8bits, MUTEX_TRANSMITTING_BIT);
        //_INT1IE = 1; not needed
    }
}

void tmr1_callback(void) {
    tmr1_Stop();
    u32 filtrdP = 0;

    if (tmr1_waiting) {
        tmr1_waiting = 0;
    } else {
        ClrWdt();
        _IDLE = 0;
        if (sns_status) {
            ///////////////////////////////////
            sns_ReadADC(&Praw);
            sns_TriggerTemperatureSample();
            /////////////////////////////////// 2ms
            tmr1_Start(MEASURE_PERIOD_T);
        } else {
            u32 Traw;
            s16 temperature;
            sns_ReadADC(&Traw);
            sns_TriggerPressureSample();
            tmr1_Start(MEASURE_PERIOD_P);
            temperature = sns_CompensateTemperature(Traw);            
            timer1Cnt = timer1Cnt + 1;
            s32 Press = sns_CompensatePressure(Praw);
            filtrdP = filterPressure(Press);
//            sprintf(dbgBuf, "Pressure : %ld\r\n", filtrdP);
//            uart_Print(dbgBuf);
            if (filtrdP != (s32)(0xFFFFFFFF)){
                vario_ping(filtrdP);
            }
            if ( (getMutexBit (mutex_8bits, MUTEX_NEW_DATA_BIT) == 0 )) { //if main() is ready to send another NMEA sentence, give it some values
                varioBuf = vario_get_speed();
                temperatureBuf = temperature;
                pressureBuf = filtrdP;
                setMutexBit (&mutex_8bits, MUTEX_NEW_DATA_BIT); // main() will see it when it has some free time
            }
            if (timer1Cnt == ctimer1Max_kobo) { //if this is the right time to send an NMEA sentence, tell it to main() through the mutex
                setMutexBit (&mutex_8bits, MUTEX_TIMETOSEND_BIT);                
            }
            else { // else, set the mutex to zero so main() will wait for the right time
                resetMutexBit (&mutex_8bits, MUTEX_TIMETOSEND_BIT);
            }
        }
        sns_status = !sns_status;        
    }
}

void tmr1_wait(u32 ms) {
    tmr1_Stop();
    tmr1_waiting = 1;
    tmr1_Start(10 * ms);
    while (tmr1_waiting){
        Nop();
    }
}

int main(void) {
    int i = 0;
    u8 checksum;

    mcu_Init();
    ClrWdt();
    sns_Reset();
    tmr1_wait(2000);
    audio_Beep(0);
    sns_Config();
    sns_status = 0;
    
#ifdef ELLE_AIME_LA_BITE
    u8 intro_i;
    for (intro_i = 0; intro_i < INTRO_LENGTH; intro_i++) {
        audio_Beep(intro_freqs[intro_i]);
        wait(intro_times[intro_i]);
    }
    audio_Beep(0);
#else
    audio_Beep(220);
    tmr1_wait(80);
    audio_Beep(1100);
    tmr1_wait(80);
    audio_Beep(2200);
    tmr1_wait(80);
    audio_Beep(0);
#endif
    tmr1_wait(500);
    // 2Hz GPS NMEA Update rate
#ifdef GPS_2Hz
    tmr1_wait(500);
    sprintf(dbgBuf, "$PMTK220,500*2B\r\n");
    uart_Print(dbgBuf);
    tmr1_wait(500);
#else
    sprintf(dbgBuf, "$PMTK220,1000*1F\r\n");
    uart_Print(dbgBuf);
    wait(500);
#endif
    // format NMEA string with dummy characters instead of numbers
    sprintf (koboTxBuf, "$D,VVVV,PPPPPP,,,TTTT,,,,*AA\r\n");
 
    sns_TriggerTemperatureSample();
    tmr1_Start(MEASURE_PERIOD_T);
    vario_init();
    _INT1IE = 1;
    Nop();
	
	// low priority (non realtime) actions are put in the following loop. It will
	// be interrupted all the time...
    while(1){
        if ((getMutexBit (mutex_8bits, MUTEX_NEW_DATA_BIT) == 1)) { //a new data is ready to be sent
            // fill NMEA string
            // "to_chars" functions return 1 (error) if number is not codable on the requested
            // number of digits. For instance this will be the case if pressure is out of bounds
            // (0xFFFFFFFF). In that case no serial frame will be sent to the Kobo!
            if (  (s16_to_chars_decimal (koboTxBuf + 3, 4, varioBuf)
                + u32_to_chars_decimal (koboTxBuf + 8, 6, pressureBuf)
                + s8_to_chars_decimal  (koboTxBuf + 17, 4, temperatureBuf) ) == 0 ) { //if no error on number conversions

                // compute checksum and add it to NMEA string
                i = 1;
                checksum = 0;
                while (koboTxBuf [i] != '*') {
                    checksum ^= koboTxBuf [i++];
                }
                i++;
                u8_to_chars_hexadecimal (koboTxBuf + i, checksum);

                // Set transmit buffer pointer to the first character to be sent
                txBufPtr = koboTxBuf;

                // wait until it is the right time to send the NMEA sentence
                while ((getMutexBit (mutex_8bits, MUTEX_TIMETOSEND_BIT) == 0)) {
                    Nop();
                }

                IEC0bits.U1TXIE = 1;
                U1STAbits.UTXEN = 1;

                // indicate start of transmission (will be reset by the U1TX interrupt routine when msg is fully sent)
                setMutexBit (&mutex_8bits, MUTEX_TRANSMITTING_BIT);

                U1TX_callback ();

                // wait until frame is fully sent
                while ((getMutexBit (mutex_8bits, MUTEX_TRANSMITTING_BIT) == 1)) {
                    Nop();
                }
            }
        }
		// reset mutex so that vario loop can update values with fresh ones
        resetMutexBit (&mutex_8bits, MUTEX_NEW_DATA_BIT);
    }

    return 0;
}
