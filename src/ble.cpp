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
    float x = random(-100, 101) / 10.0; // random() zwraca liczbę całkowitą, dzielimy przez 10 aby uzyskać wartości zmiennoprzecinkowe w zakresie -10 do 10
    float y = random(-100, 101) / 10.0;
    float z = random(-100, 101) / 10.0;

    uint8_t values[12]; // 3 * 4 = 12 bajtów

    // Kopiujemy wartości float do tablicy
    memcpy(values, &x, sizeof(x));     // Kopiujemy x
    memcpy(values + 4, &y, sizeof(y)); // Kopiujemy y (od miejsca 4 w tablicy)
    memcpy(values + 8, &z, sizeof(z)); // Kopiujemy z (od miejsca 8 w tablicy)

    // Ustawiamy tablicę jako wartość charakterystyki
    pCharacteristic->setValue(values, sizeof(values));
    pCharacteristic->notify();

    Serial.printf("Wysłano przez BLE: %f, %f, %f\n", x, y, z);

    // Hero send value via BLE
    delay(1000);
}