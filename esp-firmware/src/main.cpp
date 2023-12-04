#include <Arduino.h>

#include <checkFlags.h>
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

void connectToWiFi()
{
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
    statusBuzzer(2, 100);
    Serial.println("Connecting to WiFi...");
    delay(2000); // 2s delay
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(String(WiFi.getHostname()) + "@" + WiFi.localIP().toString());
}

void handleRootGet()
{
  server.send(200, "text/plain", "Hello from " + String(WIFI_HOSTNAME) + "!");
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

void handlePinDetailsPost()
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
      //! Send available pin details
      server.send(200, "text/plain", content);
    }
    else
    {
      server.send(403, "text/plain", "Forbidden: Invalid API Key.");
    }
  }
  statusBuzzer(1, 100);
}

void setup()
{
  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Serial initialized");

  // Delay after power on to allow for serial monitor to be connected and to make sure beeps are heard clearly.
  delay(3000);

  // Print build flags
  printBuildFlags();

  // Load config file
  loadConfig();

  // Connect to WiFi
  connectToWiFi();

  // Turn ON the built-in LED (In-Built LED works in Inverted Mode)
  digitalWrite(BUILTIN_LED, HIGH);

  // Define HTTP endpoint
  // 1. Handle Root(/) endpoint
  server.on("/", HTTP_GET, handleRootGet);
  server.on("/", HTTP_POST, methodNotAllowed);

  // 2. Handle /pin_details endpoint
  server.on("/pin_details", HTTP_GET, methodNotAllowed);
  server.on("/pin_details", HTTP_POST, handlePinDetailsPost);

  // Handle 404
  server.onNotFound(notFound);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}
void loop()
{
  server.handleClient();
}