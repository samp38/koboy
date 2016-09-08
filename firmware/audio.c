#include "pic24_all.h"
#include "i2c.h"

#define EN1 _RA3
#define EN2 _RA2

u8 volState = 0;

void setHighVolume(void) {
    EN1 = 1;
    Nop();Nop();Nop();Nop();Nop();
    EN2 = 1;
    Nop();Nop();Nop();Nop();Nop();
    volState = 3;
}
void setMediumVolume(void) {
    EN1 = 1;
    Nop();Nop();Nop();Nop();Nop();
    EN2 = 0;
    Nop();Nop();Nop();Nop();Nop();
    volState = 2;
}
void setLowVolume(void) {
    EN1 = 0;
    Nop();Nop();Nop();Nop();Nop();
    EN2 = 1;
    Nop();Nop();Nop();Nop();Nop();
    volState = 1;
}
void setNoVolume(void) {
    EN1 = 0;
    Nop();Nop();Nop();Nop();Nop();
    EN2 = 0;
    Nop();Nop();Nop();Nop();Nop();
    volState = 0;
}


void setNextVolume(void) {
    switch (volState) {
        case 0:
            setLowVolume();
            break;
        case 1:
            setMediumVolume();
            break;
        case 2:
            setHighVolume();
            break;
        case 3:
            setNoVolume();
            break;
        default:
            Nop();
    }
}

void setPreviousVolume(void) {
    switch (volState) {
        case 0:
            setHighVolume();
            break;
        case 1:
            setNoVolume();
            break;
        case 2:
            setLowVolume();
            break;
        case 3:
            setMediumVolume();
            break;
        default:
            Nop();
    }

}

void setVolume(u8 vol) {
    switch (vol) {
        case 0:
            setNoVolume();
            break;
        case 1:
            setLowVolume();
            break;
        case 2:
            setMediumVolume();
            break;
        case 3:
            setHighVolume();
            break;
        default:
            Nop();
    }
}

void audio_Config(void) {
    OC1CONbits.OCTSEL = 0;  // use TMR2
    OC1CONbits.OCSIDL = 0;  // Continue in Idle Mode
    Nop();Nop();Nop();Nop();Nop();
    _C1EVT = 0; //clear C1 event flag
    _CMIF = 0;  //clear interrupt flag

// Set start volume
    setNoVolume();
}



void audio_Beep(u32 freq){
    u16 tmrTicks;
    OC1CONbits.OCM = 0; // disable ocm
    tmr2_Stop();

    if (freq) {
        tmrTicks = (u16)((FCY>>3)/freq);
        
        // square wave:
//        OC1R = (tmrTicks >> 1);
        OC1RS = (tmrTicks >> 1);

        tmr2_Start(tmrTicks);

        OC1CONbits.OCM = 6; // PWM Mode
    } // else : the silencious beep
}

u8 getVolState(void) {
    return volState;
}


