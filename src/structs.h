#include <string>
#include <vector>

struct Device
{
    std::string name;
    std::int8_t statusPin;
    std::int8_t controlPin;
    std::int8_t state; // 0: OFF, 1: ON
    Device(const std::string &name, const std::int8_t &statusPin, const std::int8_t &controlPin, const std::int8_t &state = 0) : name(name), statusPin(statusPin), controlPin(controlPin), state(state) {}
};

struct Config
{
    std::vector<Device> devices;
    const char *wifi_ssid;
    const char *wifi_password;
    const char *clientID;
    const char *mqtt_broker;
    const char *mqtt_port;
    const char *mqtt_username;
    const char *mqtt_password;
    const char *user_uuid;

    const char *validate() const
    {
        if (wifi_ssid == nullptr || strlen(wifi_ssid) == 0)
            return "wifi_ssid";
        if (wifi_password == nullptr || strlen(wifi_password) == 0)
            return "wifi_password";
        if (clientID == nullptr || strlen(clientID) == 0)
            return "clientID";
        if (mqtt_broker == nullptr || strlen(mqtt_broker) == 0)
            return "mqtt_broker";
        if (mqtt_port == nullptr || strlen(mqtt_port) == 0)
            return "mqtt_port";
        if (mqtt_username == nullptr || strlen(mqtt_username) == 0)
            return "mqtt_username";
        if (mqtt_password == nullptr || strlen(mqtt_password) == 0)
            return "mqtt_password";
        if (user_uuid == nullptr || strlen(user_uuid) == 0)
            return "user_uuid";
        return nullptr; // All fields are valid
    }
};