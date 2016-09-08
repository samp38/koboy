/* 
 * File:   i2c.h
 * Author: sam
 *
 * Created on 18 juin 2012, 15:28
 */
#include "pcb_version.h"
#ifndef I2C_H
#define	I2C_H

void i2c_Config(void);
void i2c_Start(void);
void i2c_Repeat_Start(void);
void i2c_Stop(void);
void i2c_Put(u8 val);
u8 i2c_Get(u8 ack2Send);
void i2c_ReadN(u8  addr,u8* pdata, u16  cnt);
void i2c_ReadN2(u8  addr,u8* pdata, u16  cnt);
void i2c_Write1(u8 addr,u8 d1);
void i2c_Write2(u8 addr,u8 d1, u8 d2);
void i2c_RcvBuf(u8 devId, u8 addr, int nBytes, u8* pBuf);

//#if PCB_VERSION == 1
void i2c_Write1_MMA845X(u8 addr,u8 d1);
void i2c_RcvBuf_MMA845X(u8 devId, u8 addr, int nBytes, u8* pBuf);
//#endif
#endif	/* I2C_H */

