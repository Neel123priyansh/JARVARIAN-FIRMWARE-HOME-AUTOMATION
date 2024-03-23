#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <checks.h>
#include <structs.h>
#include <pinConfig.h>
#include <utils.h>

// Globlal configuration
Config config;

// MQTT Client
WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);

#include <WiFiManager.h>
#include <MQTTManager.h>

// Setting `0` as first `keep-alive` message is sent immediately after connection.
unsigned long previousMillis = 0;
unsigned long keep_alive_counter = 1;

void setup()
{
  // Initialize Serial if debug is enabled
  if (debug)
    Serial.begin(115200);

  Serial.println("-----------------------");
  Serial.println("Serial initialized");

  // Initialize Buzzer and In-Built LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Turn ON the built-in LED
  if (is_esp32)
    digitalWrite(LED_BUILTIN, HIGH);
  else if (is_esp8266)
    // (In-Built LED works in Inverted Mode)
    digitalWrite(LED_BUILTIN, LOW);

  // Delay after power on to allow for serial monitor to be connected and to make sure beeps are heard clearly.
  delay(3000);

  // Load Configuration
  loadConfig(config);
  printConfig(config);
  connectToWiFi(config.wifi);
  connectToMQTT(callback);
}
void loop()
{
  unsigned long currentMillis = millis();

  // WiFi
  if (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis >= 10000)
  {
    Serial.println("Connecting to Wifi...");
    previousMillis = currentMillis;
    statusBuzzer(2, 100);
    connectToWiFi(config.wifi);
  }

  // MQTT if connected: publish a message roughly every 50 seconds else: reconnect
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
      connectToMQTT(callback);
    }
  }
  mqttclient.loop();
}