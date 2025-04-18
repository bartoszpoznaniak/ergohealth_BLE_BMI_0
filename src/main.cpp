#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "ble.h"

void setup()
{
  Serial.begin(115200);
  // put your setup code here, to run once:
  setupBLE();
}

void loop()
{
  /* Local variables */
  int rc;
  esp_err_t ret;

  // put your main code here, to run repeatedly:
  Serial.println("Hello, World!");
  delay(1000);
}
