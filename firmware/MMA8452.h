/* 
 * File:   MMA8452.h
 * Author: sam
 *
 * Created on February 21, 2016, 7:56 PM
 */

#ifndef MMA8452_H
#define	MMA8452_H

void MMA8452_Init(void);
u8 MMA8452_getDoubleTap(void);
void MMA8452_enable_Z_singleTap(void);
void MMA8452_enable_Z_doubleTap(void);
void MMA8452_enable_Z_singleANDdoubleTap(void);

#endif	/* MMA8452_H */

