#include <Arduino.h>
#include "ble.h"

void setup()
{
  Serial.begin(115200);
  // put your setup code here, to run once:
  BLE::setupBLE();
}

void loop()
{
  float x = random(-100, 101) / 10.0; // random() zwraca liczbę całkowitą, dzielimy przez 10 aby uzyskać wartości zmiennoprzecinkowe w zakresie -10 do 10
  float y = random(-100, 101) / 10.0;
  float z = random(-100, 101) / 10.0;

  if (BLE::isConnected())
  {
    BLE::setValues(x, y, z);
    // Serial.printf("Wysłano przez BLE::: %f, %f, %f\n", x, y, z);
  }

  delay(500);
}
