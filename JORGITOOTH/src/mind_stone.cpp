#include <Arduino.h>
#include "mind_stone.h"

BLECharacteristic* time_elapsed_CHAR = new BLECharacteristic(TIME_UUID, BLECharacteristic::PROPERTY_READ);
BLEDescriptor* time_elapsed_DESC = new BLEDescriptor(BLEUUID((uint16_t)0x2901));

BLECharacteristic* mode_CHAR = new BLECharacteristic(MODE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor* mode_DESC = new BLEDescriptor(BLEUUID((uint16_t)0x2901));

bool deviceConnected = false;
char mode[2] = "C";

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

    service->start();

    // Start advertising
    BLEAdvertising *advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    server->getAdvertising()->start();

    // Initialize mode Characteristic value
    mode_CHAR->setValue(mode);
}

char get_mode() {
    return mode[0];
}