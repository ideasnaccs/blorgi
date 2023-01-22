#include <Arduino.h>
#include "mind_stone.h"

BLECharacteristic* time_elapsed_CHAR = new BLECharacteristic(TIME_UUID, BLECharacteristic::PROPERTY_READ);
BLEDescriptor* time_elapsed_DESC = new BLEDescriptor(BLEUUID((uint16_t)0x2901));

BLECharacteristic* mode_CHAR = new BLECharacteristic(MODE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor* mode_DESC = new BLEDescriptor(BLEUUID((uint16_t)0x2901));

bool deviceConnected = false;
char mode[2] = "E";
uint8_t mode_i = 0;

/*
 *  Server callbacks when device connects/disconnects
 */
class CorgiCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* server) {
    Serial.println("Connected.");
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* server) {
    deviceConnected = false;
    server->startAdvertising();
    Serial.println("Advertising...");
  }
};

/*
 * Handles writing to "mode" Characteristic
 */
class CharacteristicCallBack : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic_)
  {
    mode[0] = characteristic_->getValue()[0];
  }
};

/*
 * Initializes BLE device, server, characteristics, descriptors
 * Begins server advertising
 */
void init_ble() {
    BLEDevice::init(BLUETOOTH_NAME);
    BLEServer *server = BLEDevice::createServer();
    server->setCallbacks(new CorgiCallbacks());

    BLEService *service = server->createService(SERVICE_UUID);
    
    // Time Elapsed Characteristic
    service->addCharacteristic(time_elapsed_CHAR);
    time_elapsed_DESC->setValue("Time Elapsed (s)");
    time_elapsed_CHAR->addDescriptor(time_elapsed_DESC);

    // Mode Characteristic
    service->addCharacteristic(mode_CHAR);
    mode_DESC->setValue("Mode");
    mode_CHAR->addDescriptor(mode_DESC);
    mode_CHAR->setCallbacks(new CharacteristicCallBack());

    service->start();

    // Start advertising
    BLEAdvertising *advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    server->getAdvertising()->start();

    // Initialize mode Characteristic value
    mode_CHAR->setValue(mode);
}

/*
 * Reads "mode" value (as char)
 */
char get_mode() {
    return mode[0];
}

/*
 * Modes:
 * Exercise (E)
 * Chores/Work (E)
 * Studying (S)
 * Care (C)
 */
void switch_mode() {
  static char MODES[] = "EWSC";
  if (mode_i < 4) mode_i++;
  else mode_i = 0;

  mode[0] = MODES[mode_i];
  mode_CHAR->setValue(mode);
}