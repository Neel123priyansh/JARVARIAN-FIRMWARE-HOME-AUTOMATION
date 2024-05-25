// Default header files
#include <Arduino.h>
#include <EEPROM.h>
#include <PubSubClient.h>

// Custom headers
#include <structs.h>
#include <utils.h>
#include <WiFiProvisioning.h>

// MQTT Client
WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);
Config config;

// #include <MQTTManager.h>

unsigned long previousMillis = 0;
unsigned long keep_alive_counter = 1;

void setup()
{
  // Initialize Serial if DEBUG is enabled
  if (DEBUG)
    Serial.begin(115200);

  Serial.println("-----------------------");
  Serial.println("Serial initialized");

  // Initialize Buzzer and In-Built LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  updateShiftRegister();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, 1, 0, 4, WIFI_AUTH_WPA2_PSK);
  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(0) == 0xFF)
  {
    server.begin();
    Serial.println("SoftAP Started");
    Serial.println(WiFi.softAPIP());
    Serial.println(WiFi.softAPmacAddress());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleRootGet(request); });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
              { methodNotAllowed(request); });

    server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
              { handle_received_config(request, data, len, index, total); });

    server.onNotFound([](AsyncWebServerRequest *request)
                      { methodNotAllowed(request); });
  }
  else
  {
    EEPROM.get(WIFI_SSID_ADDR, config.wifi_ssid);
    EEPROM.get(WIFI_PASS_ADDR, config.wifi_password);
    Serial.println("Connecting to WiFi");
    connectToWiFi(config);
  }

  // Connect to WiFi
}
void loop()
{
  setOutput(2);
  delay(1000);
  clearOutput(2);
  delay(1000);
  // connect to wifi and reconnect if disconnected
}
