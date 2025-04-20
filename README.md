# ErgoHealth BLE BMI270

Projekt urządzenia do monitorowania ergonomii pracy z wykorzystaniem czujnika BMI270 i komunikacji BLE.

## Technologie
- PlatformIO
- ESP32
- BMI270 (akcelerometr)
- BLE (Bluetooth Low Energy)

## Konfiguracja
Projekt jest skonfigurowany w PlatformIO dla płytki ESP32 Dev Module.

### Zależności
- SparkFun BMI270 Arduino Library (v1.0.3)

### Pliki konfiguracyjne
- `platformio.ini` - główna konfiguracja projektu
- `platformio_esp32dev.ini` - specyficzna konfiguracja dla ESP32 Dev Module

## Funkcjonalność
- Odczyt danych z czujnika BMI270
- Obliczanie kątów nachylenia
- Przesyłanie danych przez BLE
- Kalibracja czujnika

## Status
Projekt w trakcie rozwoju. Aktualnie:
- ✅ Działający odczyt z BMI270
- ✅ Obliczanie kątów
- ✅ Komunikacja BLE
- 🔄 W trakcie: kalibracja czujnika 