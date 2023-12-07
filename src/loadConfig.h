#include <Arduino.h>

#include <ArduinoJson.h>
#include <LittleFS.h>

#ifdef ESP32
// can use both at same pins.
#define STATUS_BUZZER 27
#define BUILTIN_LED 2
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

void checkLittleFS()
{
    // Mount LittleFS
    while (!LittleFS.begin())
    {
        Serial.println("Failed to mount LittleFS");
        statusBuzzer(3, 100);
        delay(2000);
    }
    Serial.println("Mounted LittleFS");
}

File openConfigFile()
{
    // Open the config file
    File configFile = LittleFS.open("config.json", "r");
    while (!configFile)
    {
        Serial.println("Failed to open config file");
        statusBuzzer(3, 100);
        delay(2000);
    }
    Serial.println("Opened config file");

    return configFile;
}

void closeConfigFile(File configFile)
{
    // Close the config file
    configFile.close();
    Serial.println("Closed config file");

    // Unmount LittleFS
    LittleFS.end();
    Serial.println("Unmounted LittleFS");
}

JsonDocumentType loadConfig(JsonDocumentType &configDoc)
{
    // Mount LittleFS and open the config file
    // File configFile = openConfigFile();

    // Check if LittleFS is mounted
    checkLittleFS();

    // Open the config file
    File configFile = LittleFS.open("/config.json", "r");
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
    while (error)
    {
        Serial.println("Failed to parse config file");
        statusBuzzer(3, 100);
        delay(2000);
    }
    Serial.println("Parsed config file");

    // Close the config file and unmount LittleFS
    // closeConfigFile(configFile);

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
        int pin = device["pin"].as<int>();
        const String type = device["type"];

        // Initialize the pin
        if (type == "OUTPUT")
        {
            if (pin != -1)
            {
                pinMode(pin, OUTPUT);
                Serial.println("Initialized " + name + " on pin " + String(pin) + " as OUTPUT");
            }
            // Configure the pin of INPUT_PULLUP
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