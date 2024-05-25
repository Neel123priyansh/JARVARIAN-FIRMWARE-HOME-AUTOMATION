#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <checks.h>
#include <structs.h>
#include <utils.h>
#include <WiFiProvisioning.h>


// MQTT Client
WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);

Config config;


// #include <WiFiManager.h>
#include <MQTTManager.h>

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
  WiFi.softAP(ssid, password);
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
  

}
void loop()
{

  unsigned long currentMillis = millis();

  if (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis >= 10000)
  {
    Serial.println("Connecting to Wifi...");
    previousMillis = currentMillis;
    statusBuzzer(2, 100);
    connectToMQTT(mqttCallback);
  }
  if (currentMillis - previousMillis > 50000)
  {
    previousMillis = currentMillis;
    if (mqttclient.connected())
    {
      publish_keep_alive_message(keep_alive_counter);
      keep_alive_counter++;
    }
    else
    {
      Serial.println("-----------------------");
      Serial.println("MQTT Connection lost.. Reconnecting..");
      connectToMQTT(mqttCallback);
    }
  }
  mqttclient.loop();

  for (size_t i = 0; i < config.devices.size(); i++)
  {
    u_int8_t state = digitalRead(config.devices[i].statusPin);
    if (state != config.devices[i].state)
    {
      config.devices[i].state = state;
      publish_current_state_message(-1, state, String(config.devices[i].name.c_str()));
    }
  }
}