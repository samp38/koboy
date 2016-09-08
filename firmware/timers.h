/* 
 * File:   timers.h
 * Author: sam
 *
 * Created on 18 juin 2012, 16:16
 */

#ifndef TIMERS_H
#define	TIMERS_H


/* you should implement these :
void tmr1_callback(void);
*/


//// TIMER 1 : triggers sensor reads
void tmr1_Config(void);
void tmr1_Start(u32 uSx100);
void tmr1_Stop();

//// TIMER 2 : beep driver
void tmr2_Config(void);
void tmr2_Start(void);
void tmr2_Stop(void);

//// TIMER 3 : simple counter
void tmr3_Config(void);
void tmr3_Start();
unsigned int tmr3_Get(void);
void tmr3_Stop(void);
void tmr3_Wait(u32 ticks);

#endif	/* TIMERS_H */
