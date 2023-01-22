#ifndef MIND_STONE_H
#define MIND_STONE_H

#include <BLEDevice.h>
#include <BLEServer.h>

#define BLUETOOTH_NAME "BLORGI"
#define SERVICE_UUID "b8fdd891-3087-401a-b7f8-644b3c413222"
#define TIME_UUID "1f23bbc2-3e6e-4f90-bc11-8cf77436aa38"
#define MODE_UUID "23f62672-7dbb-4160-8b3c-1cf09ab9ac60"

extern BLECharacteristic* time_elapsed_CHAR;
extern BLEDescriptor* time_elapsed_DESC;
extern BLECharacteristic* mode_CHAR;
extern BLEDescriptor* mode_DESC;
extern bool deviceConnected;
extern char mode[2];
extern uint8_t mode_i;

void init_ble();

char get_mode();

void set_mode(char mode_);

#endif