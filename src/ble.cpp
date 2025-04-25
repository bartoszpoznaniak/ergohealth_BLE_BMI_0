#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "ble.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_MAIN "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_BG "c0647d3b-e868-4281-9fe0-b02cd48af2e8"

BLECharacteristic *pCharacteristicBG;
BLECharacteristic *pCharacteristicMain;

namespace BLE
{
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    bool connected = false;
    float maxSensitivity = 10.0;
    float sensitivity = 1.0;
    std::function<void()> resetCallback = nullptr;
    std::function<void(unsigned long)> _setBadPostureTimeCallback = nullptr;
    std::function<void(float)> _setPostureThresholdCallback = nullptr;

    class MyServerCallbacks : public BLEServerCallbacks
    {
        void onConnect(BLEServer *pServer) override
        {
            Serial.println("🔗 BLE connected");
            connected = true;
        }

        void onDisconnect(BLEServer *pServer) override
        {
            connected = false;

            Serial.println("❌ BLE disconnected");
            // Wznawiamy reklamowanie
            BLEDevice::startAdvertising();
            Serial.println("📢 Advertising restarted");
        }
    };

    class MyCharacteristicCallbacks : public BLECharacteristicCallbacks
    {
        void onWrite(BLECharacteristic *pChar) override
        {
            std::string rxValue = pChar->getValue();

            if (rxValue.length() != 5)
            {
                Serial.println("Invalid data length");
                return;
            }

            char prefix = rxValue[0];
            float value;
            memcpy(&value, rxValue.data() + 1, sizeof(float));

            Serial.print("Received: ");
            Serial.print(prefix);
            Serial.print(" -> ");
            Serial.println(value);

            // Reakcja na różne prefixy
            switch (prefix)
            {
            case 'S':
                Serial.print("🔧 Sensitivity set to: ");
                Serial.println(value);
                sensitivity = value;
                break;

            case 'R':
                if (resetCallback)
                {
                    resetCallback();
                }
                break;

            case 'T':
                if (_setBadPostureTimeCallback)
                {
                    Serial.print("⏱️ Bad posture time set to: ");
                    Serial.print(value);
                    Serial.println(" minutes");
                    // Konwertujemy minuty na milisekundy
                    _setBadPostureTimeCallback(value * 60000);
                }
                break;

            case 'P': // P jak Posture threshold
                if (_setPostureThresholdCallback)
                {
                    Serial.print("📏 Posture threshold set to: ");
                    Serial.print(value);
                    Serial.println(" degrees");
                    _setPostureThresholdCallback(value);
                }
                break;

            default:
                Serial.println("❓ Unknown prefix");
                break;
            }
        }
    };

    void setupBLE()
    {
        BLEDevice::init(BLE_DEVICE_NAME);
        BLEServer *pServer = BLEDevice::createServer();
        pServer->setCallbacks(new MyServerCallbacks());

        BLEService *pService = pServer->createService(SERVICE_UUID);

        // Main characteristic
        pCharacteristicMain = pService->createCharacteristic(
            CHARACTERISTIC_UUID_MAIN,
            BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE |
                BLECharacteristic::PROPERTY_NOTIFY);

        pCharacteristicMain->setCallbacks(new MyCharacteristicCallbacks());

        // Background characteristic
        pCharacteristicBG = pService->createCharacteristic(
            CHARACTERISTIC_UUID_BG,
            BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_NOTIFY);

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
        float sensitivityRatio = sensitivity / maxSensitivity;
        float normalizedX = x * sensitivityRatio;
        float normalizedY = y * sensitivityRatio;
        float normalizedZ = z * sensitivityRatio;

        uint8_t values[12];
        memcpy(values, &normalizedX, sizeof(normalizedX));
        memcpy(values + 4, &normalizedY, sizeof(normalizedY));
        memcpy(values + 8, &normalizedZ, sizeof(normalizedZ));

        pCharacteristicMain->setValue(values, sizeof(values));
        pCharacteristicMain->notify();
    }

    void setResetCallback(std::function<void()> callback)
    {
        resetCallback = callback;
    }

    void setBadPostureTimeCallback(std::function<void(unsigned long)> callback)
    {
        _setBadPostureTimeCallback = callback;
    }

    void setPostureThresholdCallback(std::function<void(float)> callback)
    {
        _setPostureThresholdCallback = callback;
    }

    bool isConnected()
    {
        return connected;
    }

    void sendNotification(String text)
    {
        pCharacteristicBG->setValue(text.c_str());
        pCharacteristicBG->notify();
    }
}