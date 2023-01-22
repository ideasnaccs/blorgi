#include "time_stone.h"
#include <Arduino.h>

hw_timer_t *timer = NULL;

portMUX_TYPE time_left_MUX = portMUX_INITIALIZER_UNLOCKED;

volatile bool timer_active = false;

volatile unsigned int time_left = 0;

unsigned int time_segments = 0;

/*
 * Hardware timer interrupt handler
 * Decrements remaining time OR disables "timer_active" if finished
 */ 
void IRAM_ATTR onTimerDone() {
  if (time_left > 0) {

    portENTER_CRITICAL_ISR(&time_left_MUX);
    time_left--;
    portEXIT_CRITICAL_ISR(&time_left_MUX);
  }
  else if (timer_active) {
    timer_active = false;
  }
}

/*
 * Initializes 1Hz timer clock
 */
void init_timer() {
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimerDone, true);
}

/*
 * Decrements "time_segments" value
 */
void time_decr() {
  if (time_segments > 0) {
    time_segments--;
  }
}

/*
 * Increments "time_segments" value
 */
void time_incr() {
  if (time_segments < 12) {
    time_segments++;
  }
  else {
    time_segments = 0;
  }
}

/*
 * Toggles 1Hz timer
 */
void toggle_timer() {
  if (timer_active) {
    portENTER_CRITICAL_ISR(&time_left_MUX);
    time_left = 0;
    portEXIT_CRITICAL_ISR(&time_left_MUX);
    timer_active = false;
  }
  else {
    // Resets time elapsed
    portENTER_CRITICAL_ISR(&time_left_MUX);
    time_left = time_segments * TIME_SCALER;
    portEXIT_CRITICAL_ISR(&time_left_MUX);

    // Enable timer state
    timer_active = true;

    // Count up to 1M microseconds = 1 second
    timerAlarmWrite(timer, 1000000, true);
    timerAlarmEnable(timer);
    // Phase correction
    timerRestart(timer);
  }
  
}