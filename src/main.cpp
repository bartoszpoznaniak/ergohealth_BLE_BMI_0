#include <Arduino.h>
#include "ble.h"
#include <Wire.h>
#include "SparkFun_BMI270_Arduino_Library.h"
BMI270 myIMU;
float rawAccX, rawAccY, rawAccZ;
// Offset kalibracji
float offsetX = 0;
float offsetY = 0;
uint8_t i2cAddress = BMI2_I2C_PRIM_ADDR; // 0x68
bool deviceConnected = false;

// Próg odchylenia uznawany za złą postawę (domyślnie 15 stopni)
float postureThreshold = 15.0;

// Zmienna do śledzenia czasu rozpoczęcia złej postawy
unsigned long badPostureStartTime = 0;
bool wasInBadPosture = false;

void handleReset();
void setBadPostureTime(unsigned long newTime);
void setPostureThreshold(float newThreshold);

void setup()
{
  Serial.begin(115200);
  delay(1000); // Dajmy czas na inicjalizację Serial

  Serial.println("🔵 Inicjalizacja I2C...");
  Wire.begin();
  Wire.setClock(400000); // Ustawmy wyższą częstotliwość I2C
  Serial.println("✅ I2C zainicjalizowane");

  // Inicjalizacja BMI270
  Serial.println("🔵 Inicjalizacja BMI270...");
  Serial.printf("Adres I2C: 0x%X\n", i2cAddress);

  // Sprawdźmy, czy urządzenie odpowiada na adresie I2C

  Wire.beginTransmission(i2cAddress);
  byte error = Wire.endTransmission();
  if (error == 0)
  {
    Serial.println("✅ Urządzenie odpowiada na adresie I2C");
  }
  else
  {
    Serial.printf("❌ Błąd I2C: %d\n", error);
  }

  while (myIMU.beginI2C(i2cAddress) != BMI2_OK)
  {
    Serial.println("❌ Błąd: BMI270 nie połączony, sprawdź połączenia i adres I2C!");
    delay(1000);
  }
  Serial.println("✅ BMI270 połączony!");

  BLE::setupBLE();
  Serial.printf("Jestem po setupBLE \n ");

  BLE::setResetCallback(handleReset);
}

void loop()
{
  // Odczyt danych z BMI270
  if (myIMU.getSensorData() == BMI2_OK)
  {
    rawAccX = myIMU.data.accelX;
    rawAccY = myIMU.data.accelY;
    rawAccZ = myIMU.data.accelZ;

    // Obliczanie kątów zgodnie z kodem Arduino
    float angleX = atan2(rawAccX, sqrt(rawAccY * rawAccY + rawAccZ * rawAccZ)) * 180.0 / PI - offsetX;
    float angleY = -atan2(rawAccY, sqrt(rawAccX * rawAccX + rawAccZ * rawAccZ)) * 180.0 / PI - offsetY;

    // Serial.printf("📊 Odczyt z BMI270: X=%.2f, Y=%.2f, Z=%.2f\n", rawAccX, rawAccY, rawAccZ);
    // Serial.printf("📐 Kąty: X=%.6f, Y=%.6f\n", angleX, angleY);

    float x = angleX;
    float y = angleY;
    float z = 0; // Nie używamy Z

    // Obliczenie całkowitego odchylenia od pozycji zerowej
    float posture = sqrt(x * x + y * y);

    // Sprawdzenie czy przekroczono próg złej postawy
    bool isBadPosture = posture > postureThreshold;
    // if (isBadPosture)
    // {
    //   Serial.printf("🔴");
    //   Serial.printf("🔴");
    // }
    if (isBadPosture and BLE::getNotificationDelay() > 0)
    {
      unsigned long currentTime = millis();
      // Jeśli wcześniej nie byliśmy w złej postawie, zapisz czas rozpoczęcia
      if (!wasInBadPosture)
      {
        badPostureStartTime = currentTime;
        wasInBadPosture = true;
        Serial.printf("⚠️ Wykryto złą postawę! Odchylenie: %.2f° (próg: %.2f°)\n", posture, postureThreshold);
      }
      // Sprawdź, czy właśnie przekroczono krytyczny czas
      unsigned long badPostureInterval = BLE::getNotificationDelay() * 1000;

      unsigned long badPostureDelay = currentTime - badPostureStartTime;
      //      Serial.printf("🔴 %.0f / %.0f \n", badPostureDelay / 1000.0, badPostureInterval / 1000.0);
      if (badPostureDelay >= badPostureInterval)
      { // 100ms margines na pewność
        badPostureStartTime = currentTime;
        Serial.printf("🚨 UWAGA: Wada postawy w krytycznym czasie (%.1f minut)!\n", badPostureInterval / 1000.0);
        BLE::sendNotification("B");
      }
    }
    else
    {
      // Reset licznika jeśli postura jest prawidłowa
      wasInBadPosture = false;
    }

    if (BLE::isConnected())
    {
      BLE::setValues(x, y, z);
      // Serial.printf("📤 Wysłano przez BLE: X=%.6f, Y=%.6f, Z=%.6f\n", x, y, z);
    }
    else
    {
      // Serial.println("⚠️ Brak połączenia BLE");
    }
  }
  else
  {
    Serial.println("❌ Błąd odczytu danych z BMI270");
  }

  delay(500); // Zmieniamy opóźnienie na 100ms jak w Arduino
}

void handleReset()
{
  offsetX = atan2(rawAccX, sqrt(rawAccY * rawAccY + rawAccZ * rawAccZ)) * 180.0 / PI;
  offsetY = -atan2(rawAccY, sqrt(rawAccX * rawAccX + rawAccZ * rawAccZ)) * 180.0 / PI;

  // Aktualizuj zmienne globalne na 0
  rawAccX = 0;
  rawAccY = 0;

  printf("Sensor wyzerowany");
}

void setPostureThreshold(float newThreshold)
{
  postureThreshold = newThreshold;
  Serial.printf("📏 Ustawiono próg postawy na: %.2f stopni\n", postureThreshold);
}