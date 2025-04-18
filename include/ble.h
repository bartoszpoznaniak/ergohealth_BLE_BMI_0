#ifndef BLE_H
#define BLE_H

#define BLE_DEVICE_NAME "ErgoHealth-esp32"

namespace BLE
{
    extern float x;        // Zmienna globalna dla x
    extern float y;        // Zmienna globalna dla y
    extern float z;        // Zmienna globalna dla z
    extern bool connected; // Zmienna globalna dla z

    void setupBLE();
    void notifyBLE();
    void setValues(float newX, float newY, float newZ);
    bool isConnected();
}

#endif // BLE_H