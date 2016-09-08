#include "pcb_version.h"
#if PCB_VERSION == 1
#include "pic24_all.h"
#include "i2c.h"
#include "uart.h"
#include <stdio.h>

#define I2C_ID_MMA8452  ((u8)0b00111010)
#define WHO_AM_I        0x0D
#define CTRL_REG1       0x2A
#define DATA_RATE       2       // 0=800Hz, 1=400, 2=200, 3=100, 4=50, 5=12.5, 6=6.25, 7=1.56
#define FSR             8       // Full Scale Range, 2, 4 or 8g


void MMA8452_Standby(void) {
    u8 standby;
    i2c_RcvBuf_MMA845X(I2C_ID_MMA8452, CTRL_REG1, 1, &standby);
    standby &= ~(0x01);
    i2c_Write2(I2C_ID_MMA8452, CTRL_REG1, standby);
}

void MMA8452_Activate(void) {
    u8 activate;
    i2c_RcvBuf_MMA845X(I2C_ID_MMA8452, CTRL_REG1, 1, &activate);
    activate |= 0x01;
    i2c_Write2(I2C_ID_MMA8452, CTRL_REG1, activate); 
}

u8 MMA8452_getDoubleTap(void) {
    u8 d_tap;
    u8 caca;
    u8 i = 0;
    char dbgBuffer2[10];
    i2c_RcvBuf_MMA845X(I2C_ID_MMA8452, 0x22, 1, &d_tap);
    d_tap = (d_tap & 0b00001000) >> 3;
    
//    for (i=0x00;i<=0x31;i++) {    
//        i2c_RcvBuf_MMA845X(I2C_ID_MMA8452, i, 1, &caca);
//        sprintf(dbgBuffer2,"%02x : %d\r\n",i,caca);
//        uart_Print(dbgBuffer2);
//    }
    
    return d_tap;
}

void MMA8452_checkWAI(void){
    u8 ID = 0;
    char dbgBuffer[50];
    i2c_RcvBuf_MMA845X(I2C_ID_MMA8452, WHO_AM_I, 1, &ID);
    if (ID == 0x2A) {sprintf(dbgBuffer,"MMA8452 en ligne\n");}
    else {sprintf(dbgBuffer,"Erreur de com avec MMA8452\n");}
    uart_Print(dbgBuffer);
}

void MMA8452_enable_Z_singleTap(void) {
    MMA8452_Standby();
    i2c_Write2(I2C_ID_MMA8452, 0x21, 0b00010000); //Enable Z Single Pulse Interrupt
    MMA8452_Activate(); 
}

void MMA8452_enable_Z_doubleTap(void) {
    MMA8452_Standby();
    i2c_Write2(I2C_ID_MMA8452, 0x21, 0b00100000); //Enable Z Double Pulse Interrupt
    MMA8452_Activate(); 
}

void MMA8452_enable_Z_singleANDdoubleTap(void) {
    MMA8452_Standby();
    i2c_Write2(I2C_ID_MMA8452, 0x21, 0b00110000); //Enable Z Single & double Pulse Interrupt
    MMA8452_Activate(); 
}

void MMA8452_Init() {
    u8 temp;
    MMA8452_Standby();
    MMA8452_checkWAI(); // check if sensor is online
    
    // Set DataRate & scale range
        // Setup the 3 data rate bits, from 0 to 7
    i2c_RcvBuf_MMA845X(I2C_ID_MMA8452, CTRL_REG1, 1, &temp);
    i2c_Write2(I2C_ID_MMA8452, CTRL_REG1, temp & ~(0x38));
    if (DATA_RATE <= 7) {
        i2c_RcvBuf_MMA845X(I2C_ID_MMA8452, CTRL_REG1, 1, &temp);
        i2c_Write2(I2C_ID_MMA8452, CTRL_REG1, temp | (DATA_RATE << 3));
    }
        // Set FSR
    i2c_Write2(I2C_ID_MMA8452, 0x0E, FSR >> 2);
    
    i2c_Write2(I2C_ID_MMA8452, 0x21, 0b00110000); //Enable Z Single & double Pulse Interrupt
    i2c_Write2(I2C_ID_MMA8452, 0x25, 0x7E); //Set Z Threshold (0.063g/count)
    i2c_Write2(I2C_ID_MMA8452, 0x26, 0x08); //(1.25ms@200Hz)Set Time Limit for Tap Detection to 100 ms, Normal Mode, No LPF
    i2c_Write2(I2C_ID_MMA8452, 0x27, 0x30); //(2.5ms@200Hz)Set Latency Time to 120 ms    
    i2c_Write2(I2C_ID_MMA8452, 0x28, 0x78); // (2.5ms@200Hz)Set Time Window for double tap 300ms
    
    i2c_Write2(I2C_ID_MMA8452, 0x2D, 0x08); //Enable Pulse Interrupt Block in System CTRL_REG4 and sleep interrupt
    i2c_Write2(I2C_ID_MMA8452, 0x2E, 0x08); //Route Pulse Interrupt Block to INT1 hardware Pin CTRL_REG5
            
    MMA8452_Activate();    
}
#endif