#include <Arduino.h>

#ifndef WIFI_SSID
#error "Please define WIFI_SSID in build flags"
#endif

#ifndef WIFI_PASS
#error "Please define WIFI_PASS in build flags"
#endif

#ifndef WIFI_HOSTNAME
#error "Please define WIFI_HOSTNAME in build flags"
#endif

#ifndef WIFI_IP
#error "Please define WIFI_IP in build flags"
#endif

#ifndef WIFI_GATEWAY
#error "Please define WIFI_GATEWAY in build flags"
#endif

#ifndef WIFI_SUBNET
#error "Please define WIFI_SUBNET in build flags"
#endif

#ifndef WIFI_DNS
#error "Please define WIFI_DNS in build flags"
#endif

#ifndef MQTT_SERVER
#error "Please define MQTT_SERVER in build flags"
#endif

#ifndef MQTT_PORT
#error "Please define MQTT_PORT in build flags"
#endif

#ifndef MQTT_USERNAME
#error "Please define MQTT_USERNAME in build flags"
#endif

#ifndef MQTT_PASSWORD
#error "Please define MQTT_PASSWORD in build flags"
#endif

#ifndef MQTT_TOPIC
#error "Please define MQTT_TOPIC in build flags"
#endif

#ifndef API_KEY
#error "Please define API_KEY in build flags"
#endif

void printBuildFlags()
{
    Serial.println("WIFI_SSID: " + String(WIFI_SSID));
    Serial.println("WIFI_PASS: " + String(WIFI_PASS));
    Serial.println("WIFI_HOSTNAME: " + String(WIFI_HOSTNAME));
    Serial.println("WIFI_IP: " + String(WIFI_IP));
    Serial.println("WIFI_GATEWAY: " + String(WIFI_GATEWAY));
    Serial.println("WIFI_SUBNET: " + String(WIFI_SUBNET));
    Serial.println("WIFI_DNS: " + String(WIFI_DNS));

    Serial.println("MQTT_SERVER: " + String(MQTT_SERVER));
    Serial.println("MQTT_PORT: " + String(MQTT_PORT));
    Serial.println("MQTT_USERNAME: " + String(MQTT_USERNAME));
    Serial.println("MQTT_PASSWORD: " + String(MQTT_PASSWORD));
    Serial.println("MQTT_TOPIC: " + String(MQTT_TOPIC));

    Serial.println("API_KEY: " + String(API_KEY));
}