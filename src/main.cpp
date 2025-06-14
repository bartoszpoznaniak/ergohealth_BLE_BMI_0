#include <Arduino.h>
#include "ble.h"
#include <Wire.h>
#include "SparkFun_BMI270_Arduino_Library.h"

// Próg odchylenia uznawany za złą postawę (domyślnie 15 stopni)
#define YELLOW_POSTURE_THRESHOLD 15.0
#define GREEN_POSTURE_THRESHOLD 5.0
#define PING_INTERVAL 10

BMI270 myIMU;
float rawAccX, rawAccY, rawAccZ;
// Offset kalibracji
float offsetX = 0;
float offsetY = 0;
uint8_t i2cAddress = BMI2_I2C_PRIM_ADDR; // 0x68
bool deviceConnected = false;

// Zmienna do śledzenia czasu rozpoczęcia złej postawy
unsigned long badPostureStartTime = 0;
unsigned long pingPostureStartTime = 0;
bool wasInBadPosture = false;

void handleReset();
void setBadPostureTime(unsigned long newTime);

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

int counter = 0;
int sendCounter = 0;
void loop()
{
  // counter++;
  // Serial.printf("BLE Connected: %d\n", (int)BLE::isConnected());
  // Serial.printf("Counter: %d/%d \n", counter, 30);
  // Serial.printf("Send Counter: %d \n", sendCounter);

  // if (BLE::isConnected())
  // {
  //   if (counter >= 30)
  //   {
  //     counter = 0;
  //     Serial.printf("Send \"B\"! \n");
  //     sendCounter++;
  //     BLE::sendNotification("B");
  //   }
  // }

  // BLE::setValues(10, 15, 0);

  // delay(1000);
  // return;

  if (myIMU.getSensorData() != BMI2_OK)
  {
    Serial.println("❌ Błąd odczytu danych z BMI270");
    delay(500); // Zmieniamy opóźnienie na 100ms jak w Arduino
    return;
  }

  // Odczyt danych z BMI270
  rawAccX = myIMU.data.accelX;
  rawAccY = myIMU.data.accelY;
  rawAccZ = myIMU.data.accelZ;

  // Obliczanie kątów zgodnie z kodem Arduino
  float angleX = atan2(rawAccX, sqrt(rawAccY * rawAccY + rawAccZ * rawAccZ)) * 180.0 / PI - offsetX;
  float angleY = -atan2(rawAccY, sqrt(rawAccX * rawAccX + rawAccZ * rawAccZ)) * 180.0 / PI - offsetY;

  // angleX *= BLE::getSensitivity();
  // angleY *= BLE::getSensitivity();

  // Serial.printf("📊 Odczyt z BMI270: angleX=%.2f, Y=%.2f, Z=%.2f\n", rawAccX, rawAccY, rawAccZ);
  // Serial.printf("📐 Kąty: X=%.6f, Y=%.6f\n", angleX, angleY);

  float sensitivity = BLE::getSensitivity();

  // Obliczenie całkowitego odchylenia od pozycji zerowej
  float posture = sqrt(angleX * angleX + angleY * angleY);
  // Serial.printf("Posture: %.0f / %.0f \n", posture, RED_POSTURE_THRESHOLD);

  Serial.printf("BLE Connected: %d\n", (int)BLE::isConnected());

  // Sprawdzenie czy przekroczono próg złej postawy
  bool isRedPosture = posture > 90.0f * BLE::getYellowRadius();
  bool isYellowPosture = posture > 90.0f * BLE::getGreenRadius();
  Serial.printf("isPosture: %d %d\n", (int)isRedPosture, (int)isYellowPosture);

  if (isRedPosture)
  {
    Serial.printf("IF0 Jest zla postawa!\n");
    unsigned long currentTime = millis();
    // Jeśli wcześniej nie byliśmy w złej postawie, zapisz czas rozpoczęcia
    if (!wasInBadPosture)
    {
      badPostureStartTime = currentTime;
      wasInBadPosture = true;
      Serial.printf("⚠️ Wykryto złą postawę! Odchylenie: %.2f° (próg: %.2f°)\n",
                    posture,
                    YELLOW_POSTURE_THRESHOLD);
    }
    // Sprawdź, czy właśnie przekroczono krytyczny czas
    unsigned long badPostureInterval = BLE::getNotificationDelay() * 1000;

    unsigned long badPostureDelay = currentTime - badPostureStartTime;
    Serial.printf("🔴 %.0f / %.0f \n", badPostureDelay / 1000.0, badPostureInterval / 1000.0);
    if (badPostureDelay >= badPostureInterval)
    { // 100ms margines na pewność
      badPostureStartTime = currentTime;
      Serial.printf("🚨 UWAGA: Wada postawy w krytycznym czasie (%.1f minut)!\n", badPostureInterval / 1000.0);
      if (BLE::getNotificationDelay() > 0)
      {
        BLE::sendNotification("B");
      }
    }
  }
  else
  {
    // Serial.printf("ELSE0 Poprawna postawa\n");
    // Reset licznika jeśli postura jest prawidłowa
    wasInBadPosture = false;
  }

  // Send PING
  unsigned long pingPostureDelay = millis() - pingPostureStartTime;
  if (pingPostureDelay >= PING_INTERVAL * 1000)
  {
    pingPostureStartTime = millis();
    Serial.printf("PING\n");
    BLE::sendNotification("p");
  }

  // Wysyłanie danych (animacja)
  if (BLE::isConnected())
  {
    BLE::setValues(angleX, angleY, 0);
    // Serial.printf("📤 Wysłano przez BLE: angleX=%.6f, Y=%.6f, Z=%.6f\n", angleX, y, z);
  }
  else
  {
    // Serial.println("⚠️ Brak połączenia BLE");
  }

  delay(200);
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
