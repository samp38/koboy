/* 
 * File:   sensor.h
 * Author: sam
 *
 * Created on 18 juin 2012, 16:38
 *
 * The sensor is a MS5611
 *      Pressure is delivered as integer with 0.01mbar resolution
 *      In the low atmosphere, we can assume that 0.125 mbar represent 1 meter. So 1 meter is represented
 *      by a pressure variation of 12.5 (probe value).
 *
 */

#ifndef SENSOR_H
#define	SENSOR_H

void sns_Reset(void);
void sns_Config(void);
s16  sns_CompensateTemperature(u32);
s32  sns_CompensatePressure(u32);
void sns_TriggerTemperatureSample(void);
void sns_TriggerPressureSample(void);
void sns_ReadAdc(s32 *);

#endif	/* SENSOR_H */

