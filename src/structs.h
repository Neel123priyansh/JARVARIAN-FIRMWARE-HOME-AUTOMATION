#include <string>
#include <vector>

struct WiFiConfig
{
    std::string ssid;
    std::string password;
    std::string clientID;
    std::string ip;
    std::string gateway;
    std::string subnet;
    std::string dns;
};

struct MQTTConfig
{
    std::string host;
    std::string port;
    std::string clientID;
    std::string username;
    std::string password;
    std::string topic;
};

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
    WiFiConfig wifi;
    MQTTConfig mqtt;
};