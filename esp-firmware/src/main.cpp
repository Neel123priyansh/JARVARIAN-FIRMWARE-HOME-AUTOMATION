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

void connectToWiFi(DynamicJsonDocument configDoc)
{
  // Connect to Wi-Fi
  //! Use Static IP
  WiFi.config(
      IPAddress(),
      IPAddress(),
      IPAddress(),
      IPAddress());
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
  Serial.println(String(WiFi.getHostname()) + "@" + WiFi.localIP().toString());
}

void handleRootGet(DynamicJsonDocument configDoc)
{
  server.send(200, "text/plain", "Hello from " + String(configDoc["wifi"]["hostname"]) + "!");
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

void setup()
{
  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Serial initialized");

  // Delay after power on to allow for serial monitor to be connected and to make sure beeps are heard clearly.
  delay(3000);

  // Load config file
  DynamicJsonDocument configDoc = loadConfig();

  // Connect to WiFi
  connectToWiFi(configDoc);

  // Turn ON the built-in LED (In-Built LED works in Inverted Mode)
  digitalWrite(BUILTIN_LED, HIGH);

  // Define HTTP endpoint
  // Handle Root(/) endpoint
  server.on("/", HTTP_GET, [&]()
            { handleRootGet(configDoc); });
  server.on("/", HTTP_POST, methodNotAllowed);

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