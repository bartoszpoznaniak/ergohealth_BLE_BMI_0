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
void setup()
{
  Serial.begin(115200);

  Wire.begin();

  // I nicjalizacja BMI270
  // bool success = myIMU.beginI2C(i2cAddress);
  // Serial.printf("i2cAddress:%d\n", i2cAddress);
  // if (!success)
  // {
  //   Serial.println("Error: BMI270 not connected, check wiring and I2C address!");
  //   while (1)
  //   ;
  // }

  // Serial.println("BMI270 connected!"); //

  BLE::setupBLE();
  Serial.printf("Jestem po setupBLE \n ");

}

void loop()
{

 // rawAccX = myIMU.data.accelX;
 // rawAccY = myIMU.data.accelY;

  // Wyświetlamy przez Serial z maksymalną precyzją
  // Serial.printf("Serial: %f %f\n", rawAccX, rawAccY);

  float x = random(-100, 101) / 10.0; // random() zwraca liczbę całkowitą, dzielimy przez 10 aby uzyskać wartości zmiennoprzecinkowe w zakresie -10 do 10
  float y = random(-100, 101) / 10.0;
  float z = random(-100, 101) / 10.0;

  // x = myIMU.data.accelX;
  // y = myIMU.data.accelY;
  // z = 0;

  if (BLE::isConnected())
  {
    BLE::setValues(x, y, z);
    Serial.printf("Wysłano przez BLE::: %f, %f, %f\n", x, y, z);
  }
  //Serial.printf("Juhu0 \n ");
  delay(500);
}
