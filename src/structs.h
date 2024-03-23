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
    std::string type;
    std::int8_t pin;
    Device(const std::string &name, const std::string &type, const std::int8_t &pin) : name(name), type(type), pin(pin) {}

    // std::int8_t state;
};

struct Config
{
    std::vector<Device> devices;
    WiFiConfig wifi;
    MQTTConfig mqtt;
};