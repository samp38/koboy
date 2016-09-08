#include "pic24_all.h"


// ceofficients for speed flter (maxflat Wn = 0.01)
#define B1_0 73
#define A1_0 1000
#define A1_1 854


// feedforward coefficients
#define B_0     16
#define B_1  	32
#define B_2  	16
// feedback coefficients,
#define A_0 	2048
#define A_1  	-3553
#define A_2  	1569

#define INTEGER_FACTOR 11       // filter Value will be multiplied by 2ÎNTEGER_FACTOR for precision
#define averageFilterSamplesNb  4
#define averageFilterPower      2

// if pressure is greater than this value, it shall be discarded
#define PRESSURE_NIMP_THRESHOLD_L ((s32)(10000))
#define PRESSURE_NIMP_THRESHOLD_H ((s32)(500000))


s64 x_1 = 200000000LL;
s64 x_2 = 200000000LL;
s64 y_1 = 200000000LL;
s64 y_2 = 200000000LL;

s16 speedsSum = 0;


s32 filterPressure(s32 x) {
    s64 xBIG ;
    s64 y ;

    if ((x < PRESSURE_NIMP_THRESHOLD_L) || (x > PRESSURE_NIMP_THRESHOLD_H)) {
        return (s32)(0xFFFFFFFF);
    }

    xBIG = x << INTEGER_FACTOR;
    y = ((s64)B_0*(s64)xBIG) + ((s64)B_1*x_1) + ((s64)B_2*x_2) - ((s64)A_1*y_1) - ((s64)A_2*y_2);

    x_2 = x_1;
    x_1 = xBIG;

    y_2 = y_1;
    y_1 = y/A_0;

    return (s32)(y_1 >> INTEGER_FACTOR);
}


s32 y1_1 = 0L;
s32 x1_1 = 0L;

// 1st order IIR low pass for speed
s32 speedIIRFilter(s32 x) {
    s32 y = (B1_0 * (x + x1_1)) + (A1_1 * y1_1);
    x1_1 = x;
    y1_1 = y/(A1_0);
    return y1_1;
}

s16 speed_average(s16 speed) {
    speedsSum = speedsSum + speed - (speedsSum >> averageFilterPower);
    return speedsSum >> averageFilterPower;
}
