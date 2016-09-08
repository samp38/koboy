#include "pic24_all.h"
#include "audio.h"
#include "vario.h"
#include "audio_response.h"
#include "button.h"

#define BEEP_DESCENT_PERIOD 500
#define SPEED_THRESHOLD 25
#define VARIO_SHUTDOWN_IN_SECONDS 180
#define VARIO_SHUTDOWN_COUNTER_LIMIT ((VARIO_SHUTDOWN_IN_SECONDS * SAMPLE_RATE)/SPEED_REFRESH_PERIOD_IN_SAMPLES_MAX)
#define VARIO_SHUTDOWN_SPEED_THRESHOLD 2
#define SPEED_REFRESH_PERIOD_IN_SAMPLES_MAX 8
#define MEASURE_BUFFER_SIZE 61

s16 shutdown_timeout;
u32 beep_timeout; // won't switch beeping until counter==0
u8 update_speed_timeout; // won't recalculate speed until counter==0

u8 beeping;

s32 filtrd_pressures[MEASURE_BUFFER_SIZE];     // Buffer of filtered samples
s16 filtrd_pressures_i;
s16 speed_index;
s16 speed_index_abs;
s16 speed_real;
s16 speed_real_avg;

s16 abval(s16 val) {
     return (val<0 ? (-val) : val);
}

inline void vario_reset_shutdown_counter(void) {
    shutdown_timeout = VARIO_SHUTDOWN_COUNTER_LIMIT;
}

void vario_shutdown_tick(void) {
    shutdown_timeout --;
    if (shutdown_timeout == 0) {
        //shutdown(); //TODO
    }
}

void vario_init(void) {
    beeping = 0;
    beep_timeout = 200;
    update_speed_timeout = 200;

    filtrd_pressures_i = 0;
    speed_index = 0;
    speed_index_abs = 0;

    vario_reset_shutdown_counter();
}

void vario_push_filtered(u32 filteredP) {
    filtrd_pressures_i++;
    if(filtrd_pressures_i == MEASURE_BUFFER_SIZE) {
        filtrd_pressures_i = 0;
    }
    filtrd_pressures[filtrd_pressures_i] = filteredP;
}

void vario_update_speed_index() {
    u8 integrateTime;
    s32 speedIIRFilterd;
    
    if (speed_index < 5) {integrateTime = 50;}
    else if (speed_index < 10) {integrateTime = 30;}
    else if (speed_index < 30) {integrateTime = 25;}
    else {integrateTime = 10;}
    
    s16 i = filtrd_pressures_i - integrateTime;
    if (i < 0) {
        i += MEASURE_BUFFER_SIZE;
    }
    s16 coef = ((14 * SAMPLE_RATE) / integrateTime);
    s32 speed = (filtrd_pressures[i] - filtrd_pressures[filtrd_pressures_i]) * coef;

    speedIIRFilterd = speedIIRFilter(speed);
    speed_index = (speedIIRFilterd - SPEED_THRESHOLD)/(2 * SPEED_STEP);
    speed_real = speedIIRFilterd/SPEED_STEP;
    speed_real_avg = speed_average(speed_real);
    speed_index_abs = abval(speed_index);
}

/**
 * Toggle beeping with new speed value
 */
void vario_beep_tick() {
    u32 beep_period;
    u32 beep_freq;
    
    if (speed_index < 0) {
        beep_freq = 0;
        beep_period = BEEP_DESCENT_PERIOD;
    }
    else {
        if (speed_index_abs > SPEED_INDEX_MAX) {
            speed_index = SPEED_INDEX_MAX;
            speed_index_abs = SPEED_INDEX_MAX;
        }
        beep_period = beepLen[speed_index_abs];
        beep_freq = audioFreq[speed_index];
    //  LP_Order = speedLowPassOrder[abval(speedIndex_current)];
    }
    // audio freuqnecy response : from 220 to 2200
    // Beep Period response : from 500 to 50

    if (beeping){
        audio_Beep(0);
        beep_timeout =  (( beep_period * SAMPLE_RATE) / 15000 );
        beeping = 0;
    } else {
        audio_Beep((beep_freq * 2) / 3);
        beeping = 1;
        beep_timeout = ((beep_period * SAMPLE_RATE) / 10000);
    }
}


void vario_ping(u32 filteredP) {

    vario_push_filtered(filteredP);
    
    if (update_speed_timeout-- == 0) {
        vario_update_speed_index();

        if (speed_index_abs < 50) {
            update_speed_timeout = 8;
        } else {
            update_speed_timeout = 5;
        }
        
//        if(speed_index_abs > VARIO_SHUTDOWN_SPEED_THRESHOLD) {
//            vario_reset_shutdown_counter();
//        } else {
//            vario_shutdown_tick();
//        }
    }

    if (beep_timeout-- == 0) {
        vario_beep_tick();
    }
}

s16 vario_get_speed(void) {
    return speed_real_avg;
}
