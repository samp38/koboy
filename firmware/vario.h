#ifndef VARIO_H
#define	VARIO_H

/**
 * This module will trigger climbing beeps and auto-sleep.
 *
 * Call vario_ping(s32 pressureSample) every MEASURE_PERIOD_P + MEASURE_PERIOD_T
 */

#define MEASURE_PERIOD_P 130
#define MEASURE_PERIOD_T 11   // With these value --> fech = 50 Hz
#define SAMPLE_RATE 50        // OSCILLOSCOPE MEASURED VALUE

void vario_init(void);
void vario_ping(u32 filteredP);
void vario_reset_shutdown_counter(void);
s16  vario_get_speed(void);

#endif	/* VARIO_H */

