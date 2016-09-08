#include "pic24_all.h"

// implemented in user code
void tmr1_callback(void);

void tmr1_Config(void) {
    T1CONbits.TON = 0;    // turn off the timer
    T1CONbits.TCS = 0;    // select internal clock
    T1CONbits.TGATE = 0;  // gate disabled
    T1CONbits.TSIDL = 0;  // continue in idle mode
    T1CONbits.TCKPS1 = 0; // Prescaler 1:8
    T1CONbits.TCKPS0 = 1; // Prescaler 1:8
    PR1 = 65535;
    TMR1  = 0;            //clear timer1 value
    _T1IF = 0;            //clear interrupt flag
    _T1IP = 2;            //choose a priority
    _T1IE = 0;            //disable the interrupt
}

void tmr1_Start(u32 uSx100) {
    T1CONbits.TON = 0; //turn off the timer

    PR1  = (u16)(((uSx100 * FCY)/10000L)>>3); // timer period
    TMR1 = 0; // reset counter

    T1CONbits.TON = 1; //turn on the timer
    _T1IE = 1;            //enable the interrupt
}

void tmr1_Stop(void) {
    T1CONbits.TON = 0;
    TMR1 = 0;
    _T1IE = 0;            //disable the interrupt
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    _T1IF = 0; //clear the timer interrupt bit
    tmr1_callback();
}

// Timer 2 used for generating audio tone square wave
void tmr2_Config(void) {
    T2CONbits.T32 = 0; // 16bit timer
    T2CONbits.TCS = 0; //  select internal clock
    T2CONbits.TGATE = 0; // gate disabled
    T2CONbits.TSIDL = 0; // continue in idle mode
    T2CONbits.TCKPS1 = 0; // Prescaler 1:8
    T2CONbits.TCKPS0 = 1; // Prescaler 1:8
    _T2IP = 1;   //choose a priority
    T2CONbits.TON = 0; // stop tmr2
    TMR2 = 0;
    _T2IE = 0; // disable the interrupt
}

void tmr2_Start(u16 ticks){
    PR2 = ticks;
    T2CONbits.TON = 1;
}

void tmr2_Stop(void) {
    T2CONbits.TON = 0;
    TMR2 = 0;
}

// Timer 3 used for periodic measures
void tmr3_Config(void) {
    T2CONbits.T32 = 0; // 16bit timer
    T3CONbits.TCS = 0; //  select internal clock
    T3CONbits.TGATE = 0; // gate disabled
    T3CONbits.TSIDL = 0; // continue in idle mode
    T3CONbits.TCKPS1 = 1; // Prescaler 1:256
    T3CONbits.TCKPS0 = 1; // Prescaler 1:256
    _T3IP = 1;   //choose a priority
    T3CONbits.TON = 0; // stop tmr3
    TMR3 = 0;
    _T3IE = 0; // disable the interrupt
}

void tmr3_Start(void){
    T3CONbits.TON = 0; //turn off the timer
    TMR3 = 0; // reset counter
    T3CONbits.TON = 1; //turn on the timer
}

inline unsigned int tmr3_Get(void) {
    return TMR3;
}

void tmr3_Stop(void) {
    T3CONbits.TON = 0;
    TMR3 = 0;
    _T3IE = 0; // disable the interrupt
}


//1 tick = 0.5ms
void tmr3_Wait(u32 ticks) {
    tmr3_Start();
    while(tmr3_Get() < ticks) {
        Nop();
    }
    tmr3_Stop();
}
