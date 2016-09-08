#include "pic24_all.h"
#include "timers.h"
#include "audio.h"
#include "vario.h"
#include "uart.h"
#include "timers.h"
#include <stdio.h>
#include "pcb_version.h"
#if PCB_VERSION == 1
#include "MMA8452.h"
#endif

#define SW1             _RB15           //switch state
#define SW1_PRESSED()   (SW1==0)        //switch test
#define SW1_RELEASED()  (SW1==1)        //switch test

#define LONG_PRESS_MIN_PERIOD 1000 // ticks
#define MIN_PRESS_PERIOD 20 // minimum tick count - otherwise it's considered as a boucing button
#define PRESS_TIMEOUT 1200      // TIMEOUT
#define ANTI_BOUNCE_PEROD 140    // to avoid bounces that occure when pushing button

char uartBuf[50];
u8 gpsOn = 1;
#if PCB_VERSION == 1
u8 doubleTap = 0;
#endif

void button_Config(void) {
    CONFIG_RB15_AS_DIG_INPUT();
    _CNIF = 0;  // clear CN Interrupt Flag
    ENABLE_RB15_CN_INTERRUPT();
    _CNIP = 2; // CN interruptions priority
    _CNIE = 1; //enable the Change Notification general interrupt
}


void sleepJingle(void) {
    audio_Beep(2200);
    tmr3_Wait(160);
    audio_Beep(1100);
    tmr3_Wait(160);
    audio_Beep(220);
    tmr3_Wait(160);
    audio_Beep(0);
}

void wakingJingle(void) {
    audio_Beep(220);
    tmr3_Wait(160);
    audio_Beep(1100);
    tmr3_Wait(160);
    audio_Beep(2200);
    tmr3_Wait(160);
    audio_Beep(0);
}


void buttonJingle(void) {
    audio_Beep(220);
    tmr3_Wait(100);
    audio_Beep(0);
    tmr3_Wait(100);
    audio_Beep(220);
    tmr3_Wait(100);
    audio_Beep(0);
    tmr3_Wait(100);
    audio_Beep(220);
    tmr3_Wait(100);
    audio_Beep(0);
}

void shutdown(void) {        
    // GPS sleep mode        
    // Jingle
    if (getVolState() == 0) {
        setNextVolume();
        sleepJingle();
        setPreviousVolume();
    }
    else { sleepJingle(); }
    tmr1_Stop();
    tmr2_Stop();              
    Nop();
    sprintf(uartBuf, "$PMTK161,0*28\r\n");
    uart_Print(uartBuf);
    tmr3_Wait(360);
    _CNIE = 1;
    _CN11IE = 1;
    gpsOn = 0;
    Sleep();        // Go to Sleep
    Nop();
}

void wakeup(void) {    
    sprintf(uartBuf, "$PMTK220,500*2B\r\n");
    uart_Print(uartBuf);
    // jingle
    if (getVolState() == 0) {
        setNextVolume();
        wakingJingle();
        setPreviousVolume();
    }
    else { wakingJingle(); }
    sns_TriggerTemperatureSample();
    tmr1_Start(MEASURE_PERIOD_T);
    vario_init();
    gpsOn = 1; 
    _INT1IE = 1;
}

#if PCB_VERSION == 0
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {
    _CNIF = 0;              // Clear Change Notification Interrupt Flag
    DISABLE_RB15_CN_INTERRUPT(); // Disable CN Interrupt for bounces
    // BUTTON HAS BEEN PRESSED
    // anti-bounce
    tmr3_Wait(ANTI_BOUNCE_PEROD);
    tmr3_Start();           // Start Timer for press duration measurement

    if (SW1_PRESSED()){
        while(SW1_PRESSED() && tmr3_Get() < PRESS_TIMEOUT) {Nop();}
        if (tmr3_Get() > LONG_PRESS_MIN_PERIOD) {
            // Long press -> Entering deep sleep mode
            shutdown();    
        } else {
            if(gpsOn == 0) {
                shutdown();
            }
            else {
                // Short press -> set new Volume
                vario_reset_shutdown_counter();
                setNextVolume();
                buttonJingle();
            }
            
        }
    }
    tmr3_Stop();
    ENABLE_RB15_CN_INTERRUPT();
}
#elif PCB_VERSION == 1

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {    
    _CNIF = 0;              // Clear Change Notification Interrupt Flag
    DISABLE_RB15_CN_INTERRUPT(); // Disable CN Interrupt for bounces
 
    if (SW1_PRESSED()) {
           doubleTap = MMA8452_getDoubleTap();
        if (doubleTap == 0 && gpsOn == 1) {
            setNextVolume();
            buttonJingle();
            vario_reset_shutdown_counter();    
        }
        else {
            if (gpsOn == 1) {
                setPreviousVolume();                                                
                MMA8452_enable_Z_doubleTap();
                shutdown();
            }
            else {
                MMA8452_enable_Z_singleANDdoubleTap();
                wakeup();
            }
        }
    }
//    else {
//        doubleTap = getDoubleTap();
//    }
    ENABLE_RB15_CN_INTERRUPT();
}

#endif

