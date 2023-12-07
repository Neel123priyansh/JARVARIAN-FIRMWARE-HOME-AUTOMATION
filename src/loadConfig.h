#include <Arduino.h>

#include <ArduinoJson.h>
#include <LittleFS.h>

const size_t JSON_FILE_SIZE = 2048;

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
        Serial.println("-----------------------");
        Serial.println("WIFI_SSID: \t" + String(configDoc["wifi"]["ssid"].as<String>()));
        Serial.println("WIFI_PASS: \t" + String(configDoc["wifi"]["password"].as<String>()));
        Serial.println("WIFI_HOSTNAME: \t" + String(configDoc["wifi"]["hostname"].as<String>()));
        Serial.println("WIFI_IP: \t" + String(configDoc["wifi"]["ip"].as<String>()));
        Serial.println("WIFI_GATEWAY: \t" + String(configDoc["wifi"]["gateway"].as<String>()));
        Serial.println("WIFI_SUBNET: \t" + String(configDoc["wifi"]["subnet"].as<String>()));
        Serial.println("WIFI_DNS: \t" + String(configDoc["wifi"]["dns"].as<String>()));

        Serial.println("MQTT_CLIENT_ID: \t" + String(configDoc["mqtt"]["clientID"].as<String>()));
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
        if (debug)
        {
            Serial.println("-----------------------");
            Serial.println("Failed to mount LittleFS");
        }
        statusBuzzer(3, 100);
        delay(2000);
    }
    if (debug)
    {
        Serial.println("-----------------------");
        Serial.println("Mounted LittleFS");
    }
}

File openConfigFile()
{
    // Open the config file
    File configFile = LittleFS.open("/config.json", "r");
    while (!configFile)
    {
        if (debug)
        {
            Serial.println("-----------------------");
            Serial.println("Failed to open config file");
        }

        statusBuzzer(3, 100);
        delay(2000);
    }
    if (debug)
    {
        Serial.println("-----------------------");
        Serial.println("Opened config file");
    }

    return configFile;
}

void initilizedPins(JsonDocumentType &configDoc)
{
    // Loop through the devices array in the JSON document
    if (debug)
        Serial.println("-----------------------");
    for (const auto &device : configDoc["devices"].as<JsonArray>())
    {
        const String name = device["name"];
        int pin = device["pin"].as<int>();
        const String type = device["type"];

        // Initialize the pin as OUTPUT
        if (type == "OUTPUT")
        {
            //! Check if pin is VALID
            pinMode(pin, OUTPUT);
            if (debug)
                Serial.println("Initialized " + name + " on pin " + String(pin) + " as OUTPUT");
        }
        // Initialize the pin as INPUT
        else if (type == "INPUT")
        {
            //! Check if pin is VALID
            pinMode(pin, INPUT);
            if (debug)
                Serial.println("Initialized " + name + " on pin " + String(pin) + " as INPUT");
        }
        else
        {
            while (true)
            {
                if (debug)
                    Serial.println("Invalid pin type" + type);
                statusBuzzer(4, 100);
                delay(2000);
            }
        }
    }
}

bool loadConfig(char *buffer)
{
    // Check if LittleFS is mounted
    checkLittleFS();

    // Open the config file
    File configFile = openConfigFile();

    size_t bytesRead = configFile.readBytes(buffer, JSON_FILE_SIZE);

    if (bytesRead == 0)
    {
        Serial.println("Failed to read config file");
        return false;
    }
    // Null-terminate the buffer to ensure it's a valid string
    buffer[bytesRead] = '\0';

    // Close the config file
    configFile.close();
    if (debug)
        Serial.println("Closed config file");

    return true;
}