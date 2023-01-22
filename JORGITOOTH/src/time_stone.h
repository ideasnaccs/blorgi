#ifndef TIME_STONE_H
#define TIME_STONE_H

#include <Arduino.h>

// Multiplied by time_segments to scale to seconds
#define TIME_SCALER 600

extern hw_timer_t *timer;

extern portMUX_TYPE time_left_MUX;

extern volatile bool timer_active;

extern volatile unsigned int time_left;

extern unsigned int time_segments;

void init_timer();

void IRAM_ATTR onTimerDone();

void time_decr();

void time_incr();

void toggle_timer();

#endif