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
    const char *ssid;
    const char *password;
    const char *clientID;
    const char *mqtt_broker;
    const char *mqtt_port;
    const char *mqtt_username;
    const char *mqtt_password;
    const char *mqtt_topic;
    const char *uuid;
};
