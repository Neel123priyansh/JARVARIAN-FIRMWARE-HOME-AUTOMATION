#include <Arduino.h>

#include <ArduinoJson.h>
#include <LittleFS.h>

#ifdef ESP32
#define STATUS_BUZZER 2
// #define BUILTIN_LED D3
#endif

#ifdef ESP8266
#define STATUS_BUZZER D0
#define BUILTIN_LED D4
#endif

#ifdef DEBUG
int debug = 1;
#else
int debug = 0;
#endif

using JsonDocumentType = ArduinoJson::V6213PB2::DynamicJsonDocument;

void statusBuzzer(int times, int delayTime)
{
    for (int i = 0; i < times; i++)
    {
        digitalWrite(STATUS_BUZZER, HIGH);
        delay(delayTime);
        digitalWrite(STATUS_BUZZER, LOW);
        delay(delayTime);
    }
}

int mapPin(const String &pinString)
{
    // static const uint8_t D0   = 16;
    // static const uint8_t D1   = 5;
    // static const uint8_t D2   = 4;
    // static const uint8_t D3   = 0;
    // static const uint8_t D4   = 2;
    // static const uint8_t D5   = 14;
    // static const uint8_t D6   = 12;
    // static const uint8_t D7   = 13;
    // static const uint8_t D8   = 15;
    // static const uint8_t D9   = 3;
    // static const uint8_t D10  = 1;
    if (pinString.equals("D0"))
    {
        return 16;
    }
    else if (pinString.equals("D1"))
    {
        return 5;
    }
    else if (pinString.equals("D2"))
    {
        return 4;
    }
    else if (pinString.equals("D3"))
    {
        return 0;
    }
    else if (pinString.equals("D4"))
    {
        return 2;
    }
    else if (pinString.equals("D5"))
    {
        return 14;
    }
    else if (pinString.equals("D6"))
    {
        return 12;
    }
    else if (pinString.equals("D7"))
    {
        return 13;
    }
    else if (pinString.equals("D8"))
    {
        return 15;
    }
    else if (pinString.equals("D9"))
    {
        return 3;
    }
    else if (pinString.equals("D10"))
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void printConfig(JsonDocumentType &configDoc)
{
    if (debug)
    {
        Serial.println("WIFI_SSID: \t" + String(configDoc["wifi"]["ssid"].as<String>()));
        Serial.println("WIFI_PASS: \t" + String(configDoc["wifi"]["password"].as<String>()));
        Serial.println("WIFI_HOSTNAME: \t" + String(configDoc["wifi"]["hostname"].as<String>()));
        Serial.println("WIFI_IP: \t" + String(configDoc["wifi"]["ip"].as<String>()));
        Serial.println("WIFI_GATEWAY: \t" + String(configDoc["wifi"]["gateway"].as<String>()));
        Serial.println("WIFI_SUBNET: \t" + String(configDoc["wifi"]["subnet"].as<String>()));
        Serial.println("WIFI_DNS: \t" + String(configDoc["wifi"]["dns"].as<String>()));

        Serial.println("MQTT_SERVER: \t" + String(configDoc["mqtt"]["host"].as<String>()));
        Serial.println("MQTT_PORT: \t" + String(configDoc["mqtt"]["port"].as<String>()));
        Serial.println("MQTT_USERNAME: \t" + String(configDoc["mqtt"]["username"].as<String>()));
        Serial.println("MQTT_PASSWORD: \t" + String(configDoc["mqtt"]["password"].as<String>()));
        Serial.println("MQTT_TOPIC: \t" + String(configDoc["mqtt"]["topic"].as<String>()));
    }
}

JsonDocumentType loadConfig(JsonDocumentType &configDoc)
{
    // Mount LittleFS
    while (!LittleFS.begin())
    {
        while (true)
        {
            Serial.println("Failed to mount LittleFS");

            statusBuzzer(3, 100);

            delay(2000);
        }
    }
    Serial.println("Mounted LittleFS");

    // Read the config file
    File configFile = LittleFS.open("config.json", "r");
    while (!configFile)
    {
        while (true)
        {
            Serial.println("Failed to open config file");

            statusBuzzer(3, 100);

            delay(2000);
        }
    }
    Serial.println("Opened config file");

    // Parse the JSON content
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);

    DeserializationError error = deserializeJson(configDoc, buf.get());
    if (error)
    {
        while (true)
        {
            Serial.println("Failed to parse config file");

            statusBuzzer(3, 100);

            delay(2000);
        }
    }
    Serial.println("Parsed config file");

    // Close the config file
    configFile.close();
    Serial.println("Closed config file");

    // Unmount LittleFS
    LittleFS.end();
    Serial.println("Unmounted LittleFS");

    // Loop through the devices array in the JSON document
    for (const auto &device : configDoc["devices"].as<JsonArray>())
    {
        const String name = device["name"];
        const String pin = device["pin"];
        const String type = device["type"];

        // Initialize the pin
        if (type == "OUTPUT")
        {
            int pinNumber = mapPin(pin);
            if (pinNumber != -1)
            {
                pinMode(pinNumber, OUTPUT);
                Serial.println("Initialized " + name + " on pin " + pin + "(GPIO " + pinNumber + ") " + "as " + type);
            }
            else
            {
                while (true)
                {
                    Serial.println("Invalid pin" + pin);
                    statusBuzzer(4, 100);
                    delay(2000);
                }
            }
        }
        else
        {
            Serial.println("Invalid pin type");
        }
    }
    printConfig(configDoc);
    return configDoc;
}