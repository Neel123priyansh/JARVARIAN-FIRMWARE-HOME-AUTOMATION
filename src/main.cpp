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

// MQTT Client
WiFiClient espClient;
PubSubClient mqttclient(espClient);
// Setting as first message is sent immediately after connection
unsigned long previousMillis = 0;

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

  // Connect to WiFi network
  const char *ssid = configDoc["wifi"]["ssid"].as<const char *>();
  const char *password = configDoc["wifi"]["password"].as<const char *>();

  WiFi.begin(ssid, password);

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

void connectToMQTT()
{
  Serial.println("Connecting to MQTT Broker...");
  // if (mqttclient.connect(String(configDoc["wifi"]["hostname"]).c_str(), String(configDoc["mqtt"]["username"]).c_str(), String(configDoc["mqtt"]["password"]).c_str()))
  String clientID = "nodemcu-" + String(random(0xffff), HEX);

  if (mqttclient.connect(clientID.c_str(), "mosquitto", "mosquitto"))
  {
    Serial.println("Connected to MQTT Broker");
    // mqttclient.subscribe(String(configDoc["mqtt"]["topic"]).c_str());
    mqttclient.subscribe("home/2/bedroom");
  }
  else
  {
    Serial.println("MQTT connection failed");
    Serial.print("Error code: ");
    Serial.println(mqttclient.state());
    delay(2000);
  }
}

void handleRootGet()
{
  server.send(200, "text/plain", "Hello from " + String(WiFi.getHostname()) + " @ " + WiFi.localIP().toString());
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


// void handleConfigGet()
// {
  // Load config file
  // File configFile = openConfigFile();

  // Send the config file
  // server.streamFile(configFile, "application/json");

  // Close the file
  // closeConfigFile(configFile);

  // Buzzer status
  // statusBuzzer(1, 100);
// }

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

void callback(char *topic, byte *payload, unsigned short int length)
{
  if (debug)
    Serial.println("-----------------------");

  // Convert payload to a string
  String message;
  for (unsigned short int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  if (message.equals("keep-alive"))
    return; // Skip processing for keep-alive messages
  if (message.equals("status"))
    return; // Skip processing for keep-alive messages
  if (message.equals("OK"))
    return; // Skip processing for keep-alive messages
  if (message.equals("Invalid state"))
    return; // Skip processing for ERROR messages
  if (message.equals("Not a valid JSON message"))
    return; // Skip processing for ERROR messages

  if (message.equals("status"))
  {
    mqttclient.publish("home/2/bedroom", "OK");
    return;
  }

  // if debug is enabled, print the message
  if (debug)
  {
    Serial.println("Message arrived in topic: " + String(topic));
    Serial.println("Message: " + String(message));
  }

  // Handle the message if it's a JSON
  DynamicJsonDocument doc(64);
  DeserializationError error = deserializeJson(doc, message);
  if (error)
  {
    Serial.println("Not a valid JSON message");
    mqttclient.publish("home/2/bedroom", "Not a valid JSON message");
    return;
  }

  if (doc.containsKey("pin") && doc.containsKey("state"))
  {
    const String pin = doc["pin"];
    const String state = doc["state"];

    if (state.equals("ON"))
    {
      digitalWrite(mapPin(pin), HIGH);
      mqttclient.publish("home/2/bedroom", "OK");
    }
    else if (state.equals("OFF"))
    {
      digitalWrite(mapPin(pin), LOW);
      mqttclient.publish("home/2/bedroom", "OK");
    }
    else
    {
      Serial.println("Invalid state");
      mqttclient.publish("home/2/bedroom", "Invalid state");
    }

    if (debug)
    {
      Serial.println("Pin: " + pin);
      Serial.println("State: " + state);
    }

    statusBuzzer(1, 100);
  }

  if (debug)
    Serial.println("-----------------------");
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

  // Setup connection to MQTT Broker (Server)
  // mqttclient.setServer(String(configDoc["mqtt"]["host"]).c_str(), configDoc["mqtt"]["port"].as<int>());
  mqttclient.setServer("192.168.0.254", 1883);
  mqttclient.setCallback(callback);
}
void loop()
{
  server.handleClient();

  // if (WiFi.status() != WL_CONNECTED)
  // {
  //   Serial.println("WiFi connection lost.. Reconnecting..");
  //   connectToWiFi(configDoc);
  // }

  if (!mqttclient.connected())
  {
    connectToMQTT();
  }
  mqttclient.loop();

  // publish a message roughly every 5 seconds.
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 10000)
  {
    previousMillis = currentMillis;
    mqttclient.publish("home/2/bedroom", "keep-alive");
  }
}