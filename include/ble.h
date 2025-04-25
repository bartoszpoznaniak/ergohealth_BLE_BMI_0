#ifndef BLE_H
#define BLE_H

#include <functional>

#define BLE_DEVICE_NAME "ErgoHealth-esp32"

namespace BLE
{
    extern float x;
    extern float y;
    extern float z;
    extern bool connected;
    extern float maxSensitivity;
    extern float sensitivity;
    extern std::function<void()> resetCallback;

    void setupBLE();
    void notifyBLE();
    void setValues(float newX, float newY, float newZ);
    void setResetCallback(std::function<void()> callback);
    bool isConnected();
    void sendNotification(String text);
}

#endif // BLE_H