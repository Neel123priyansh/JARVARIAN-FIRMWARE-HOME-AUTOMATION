#include <Arduino.h>
#include <PubSubClient.h>

#include <loadConfig.h>

// Include Functionality according to the board
#if !(defined(ESP32) || defined(ESP8266))
#error "This code is intended to run on the ESP32 or ESP8266 platform!"
#endif

#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#endif

WiFiClient espClient;
PubSubClient mqttclient(espClient);

void connectToWiFi(JsonDocumentType &configDoc)
{
  // Connect to WiFi network
  String static_ip = configDoc["wifi"]["ip"].as<String>();
  String gateway = configDoc["wifi"]["gateway"].as<String>();
  String subnet = configDoc["wifi"]["subnet"].as<String>();
  String dns = configDoc["wifi"]["dns"].as<String>();
  IPAddress ip;
  IPAddress gateway_ip;
  IPAddress subnet_ip;
  IPAddress dns_ip;
  ip.fromString(static_ip);
  gateway_ip.fromString(gateway);
  subnet_ip.fromString(subnet);
  dns_ip.fromString(dns);
  WiFi.config(ip, gateway_ip, subnet_ip, dns_ip);
  WiFi.setHostname(configDoc["wifi"]["hostname"]);
  
  WiFi.begin(String(configDoc["wifi"]["ssid"]).c_str(), String(configDoc["wifi"]["password"]).c_str());
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    statusBuzzer(2, 100);
    Serial.println("Connecting to WiFi...");
    delay(2000); // 2s delay
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(String(WiFi.getHostname()) + " @ " + WiFi.localIP().toString());
}

void handleRootGet()
{
  server.send(200, "text/plain", "Hello from " + String(WiFi.hostname()) + " @ " + WiFi.localIP().toString());
  statusBuzzer(1, 100);
}

//! Remove this function later. This is just for testing.
void printConfigDoc(JsonDocumentType &configDoc)
{
  String configString;
  serializeJson(configDoc, configString);
  Serial.println(configString);
}

void handleConfigGet(JsonDocumentType &configDoc)
{
  String configString;
  serializeJson(configDoc, configString);
  Serial.println(configString);
  server.send(200, "application/json", configString);
  statusBuzzer(1, 100);
}

void methodNotAllowed()
{
  server.send(405, "text/plain", "405: Method Not Alloweed");
  statusBuzzer(1, 100);
}

void notFound()
{
  server.send(404, "text/plain", "404: Not found");
  statusBuzzer(1, 100);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  // Serial.print("Message arrived in topic: ");
  // Serial.println(topic);

  // Serial.print("Message:");
  // for (int i = 0; i < length; i++)
  // {
  //   Serial.print((char)payload[i]);
  // }
  // Serial.println();

  // Serial.println("-----------------------");
}

void setup()
{
  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Serial initialized");

  // Initialize Buzzer and In-Built LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  // Turn ON the built-in LED (In-Built LED works in Inverted Mode)
  digitalWrite(BUILTIN_LED, LOW);

  // Delay after power on to allow for serial monitor to be connected and to make sure beeps are heard clearly.
  delay(3000);

  // Load config file
  DynamicJsonDocument configDoc(2048);
  configDoc = loadConfig(configDoc);

  // Connect to WiFi
  connectToWiFi(configDoc);

  // Define HTTP endpoint
  // Handle Root(/) endpoint
  server.on("/", HTTP_GET, handleRootGet);
  server.on("/", HTTP_POST, methodNotAllowed);

  //! Handle Config(/config) endpoint
  server.on("/config", HTTP_GET, [&configDoc]()
            { handleConfigGet(configDoc); });
  server.on("/config", HTTP_POST, methodNotAllowed);

  // Handle 404
  server.onNotFound(notFound);

  // Start server
  server.begin();
  Serial.println("HTTP server started");

  // Connect to MQTT Broker
  // mqttclient.setServer(String(configDoc["mqtt"]["host"]).c_str(), configDoc["mqtt"]["port"]);
  // mqttclient.setCallback(callback);
}
void loop()
{
  server.handleClient();

  // if (!mqttclient.connected())
  // {
  //   Serial.println("Connecting to MQTT Broker...");
  //   if (mqttclient.connect(String(configDoc["wifi"]["hostname"]).c_str(), String(configDoc["mqtt"]["username"]).c_str(), String(configDoc["mqtt"]["password"]).c_str()))
  //   {
  //     Serial.println("Connected to MQTT Broker");
  //     mqttclient.subscribe(String(configDoc["mqtt"]["topic"]).c_str());
  //   }
  //   else
  //   {
  //     Serial.println("MQTT connection failed");
  //     Serial.println(mqttclient.state());
  //     delay(2000);
  //   }
  // }
}