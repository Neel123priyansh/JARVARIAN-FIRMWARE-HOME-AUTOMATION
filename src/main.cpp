#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
// Include Functionality according to the board
#if !(defined(ESP32) || defined(ESP8266))
#error "This code is intended to run on the ESP32 or ESP8266 platform!"
#endif

#ifdef ESP32
int is_esp32 = 1;
int is_esp8266 = 0;

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32httpUpdate.h>

WebServer server(80);

// can use both at same pins.
#define STATUS_BUZZER 27
#define BUILTIN_LED 2
#endif

#ifdef ESP8266
int is_esp32 = 0;
int is_esp8266 = 1;
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
ESP8266WebServer server(80);

#define STATUS_BUZZER 16
#define BUILTIN_LED 2
#endif

#ifdef DEBUG
int debug = 1;
int update_firmware = 0;
#else
int debug = 0;
int update_firmware = 1;
#endif

// Load config file
#include <loadConfig.h>
DynamicJsonDocument configDoc(JSON_FILE_SIZE);
//! shrink to fit the size of the JSON document

// MQTT Client
WiFiClient espClient;
PubSubClient mqttclient(espClient);
InfluxDBClient client;

// Setting `0` as first `keep-alive` message is sent immediately after connection.
unsigned long previousMillis = 0;
unsigned long keep_alive_counter = 1;

void connectToWiFi()
{
  // Set static IP
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

  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Setting static IP...");
    Serial.print("IP: ");
    Serial.println(static_ip);
    Serial.print("Gateway: ");
    Serial.println(gateway);
    Serial.print("Subnet: ");
    Serial.println(subnet);
    Serial.print("DNS: ");
    Serial.println(dns);
  }
  WiFi.config(ip, gateway_ip, subnet_ip, dns_ip);

  // Set hostname
  const char *hostname = configDoc["wifi"]["hostname"].as<const char *>();
  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Setting hostname...");
    Serial.println("Hostname: " + String(hostname));
  }
  WiFi.setHostname(hostname);

  // Connect to WiFi network
  String ssid = configDoc["wifi"]["ssid"].as<String>();
  String password = configDoc["wifi"]["password"].as<String>();

  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Setting up WiFi Credentials...");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
  }

  WiFi.begin(ssid, password);

  // Wait for connection
  if (debug)
    Serial.println("-----------------------");
  while (WiFi.status() != WL_CONNECTED)
  {
    statusBuzzer(2, 100);
    if (debug)
      Serial.println("Connecting to WiFi...");
    delay(2000); // 2s delay
  }

  if (debug)
  {
    Serial.println("Connected to the WiFi network");
    Serial.println(String(WiFi.getHostname()) + " @ " + WiFi.localIP().toString());
  }
  // WiFi.setAutoReconnect(true);
}

void connectToMQTT()
{
  String clientID = configDoc["wifi"]["hostname"].as<String>();
  String username = configDoc["mqtt"]["username"].as<String>();
  String password = configDoc["mqtt"]["password"].as<String>();
  String topic = configDoc["mqtt"]["topic"].as<String>();

  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Connecting to MQTT Broker...");
    Serial.print("Client ID: ");
    Serial.println(clientID);
    Serial.print("Username: ");
    Serial.println(username);
    Serial.print("Password: ");
    Serial.println(password);
  }

  if (mqttclient.connect(clientID.c_str(), username.c_str(), password.c_str()))
  {
    mqttclient.subscribe(topic.c_str());
    if (debug)
    {
      Serial.println("Connected to MQTT Broker");
      Serial.println("Subscribed to topic: " + topic);
    }
  }
  else
  {
    if (debug)
    {
      Serial.println("MQTT connection failed");
      Serial.print("Error code: ");
      Serial.println(mqttclient.state());
    }
    statusBuzzer(5, 100);
  }
}

void handleRootGet()
{
  server.send(200, "text/plain", "Hello from " + String(WiFi.getHostname()) + " @ " + WiFi.localIP().toString());
  statusBuzzer(1, 100);
}

void handleConfigGet()
{
  String configString;
  serializeJson(configDoc, configString);

  configString.replace(configDoc["ota"]["password"].as<String>(), "******************");
  configString.replace(configDoc["mqtt"]["password"].as<String>(), "******************");
  configString.replace(configDoc["wifi"]["password"].as<String>(), "******************");

  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Sending config file");
    Serial.println(configString);
  }

  server.send(200, "application/json", configString);

  // Buzzer status
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

void publish_keep_alive_message()
{
  StaticJsonDocument<256> doc;
  doc["origin"] = configDoc["mqtt"]["clientID"].as<String>();
  doc["messageType"] = "keep_alive";
  doc["message"]["keep_alive_counter"] = keep_alive_counter;
  doc["message"]["uptime"] = millis() / 1000; // uptime in seconds
  keep_alive_counter++;

  String docString;
  serializeJson(doc, docString);
  if (debug)
    Serial.println(docString);
  mqttclient.publish(configDoc["mqtt"]["topic"].as<String>().c_str(), docString.c_str());
}

void publish_current_state_message(int pinNumber, String deviceName)
{
  StaticJsonDocument<256> doc;
  doc["origin"] = configDoc["mqtt"]["clientID"].as<String>();
  doc["messageType"] = "current_state";
  doc["message"]["device_id"] = deviceName;
  doc["message"]["state"] = digitalRead(pinNumber) ? "ON" : "OFF";

  String docString;
  serializeJson(doc, docString);
  if (debug)
    Serial.println(docString);
  mqttclient.publish(configDoc["mqtt"]["topic"].as<String>().c_str(), docString.c_str());
}

void publish_error_message(String message)
{
  StaticJsonDocument<256> doc;
  doc["origin"] = configDoc["mqtt"]["clientID"].as<String>();
  doc["messageType"] = "error";
  doc["message"]["message"] = message;

  String docString;
  serializeJson(doc, docString);
  if (debug)
    Serial.println(docString);
  mqttclient.publish(configDoc["mqtt"]["topic"].as<String>().c_str(), docString.c_str());
}

void ota()
{

  ArduinoOTA.setHostname(configDoc["wifi"]["hostname"].as<const char *>());
  ArduinoOTA.setPassword(configDoc["ota"]["password"].as<const char *>());
  ArduinoOTA.setPort(configDoc["ota"]["port"].as<int>());
  ArduinoOTA.begin();

  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("OTA Initialized");
    Serial.print("Hostname: ");
    Serial.println(configDoc["wifi"]["hostname"].as<const char *>());
    Serial.print("Password: ");
    Serial.println(configDoc["ota"]["password"].as<const char *>());
    Serial.print("Port: ");
    Serial.println(configDoc["ota"]["port"].as<int>());
  }

  ArduinoOTA.onStart([]()
                     { Serial.println("Start"); });
  ArduinoOTA.onEnd([]()
                   {
    Serial.println("\nEnd");
    ESP.restart(); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });

  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
    {
      Serial.println("Auth Failed");
    }
    else if (error == OTA_BEGIN_ERROR)
    {
      Serial.println("Begin Failed");
    }
    else if (error == OTA_CONNECT_ERROR)
    {
      Serial.println("Connect Failed");
    }
    else if (error == OTA_RECEIVE_ERROR)
    {
      Serial.println("Receive Failed");
    }
    else if (error == OTA_END_ERROR)
    {
      Serial.println("End Failed");
    } });
}

// TODO: Trigger OTA update from MQTT
void receive_ota_update()
{
  Serial.println("Firmware Update Received");
  server.send(200, "text/plain", "Receiving and Updating");
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    Serial.println("File upload started");
    Serial.println("File name: " + upload.filename);
  }

  //   if (SPIFFS.exists("/update.bin")) {
  //     SPIFFS.remove("/update.bin");
  //   }
  //   Update.begin(upload.totalSize);
  // } else if (upload.status == UPLOAD_FILE_WRITE) {
  //   Serial.print(".");
  //   if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
  //     Serial.println("Error during firmware upload");
  //   }
  // } else if (upload.status == UPLOAD_FILE_END) {
  //   if (Update.end(true)) {
  //     Serial.println("File upload success, rebooting...");
  //     server.send(200, "text/plain", "Update success, rebooting...");
  //     delay(100);
  //     ESP.restart();
  //   } else {
  //     Serial.println("Error during firmware update");
  //     server.send(500, "text/plain", "Error during firmware update");
  //   }
  // }
}

void check_current_firmware(int current_updated_ver, int last_stable_ver)
{
  Serial.println("Current Firmware - " + current_updated_ver);
  Serial.println("Intializing Firmware Checks ...");
}

void rollback()
{
}

void callback(char *topic, byte *payload, unsigned short int length)
{
  // Convert payload to a JSON Object
  String message;
  for (unsigned int i = 0; i < length; i++)
    message += (char)payload[i];

  // if debug is enabled, print the message
  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Message arrived in topic: " + String(topic));
    Serial.println("Message: " + String(message));
  }

  // Parse message to JSON
  StaticJsonDocument<256> messageDoc;
  DeserializationError error = deserializeJson(messageDoc, message);
  if (error)
  {
    if (debug)
      Serial.println("Not a valid JSON message");
    publish_error_message("Not a valid JSON message");
    return;
  }

  // Skip messages originating from this device
  if (messageDoc["origin"].as<String>().equals(configDoc["mqtt"]["clientID"].as<String>()))
    return;

  // Handle change_state messages
  if (messageDoc["messageType"].as<String>().equals("change_state"))
  {
    if (debug)
      Serial.println("Message type: change_state");

    String deviceId = messageDoc["message"]["device_id"].as<String>();
    String state = messageDoc["message"]["state"].as<String>();

    // Loop through the devices array in the JSON document
    for (const auto &device : configDoc["devices"].as<JsonArray>())
    {
      String name = device["name"];
      int pin = device["pin"].as<int>();
      String type = device["type"];

      if (deviceId.equals(name))
      {
        if (debug)
        {
          Serial.println("Device found in config file");
          Serial.println("Device name: " + name);
          Serial.println("Device pin: " + String(pin));
          Serial.println("Device type: " + type);
        }

        if (state.equals("ON"))
        {
          digitalWrite(pin, HIGH);
          publish_current_state_message(pin, name);
          return;
        }
        else if (state.equals("OFF"))
        {
          digitalWrite(pin, LOW);
          publish_current_state_message(pin, name);
          return;
        }
        else
        {
          if (debug)
            Serial.println("Invalid state" + state);
          publish_error_message("Invalid state" + state);
          return;
        }
      }
    }
    Serial.println("Device (" + deviceId + ") not found in config file");
    return;
  }

  // Handle current_state messages
  if (messageDoc["messageType"].as<String>().equals("current_state"))
  {
    if (debug)
      Serial.println("Message type: current_state");

    String deviceId = messageDoc["message"]["device_id"].as<String>();

    // Loop through the devices array in the JSON document
    for (const auto &device : configDoc["devices"].as<JsonArray>())
    {
      String name = device["name"];
      int pin = device["pin"].as<int>();
      String type = device["type"];

      if (deviceId.equals(name))
      {
        if (debug)
        {
          Serial.println("Device found in config file");
          Serial.println("Device name: " + name);
          Serial.println("Device pin: " + String(pin));
          Serial.println("Device type: " + type);
        }

        publish_current_state_message(pin, name);
        return;
      }
    }
    Serial.println("Device (" + deviceId + ") not found in config file");
    return;
  }
}

void connectToInfluxDB()

{
  client.setConnectionParams(configDoc["influxdb"]["host"].as<String>(), configDoc["influxdb"]["org"].as<String>(), configDoc["influxdb"]["bucket"].as<String>(), configDoc["influxdb"]["token"].as<String>());
  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Connecting to InfluxDB...");
    Serial.print("Host: ");
    Serial.println(configDoc["influxdb"]["host"].as<String>());
    Serial.print("Org: ");
    Serial.println(configDoc["influxdb"]["org"].as<String>());
    Serial.print("Bucket: ");
    Serial.println(configDoc["influxdb"]["bucket"].as<String>());
    Serial.print("Token: ");
    Serial.println(configDoc["influxdb"]["token"].as<String>());
  }
  if (client.validateConnection())
  {
    if (debug)
      Serial.println("Connected to InfluxDB:"+client.getServerUrl()+client.isConnected());
  }
  else
  {
    if (debug)
      Serial.println("InfluxDB connection failed:"+client.getLastErrorMessage());
    statusBuzzer(5, 100);
  }
}

void setup()
{
  // Initialize Serial
  Serial.begin(115200);
  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Serial initialized");
  }

  // Initialize Buzzer and In-Built LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  // Turn ON the built-in LED
  if (is_esp32)
    digitalWrite(BUILTIN_LED, HIGH);
  else if (is_esp8266)
    // (In-Built LED works in Inverted Mode)
    digitalWrite(BUILTIN_LED, LOW);

  // Delay after power on to allow for serial monitor to be connected and to make sure beeps are heard clearly.
  delay(3000);

  // Load config file
  char jsonBuffer[JSON_FILE_SIZE];
  bool configLoaded = loadConfig(jsonBuffer);

  while (!configLoaded)
  {
    if (debug)
    {
      Serial.println("-----------------------");
      Serial.println("Failed to load config file");
    }
    statusBuzzer(3, 100);
    delay(2000);
  }

  // Successfully loaded the config file
  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Config file loaded successfully:");
  }

  // Parse the JSON document if needed
  DeserializationError error = deserializeJson(configDoc, jsonBuffer);

  while (error)
  {
    if (debug)
    {
      Serial.println("-----------------------");
      Serial.println("Failed to parse config file. ERROR: ");
      Serial.println(error.c_str());
    }
    statusBuzzer(3, 100);
    delay(2000);
  }

  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Parsed JSON successfully");
  }

  // Print the config file
  if (debug)
    printConfig(configDoc);

  // Initialize pins
  initilizedPins(configDoc);

  // Connect to WiFi
  connectToWiFi();
  connectToInfluxDB();

  // Initialize OTA
  if (debug)
  {
    ota();
  }
  if (update_firmware)
  {
    receive_ota_update();
  }

  // Define HTTP endpoint
  // Handle Root(/) endpoint
  server.on("/", HTTP_GET, handleRootGet);
  server.on("/", HTTP_POST, methodNotAllowed);

  // Handle Config(/config) endpoint
  server.on("/config", HTTP_GET, handleConfigGet);
  server.on("/config", HTTP_POST, methodNotAllowed);

  // Handle OTA(/otaupdate) endpoint
  server.on("/otaupdate", HTTP_GET, methodNotAllowed);
  server.on("/otaupdate", HTTP_POST, receive_ota_update);

  // Handle 404
  server.onNotFound(notFound);

  // Start server
  server.begin();
  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("HTTP server started");
  }

  // Setup connection to MQTT Broker (Server)
  String server = configDoc["mqtt"]["host"].as<String>();
  String port = configDoc["mqtt"]["port"].as<String>();

  if (debug)
  {
    Serial.println("-----------------------");
    Serial.println("Initializing MQTT Client...");
    Serial.print("Server: ");
    Serial.println(server);
    Serial.print("Port: ");
    Serial.println(port);
  }

  mqttclient.setServer(server.c_str(), port.toInt());
  mqttclient.setCallback(callback);


  connectToMQTT();
}

void loop()
{

  server.handleClient();
  ArduinoOTA.handle();

  // if (client.validateConnection() == false)
  // {
  //   if (debug)
  //     Serial.println("InfluxDB connection lost.. Reconnecting..");
  //   connectToInfluxDB();
  // }

  if (WiFi.status() != WL_CONNECTED)
  {
    if (debug)
      Serial.println("WiFi connection lost.. Reconnecting..");
    connectToWiFi();
  }

  // publish a message roughly every 5 seconds if connected else reconnect
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 10000)
  {
    previousMillis = currentMillis;
    if (MQTT_CONNECTED == mqttclient.state())
      publish_keep_alive_message();
    else
    {
      Serial.println("-----------------------");
      Serial.println("MQTT Connection lost.. Reconnecting..");
      connectToMQTT();
    }
    mqttclient.loop();
  }
  // 
}