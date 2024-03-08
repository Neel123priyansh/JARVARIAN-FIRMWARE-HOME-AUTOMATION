void connectToWiFi(WiFiConfig &config)
{
    // Set static IP
    IPAddress ip;
    ip.fromString(config.ip.c_str());
    IPAddress gateway_ip, subnet_ip, dns_ip;
    gateway_ip.fromString(config.gateway.c_str());
    subnet_ip.fromString(config.subnet.c_str());
    dns_ip.fromString(config.dns.c_str());

    Serial.println("-----------------------");
    Serial.println("Setting static IP...");
    Serial.print("IP: ");
    Serial.println(config.ip.c_str());
    Serial.print("Gateway: ");
    Serial.println(config.gateway.c_str());
    Serial.print("Subnet: ");
    Serial.println(config.subnet.c_str());
    Serial.print("DNS: ");
    Serial.println(config.dns.c_str());

    WiFi.config(ip, gateway_ip, subnet_ip, dns_ip);

    // Set hostname
    Serial.println("-----------------------");
    Serial.println("Setting hostname...");
    Serial.print("Hostname: ");
    Serial.println(config.clientID.c_str());
    WiFi.setHostname(config.clientID.c_str());

    // Connect to WiFi network
    Serial.println("-----------------------");
    Serial.println("Setting up WiFi Credentials...");
    Serial.print("SSID: ");
    Serial.println(config.ssid.c_str());
    Serial.print("Password: ");
    Serial.println(config.password.c_str());
    WiFi.begin(config.ssid.c_str(), config.password.c_str());

    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
        Serial.println("-----------------------");
        Serial.println("Connected to the WiFi network");
        Serial.println(String(WiFi.getHostname()) + " @ " + WiFi.localIP().toString());
    }
    else
    {
        Serial.println("-----------------------");
        Serial.println("Failed to connect to the WiFi network");
    }
}
