const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";

AsyncWebServer server(80);

void handleRootGet(AsyncWebServerRequest *request) { request->send(200, "text/plain", "Hello from " + String(WiFi.getHostname()) + " @ " + WiFi.localIP().toString()); }

void methodNotAllowed(AsyncWebServerRequest *request) { request->send(405, "text/plain", "405 :METHOD NOT ALLOWED"); }

void connectToWiFi(Config &config)
{

    WiFi.begin(config.ssid, config.password);

    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
        Serial.println("-----------------------");
        Serial.println("Connected to the WiFi network");
        Serial.println(String(WiFi.getHostname()) + " @ " + WiFi.localIP().toString());
    }
    else
    {
        Serial.println("-----------------------");
        Serial.println("Failed to connect to the WiFi network");
    }
}

void handle_received_config(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    StaticJsonDocument<300> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, data);
    if (error)
    {
        request->send(400, "text/plain", "400: BAD JSON");
        Serial.println(error.c_str());

        return;
    }
    config.ssid = jsonDoc["ssid"];
    config.password = jsonDoc["password"];
    config.mqtt_broker = jsonDoc["mqtt_broker"];
    config.mqtt_port = jsonDoc["mqtt_port"];
    config.mqtt_username = jsonDoc["mqtt_username"];
    config.mqtt_password = jsonDoc["mqtt_password"];
    config.uuid = jsonDoc["uuid"];
    config.clientID = WiFi.softAPmacAddress().c_str();
    config.mqtt_topic = jsonDoc["mqtt_topic"];

    // if received config is valid
    if (config.ssid && config.password && config.mqtt_broker && config.mqtt_port && config.mqtt_username && config.mqtt_password && config.uuid)
    {
        request->send(200, "text/plain", "200: OK");
        WiFi.mode(WIFI_STA);
        connectToWiFi(config);
    }
    else
    {
        request->send(400, "text/plain", "400: BAD JSON");
    }

    return;
}
