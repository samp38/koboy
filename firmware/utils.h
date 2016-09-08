/* 
 * File:   utils.h
 * Author: sam
 *
 * Created on 23 février 2013, 23:12
 */

#ifndef UTILS_H
#define	UTILS_H

void fill_rolling_array(s32 array[], u8 index_current, u8 index_max, s32 value);
s32 get_nLastSamples_sum(s32 array[], u8 index_current, u8 index_max, u8 order);

#endif	/* UTILS_H */

