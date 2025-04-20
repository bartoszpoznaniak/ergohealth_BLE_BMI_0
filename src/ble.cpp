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
    bool connected = false;
    float maxSensitivity = 10.0;
    float sensitivity = 1.0;

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

            case 'X':
                Serial.print("🧭 X set to: ");
                Serial.println(value);
                break;

            // Dodaj inne przypadki jak Y, Z, T, L itd.
            default:
                Serial.println("❓ Unknown prefix");
                break;
            }
        }
    };

    void setupBLE()
    {
        Serial.println("🔵 Inicjalizacja BLE...");
        BLEDevice::init(BLE_DEVICE_NAME);
        Serial.println("✅ BLEDevice::init zakończone");

        BLEServer *pServer = BLEDevice::createServer();
        Serial.println("✅ Utworzono serwer BLE");

        pServer->setCallbacks(new MyServerCallbacks());
        Serial.println("✅ Ustawiono callbacki serwera");

        BLEService *pService = pServer->createService(SERVICE_UUID);
        Serial.println("✅ Utworzono usługę BLE");

        pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE |
                BLECharacteristic::PROPERTY_NOTIFY);
        Serial.println("✅ Utworzono charakterystykę BLE");

        pCharacteristic->setValue("Hello World says Neil");
        pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
        Serial.println("✅ Ustawiono wartości i callbacki charakterystyki");

        pService->start();
        Serial.println("✅ Uruchomiono usługę BLE");

        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(SERVICE_UUID);
        Serial.println("✅ Dodano UUID usługi do reklamowania");

        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06);
        pAdvertising->setMinPreferred(0x12);
        Serial.println("✅ Skonfigurowano parametry reklamowania");

        BLEDevice::startAdvertising();
        Serial.println("✅ Rozpoczęto reklamowanie BLE");
        Serial.println("📱 Urządzenie jest gotowe do połączenia!");
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
        if (!connected)
        {
            Serial.println("⚠️ Próba wysłania danych bez połączenia BLE");
            return;
        }

        float sensitivityRatio = sensitivity / maxSensitivity;
        float normalizedX = x * sensitivityRatio;
        float normalizedY = y * sensitivityRatio;
        float normalizedZ = z * sensitivityRatio;

        Serial.printf("📤 Wysyłanie danych: X=%.2f, Y=%.2f, Z=%.2f\n", normalizedX, normalizedY, normalizedZ);

        uint8_t values[12];
        memcpy(values, &normalizedX, sizeof(normalizedX));
        memcpy(values + 4, &normalizedY, sizeof(normalizedY));
        memcpy(values + 8, &normalizedZ, sizeof(normalizedZ));

        pCharacteristic->setValue(values, sizeof(values));
        pCharacteristic->notify();
        Serial.println("✅ Dane wysłane przez BLE");
    }

    bool isConnected()
    {
        return connected;
    }
}