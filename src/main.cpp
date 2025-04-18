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
  loopBLE();
}
