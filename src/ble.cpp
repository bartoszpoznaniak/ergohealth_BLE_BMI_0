#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "ble.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;

void setupBLE()
{
    BLEDevice::init(BLE_DEVICE_NAME);
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristic->setValue("Hello World says Neil");
    pService->start();

    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);

    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);

    BLEDevice::startAdvertising();

    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loopBLE()
{
    int value = 60 + (uint8_t)(esp_random() % 21);
    char valueStr[8];
    snprintf(valueStr, sizeof(valueStr), "%d", value);

    pCharacteristic->setValue(valueStr);
    pCharacteristic->notify();

    Serial.print("Wysłano przez BLE: ");
    Serial.println(valueStr);

    // Hero send value via BLE
    delay(1000);
}