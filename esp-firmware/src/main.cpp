#include <Arduino.h>

// Include Functionality according to the board
#ifdef ESP32
#include <WiFi.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#endif

//! Define WiFi Credentials (will use env vars in future)
//! Device is not appearing on router portal
#define WIFI_SSID "SwastikWiFi"
#define WIFI_PASS "jarvis@wifi"
#define WIFI_HOSTNAME "IoT-Device"
#define WIFI_IP IPAddress(192, 168, 0, 240)
#define WIFI_GATEWAY IPAddress(192, 168, 0, 1)
#define WIFI_SUBNET IPAddress(255, 255, 255, 0)
#define WIFI_DNS IPAddress(192, 168, 0, 1) //! Set DNS such that it can't connect to the internet

void connectToWiFi() {
  // Connect to Wi-Fi
  WiFi.config(WIFI_IP, WIFI_GATEWAY, WIFI_SUBNET, WIFI_DNS);
  WiFi.setHostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Connected to WiFi
  Serial.println("Connected to WiFi");
  Serial.println(String(WiFi.getHostname()) + "@" + WiFi.localIP().toString());
}

void handleRoot() {
  server.send(200, "text/plain", "Hello from " + String(WIFI_HOSTNAME) + "!");
}


void setup()
{
  // Initialize Serial
  Serial.begin(115200);

  // Connect to WiFi
  connectToWiFi();

  // Define HTTP endpoint
  server.on("/", HTTP_GET, handleRoot);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  // Handle HTTP requests
  server.handleClient();
}