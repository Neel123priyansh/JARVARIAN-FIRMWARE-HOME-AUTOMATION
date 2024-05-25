#if !(defined(ESP32) || defined(ESP8266))
#error "This code is intended to run on the ESP32 or ESP8266 platform!"
#endif

#ifdef ESP32
int is_esp32 = 1;
int is_esp8266 = 0;

#include <WiFi.h>

// can use both at same pins.
#define STATUS_BUZZER 27

#endif

#ifdef ESP8266
int is_esp32 = 0;
int is_esp8266 = 1;

#include <ESP8266WiFi.h>

#define STATUS_BUZZER 16
#define BUILTIN_LED 2
#endif

#ifdef DEBUG
int debug = 1;
#else
int debug = 0;
#endif
