#if !(defined(ESP32) || defined(ESP8266))
#error "This code is intended to run on the ESP32 or ESP8266 platform!"
#endif

#ifdef ESP32
#define IS_ESP32 1
#define STATUS_BUZZER 27
#define dataPin 14
#define latchPin 27
#define clockPin 26
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32httpUpdate.h>
#include <ESPAsyncWebServer.h>
#else
#define IS_ESP32 0
#endif
#ifdef ESP8266
#define IS_ESP8266 1
#define STATUS_BUZZER 16
#include <ESP8266WiFi.h>
#else
#define IS_ESP8266 0
#endif
#ifdef DEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif
