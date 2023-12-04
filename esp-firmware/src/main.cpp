#include <Arduino.h>

// Include Functionality according to the board
#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
using ServerType = WebServer;
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
using ServerType = ESP8266WebServer;
#endif

//! Define WiFi Credentials (will use env vars in future)
//! Device is not appearing on router portal
#define WIFI_SSID "yourwifissid"
#define WIFI_PASS "yourwifipassword"
#define WIFI_HOSTNAME "IoT-Device"


ServerType server(80);

void connectToWiFi() {
  // Connect to Wi-Fi

  WiFi.setHostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

    Serial.println("Connected to the WiFi network"+WiFi.localIP().toString());
}

void handleRoot()
{
    server.send(200, "text/plain", "Hello from IoT Device!");
}

void handle_led()
{
    String message = server.arg("command");
    
    Serial.println(message);
    if (message == "on")
        digitalWrite(LED_BUILTIN, LOW);
    if (message=="off")
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    String Response = "LED is " + message;
    server.send(200, "text/plain", Response);
    
}

void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    
    connectToWiFi();
    server.on("/",HTTP_GET, handleRoot);
    server.on("/command",HTTP_POST,handle_led);
    
    server.begin();
    Serial.println("HTTP server started");
    
}
void loop()
{
    server.handleClient();
    
}

  