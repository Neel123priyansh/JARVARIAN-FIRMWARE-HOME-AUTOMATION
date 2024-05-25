#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";


// Config config;
AsyncWebServer server(80);

void handleRootGet(AsyncWebServerRequest *request) { request->send(200, "text/plain", "Hello from " + String(WiFi.getHostname()) + " @ " + WiFi.localIP().toString()); }

void methodNotAllowed(AsyncWebServerRequest *request) { request->send(405, "text/plain", "405 :METHOD NOT ALLOWED"); }

u_int8_t connectToWiFi(Config &config)
{

    WiFi.begin(config.wifi_ssid, config.wifi_password);

    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
        Serial.println("-----------------------");
        Serial.println("Connected to the WiFi network");
        Serial.println(String(WiFi.getHostname()) + " @ " + WiFi.localIP().toString());
        return 1;
    }
    else
    {
        Serial.println("-----------------------");
        Serial.println("Failed to connect to the WiFi network");
        return 0;
    }
}

void handle_received_config(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
    DynamicJsonDocument jsonDoc(capacity);
    DeserializationError error = deserializeJson(jsonDoc, data);
    if (error)
    {
        request->send(400, "text/plain", "400: BAD JSON");
        Serial.println(error.c_str());

        return;
    }

    config.wifi_ssid = jsonDoc["ssid"];
    config.wifi_password = jsonDoc["wifi_password"];
    config.mqtt_broker = jsonDoc["mqtt_broker"];
    config.mqtt_port = jsonDoc["mqtt_port"];
    config.mqtt_username = jsonDoc["mqtt_username"];
    config.mqtt_password = jsonDoc["mqtt_password"];
    config.user_uuid = jsonDoc["user_uuid"];
    config.clientID = WiFi.macAddress().c_str();

    const char *missingField = config.validate();
    if (missingField == nullptr)
    {
        request->send(200, "text/plain", "200: OK");
        WiFi.mode(WIFI_STA);
        EEPROM.put(WIFI_SSID_ADDR, config.wifi_ssid);
        EEPROM.put(WIFI_PASS_ADDR, config.wifi_password);
        EEPROM.commit();
        connectToWiFi(config);

        request->send(200, "text/plain", "Config received and applied");
    }
    else
    {
        String response = "400: Missing field: ";
        response += missingField;
        request->send(400, "text/plain", response);
    }

    return;
}
