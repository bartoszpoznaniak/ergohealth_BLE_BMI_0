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
    extern float notificationDelay;
    extern float redRadius;
    extern float yellowRadius;
    extern float greenRadius;

    extern std::function<void()> resetCallback;

    void setupBLE();
    void notifyBLE();
    void setValues(float newX, float newY, float newZ);
    void setResetCallback(std::function<void()> callback);
    bool isConnected();
    void sendNotification(String text);

    float getSensitivity();
    float getNotificationDelay();
    float getRedRadius();
    float getYellowRadius();
    float getGreenRadius();
}

#endif // BLE_H