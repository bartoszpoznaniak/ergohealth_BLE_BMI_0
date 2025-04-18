#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "ble.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;

namespace BLE
{
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

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

    void setValues(float newX, float newY, float newZ)
    {
        x = newX;
        y = newY;
        z = newZ;

        notifyBLE();
    }

    void notifyBLE()
    {
        uint8_t values[12];                // 3 * 4 = 12 bajtów
        memcpy(values, &x, sizeof(x));     // Kopiujemy x
        memcpy(values + 4, &y, sizeof(y)); // Kopiujemy y (od miejsca 4 w tablicy)
        memcpy(values + 8, &z, sizeof(z)); // Kopiujemy z (od miejsca 8 w tablicy)

        pCharacteristic->setValue(values, sizeof(values));
        pCharacteristic->notify();
    }
}