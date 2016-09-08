/* 
 * File:   audio.h
 * Author: sam
 *
 * Created on 24 juin 2012, 13:37
 */

#ifndef AUDIO_H
#define	AUDIO_H

void audio_Config(void);
void audio_Beep(u32);
void setNextVolume(void);
void setPreviousVolume(void);
u8 getVolState(void);
void setVolume(u8 vol);

#endif	/* AUDIO_H */

