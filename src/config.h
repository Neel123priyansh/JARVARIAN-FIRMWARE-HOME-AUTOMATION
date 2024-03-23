void loadConfig(struct Config &config)
{
    // Initialize WiFi configuration
    config.wifi.ssid = "SwastikWiFi";
    config.wifi.password = "jarvis@wifi";
    config.wifi.clientID = "IoT-Device-ESP32";
    config.wifi.ip = "192.168.0.232";
    config.wifi.gateway = "192.168.0.1";
    config.wifi.subnet = "255.255.255.0";
    config.wifi.dns = "192.168.0.1";

    // Initialize MQTT configuration
    config.mqtt.host = "192.168.0.254";
    config.mqtt.port = "1883";
    config.mqtt.clientID = "IoT-Device-ESP32";
    config.mqtt.username = "mosquitto";
    config.mqtt.password = "mosquitto";
    config.mqtt.topic = "home/2/bedroom";

    // Initialize devices
    config.devices = {
        {"BLUE_LED", "OUTPUT", 5},
        {"YELLOW_LED", "OUTPUT", 4},
        {"RED_LED", "OUTPUT", 0},
        {"GREEN_LED", "OUTPUT", 14},
    };
}
