#ifndef BLE_H
#define BLE_H

#define BLE_DEVICE_NAME "ErgoHealth-esp32"

namespace BLE
{
    extern float x;
    extern float y;
    extern float z;
    extern bool connected;
    extern float maxSensitivity;
    extern float sensitivity;

    void setupBLE();
    void notifyBLE();
    void setValues(float newX, float newY, float newZ);
    bool isConnected();
}

#endif // BLE_H