#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>

// Include Functionality according to the board
#if !(defined(ESP32) || defined(ESP8266))
#error "This code is intended to run on the ESP32 or ESP8266 platform!"
#endif

#ifdef ESP32
#include <WiFi.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#endif

void blinkLED(int times, int delayTime)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayTime);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delayTime);
  }
}

void connectToWiFi()
{
#ifndef WIFI_SSID
#error "Please define WIFI_SSID in build flags"
#endif

#ifndef WIFI_PASS
#error "Please define WIFI_PASS in build flags"
#endif

#ifndef WIFI_HOSTNAME
#error "Please define WIFI_HOSTNAME in build flags"
#endif

#ifndef WIFI_IP
#error "Please define WIFI_IP in build flags"
#endif

#ifndef WIFI_GATEWAY
#error "Please define WIFI_GATEWAY in build flags"
#endif

#ifndef WIFI_SUBNET
#error "Please define WIFI_SUBNET in build flags"
#endif

#ifndef WIFI_DNS
#error "Please define WIFI_DNS in build flags"
#endif

  // Connect to Wi-Fi
  //! Use Static IP
  WiFi.config(
      IPAddress(),
      IPAddress(),
      IPAddress(),
      IPAddress());
  WiFi.setHostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    blinkLED(1, 100);
    Serial.println("Connecting to WiFi...");
  }

  // Connected to WiFi
  Serial.println("Connected to WiFi");
  Serial.println(String(WiFi.getHostname()) + "@" + WiFi.localIP().toString());
}

void handleRootGet()
{
  server.send(200, "text/plain", "Hello from " + String(WIFI_HOSTNAME) + "!");
  blinkLED(1, 100);
}

void methodNotAllowed()
{
  server.send(405, "text/plain", "405: Method Not Alloweed");
  blinkLED(1, 100);
}

void notFound()
{
  server.send(404, "text/plain", "404: Not found");
  blinkLED(1, 100);
}

void handleAvailablePinsPost()
{
  // Retrieve the API key from the headers
  String apiKey = server.header("X-API-Key");

  // Check if X-API-Key header was present
  if (apiKey.length() == 0)
  {
    server.send(401, "text/plain", "Unauthorized: No API Key provided.");
  }
  else
  {
    // Check if the API key matches the expected value
    if (apiKey.equals(API_KEY))
    {
      String content = "API Key accepted. Post request received. Sending available pin details.";
      //! Use config/$DEVICE_NAME.json
      server.send(200, "text/plain", content);
    }
    else
    {
      server.send(403, "text/plain", "Forbidden: Invalid API Key.");
    }
    blinkLED(1, 100);
  }
}

void setup()
{
  // Set pin mode
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Initialize Serial
  Serial.begin(115200);

  // Connect to WiFi
  connectToWiFi();

  // Define HTTP endpoint
  // 1. Handle Root(/) endpoint
  server.on("/", HTTP_GET, handleRootGet);
  server.on("/", HTTP_POST, methodNotAllowed);

  // 2. Handle /available_pins endpoint
  server.on("/available_pins", HTTP_GET, methodNotAllowed);
  server.on("/available_pins", HTTP_POST, handleAvailablePinsPost);

  // Handle 404
  server.onNotFound(notFound);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  // Handle HTTP requests
  server.handleClient();
}