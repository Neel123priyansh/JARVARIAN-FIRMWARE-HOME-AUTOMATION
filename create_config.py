"""This file creates `src/pinConfig.h` file based on `data/config.json` file."""

import json

def generate_load_function(json_data):
    load_function = "// This file is generated automaticly by `create_config.py`\n\n"
    load_function += "void loadConfig(struct Config &config) {\n"
    load_function += "    // Initialize WiFi configuration\n"
    load_function += f"    config.wifi.ssid = \"{json_data['wifi']['ssid']}\";\n"
    load_function += f"    config.wifi.password = \"{json_data['wifi']['password']}\";\n"
    load_function += f"    config.wifi.clientID = \"{json_data['wifi']['clientID']}\";\n"
    load_function += f"    config.wifi.ip = \"{json_data['wifi']['ip']}\";\n"
    load_function += f"    config.wifi.gateway = \"{json_data['wifi']['gateway']}\";\n"
    load_function += f"    config.wifi.subnet = \"{json_data['wifi']['subnet']}\";\n"
    load_function += f"    config.wifi.dns = \"{json_data['wifi']['dns']}\";\n\n"

    load_function += "    // Initialize MQTT configuration\n"
    load_function += f"    config.mqtt.host = \"{json_data['mqtt']['host']}\";\n"
    load_function += f"    config.mqtt.port = \"{json_data['mqtt']['port']}\";\n"
    load_function += f"    config.mqtt.clientID = \"{json_data['mqtt']['clientID']}\";\n"
    load_function += f"    config.mqtt.username = \"{json_data['mqtt']['username']}\";\n"
    load_function += f"    config.mqtt.password = \"{json_data['mqtt']['password']}\";\n"
    load_function += f"    config.mqtt.topic = \"{json_data['mqtt']['topic']}\";\n\n"

    load_function += "    // Initialize devices\n"
    load_function += "    config.devices = {\n"
    for device in json_data['devices']:
        load_function += f"\t\tDevice(\"{device['name']}\",{device['statusPin']},{device['controlPin']}),\n"
    load_function += "    };\n}\n"

    return load_function

# Write config.h file
with open('src/pinConfig.h', 'w', encoding='utf-8') as file:
    with open('data/config.json', 'r', encoding='utf-8') as json_file:
        config = json.loads(json_file.read())  # Read the content of the file
    file.write(generate_load_function(config))
