void statusBuzzer(int times, int delayTime)
{
    for (int i = 0; i < times; i++)
    {
        digitalWrite(STATUS_BUZZER, HIGH);
        delay(delayTime);
        digitalWrite(STATUS_BUZZER, LOW);
        delay(delayTime);
    }
}

void printConfig(Config &config) {
    Serial.println("-----------------------");
    // Print WiFi configuration
    Serial.println("WiFi Configuration:");
    Serial.print("SSID: ");
    Serial.println(config.wifi.ssid.c_str());
    Serial.print("Password: ");
    Serial.println(config.wifi.password.c_str());
    Serial.print("Client ID: ");
    Serial.println(config.wifi.clientID.c_str());
    Serial.print("IP: ");
    Serial.println(config.wifi.ip.c_str());
    Serial.print("Gateway: ");
    Serial.println(config.wifi.gateway.c_str());
    Serial.print("Subnet: ");
    Serial.println(config.wifi.subnet.c_str());
    Serial.print("DNS: ");
    Serial.println(config.wifi.dns.c_str());

    // Print MQTT configuration
    Serial.println("\nMQTT Configuration:");
    Serial.print("Host: ");
    Serial.println(config.mqtt.host.c_str());
    Serial.print("Port: ");
    Serial.println(config.mqtt.port.c_str());
    Serial.print("Client ID: ");
    Serial.println(config.mqtt.clientID.c_str());
    Serial.print("Username: ");
    Serial.println(config.mqtt.username.c_str());
    Serial.print("Password: ");
    Serial.println(config.mqtt.password.c_str());
    Serial.print("Topic: ");
    Serial.println(config.mqtt.topic.c_str());

    // Print devices configuration
    Serial.println("\nDevices:");
    for (const auto &device : config.devices) {
        Serial.print("Name: ");
        Serial.print(device.name.c_str());
        Serial.print(" | StatusPin: ");
        Serial.print(String(device.statusPin).c_str());
        Serial.print(" | ControlPin: ");
        Serial.print(String(device.controlPin).c_str());
        Serial.println();
    }
}

void trigerTTP223(Device &device) {
    Serial.println("-----------------------");
    Serial.println("Trigerring TTP223 sensor...");
    digitalWrite(device.controlPin, HIGH);
    delay(100);
    digitalWrite(device.controlPin, LOW);
    Serial.println("TTP223 sensor triggered");
}