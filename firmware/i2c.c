#include "pic24_all.h"
#include "pcb_version.h"

#define I2C_ACK 0
#define I2C_NAK 1
#define I2C_WADDR(x) ((x) & 0xFE) //clear R/W bit of I2C addr
#define I2C_RADDR(x) ((x) | 0x01) //set R/W bit of I2C addr

void i2c_Config(void) {
    I2C1BRG = 0;
    I2C1CONbits.I2CEN = 1;
}

void i2c_Start(void) {
    I2C1CONbits.SEN = 1;   // initiate start
    // wait until start finished
    while (I2C1CONbits.SEN);
}

void i2c_Repeat_Start(void) {
    I2C1CONbits.RSEN = 1;   // initiate restart
    // wait until start finished
    while (I2C1CONbits.RSEN);
}

void i2c_Stop(void) {
    I2C1CONbits.PEN=1;     // initiate stop, PEN=1
    //wait until stop finished
    while (I2C1CONbits.PEN);
}

void i2c_Put(u8 val) {
    I2C1TRN = val;    // write byte
    while (I2C1STATbits.TRSTAT);   // wait for 8bits+ ack bit to finish
}


u8 i2c_Get(u8 ack2Send) {
    u8 inByte;
    while (I2C1CON & 0x1F);           //wait for idle condition
    I2C1CONbits.RCEN = 1;             //enable receive
    while (!I2C1STATbits.RBF);        //wait for receive byte
    inByte = I2C1RCV;              //read byte;
    //wait for idle condition before attempting ACK
    while (I2C1CON & 0x1F);           //lower 5 bits must be 0
    I2C1CONbits.ACKDT = ack2Send;  //ACK bit to send back on receive
    I2C1CONbits.ACKEN = 1;            //enable ACKbit transmission
    while (I2C1CONbits.ACKEN);        //wait for completion
    return(inByte);                  //return the value
}

void i2c_ReadN(u8  addr,u8* pdata, u16  cnt) {
    u16  inx;
    i2c_Start();
    i2c_Put(I2C_RADDR( addr));
    for (inx = 0;  inx <  cnt-1;  inx++) {
        *pdata++ = i2c_Get(I2C_ACK);
    }
    *pdata = i2c_Get(I2C_NAK);
    i2c_Stop();
}

void i2c_ReadN_MMA845X(u8  addr,u8* pdata, u16  cnt) {
    u16  inx;
    i2c_Repeat_Start();
    i2c_Put(I2C_RADDR( addr));
    for (inx = 0;  inx <  cnt-1;  inx++) {
        *pdata++ = i2c_Get(I2C_ACK);
    }
    *pdata = i2c_Get(I2C_NAK);
    i2c_Stop();
}

void i2c_Write1(u8 addr,u8 d1) {
    i2c_Start();
    i2c_Put(I2C_WADDR(addr));
    i2c_Put(d1);
    i2c_Stop();
}

void i2c_Write2(u8 addr,u8 d1, u8 d2) {
    i2c_Start();
    i2c_Put(I2C_WADDR( addr));
    i2c_Put( d1);
    i2c_Put( d2);
    i2c_Stop();
}

void i2c_RcvBuf(u8 devId, u8 addr, int nBytes, u8* pBuf) {
    i2c_Write1(devId,addr);
    i2c_ReadN(devId, pBuf, nBytes);
}
#if PCB_VERSION == 1
void i2c_Write1_MMA845X(u8 addr,u8 d1) {
    i2c_Start();
    i2c_Put(I2C_WADDR(addr));
    i2c_Put(d1);
}

void i2c_RcvBuf_MMA845X(u8 devId, u8 addr, int nBytes, u8* pBuf) {
    i2c_Write1_MMA845X(devId,addr);
    i2c_ReadN_MMA845X(devId, pBuf, nBytes);
}
#endif

