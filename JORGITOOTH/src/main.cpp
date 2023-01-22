#include <Arduino.h>
#include "time_stone.h"
#include "mind_stone.h"

#define HEAD 25
#define DECR 26
#define INCR 27

void setup() {
  pinMode(HEAD, INPUT_PULLDOWN);
  pinMode(INCR, INPUT_PULLDOWN);
  pinMode(DECR, INPUT_PULLDOWN);
  attachInterrupt(HEAD, &start_timer, RISING);
  attachInterrupt(INCR, &time_incr, RISING);
  attachInterrupt(DECR, &time_decr, RISING);

  init_timer();
  init_ble();

  Serial.begin(115200);
  Serial.println("Bluetooth pairing...");
}

void loop() {
  if (deviceConnected) {
    uint32_t value = time_left;
    time_elapsed_CHAR->setValue(value);
  }
}