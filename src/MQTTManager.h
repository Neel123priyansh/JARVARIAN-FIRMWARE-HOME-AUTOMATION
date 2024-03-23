void connectToMQTT(void (*callback)(char *, byte *, unsigned short int))
{
    String server = config.mqtt.host.c_str();
    String port = config.mqtt.port.c_str();
    mqttclient.setServer(server.c_str(), port.toInt());
    mqttclient.setKeepAlive(60);
    mqttclient.setCallback(callback);

    if (mqttclient.connect(config.mqtt.clientID.c_str(), config.mqtt.username.c_str(), config.mqtt.password.c_str()))
    {
        mqttclient.subscribe(config.mqtt.topic.c_str());
        Serial.println("Connected to MQTT Broker");
        Serial.print("Subscribed to topic: ");
        Serial.println(config.mqtt.topic.c_str());
    }
    else
    {
        Serial.println("MQTT connection failed");
        Serial.print("Error code: ");
        Serial.println(mqttclient.state());
        statusBuzzer(5, 100);
    }
}

void publish_keep_alive_message(unsigned long &keep_alive_counter)
{
    StaticJsonDocument<256> doc;
    doc["origin"] = config.mqtt.clientID.c_str();
    doc["messageType"] = "keep_alive";
    doc["message"]["keep_alive_counter"] = keep_alive_counter;
    doc["message"]["uptime"] = millis() / 1000; // uptime in seconds

    String docString;
    serializeJson(doc, docString);
    mqttclient.publish(config.mqtt.topic.c_str(), docString.c_str());
}

void publish_current_state_message(int8_t statusPin, int8_t state, String deviceName)
{
    StaticJsonDocument<256> doc;
    doc["origin"] = config.mqtt.clientID.c_str();
    doc["messageType"] = "current_state";
    doc["message"]["device_id"] = deviceName;
    if (state == -1)
        doc["message"]["state"] = digitalRead(statusPin) ? "ON" : "OFF";
    else
        doc["message"]["state"] = state ? "ON" : "OFF";

    String docString;
    serializeJson(doc, docString);
    Serial.println("Response: " + docString);
    mqttclient.publish(config.mqtt.topic.c_str(), docString.c_str());
}

void publish_error_message(String message)
{
    StaticJsonDocument<256> doc;
    doc["origin"] = config.mqtt.clientID.c_str();
    doc["messageType"] = "error";
    doc["message"]["message"] = message;

    String docString;
    serializeJson(doc, docString);
    Serial.println("Response: " + docString);
    mqttclient.publish(config.mqtt.topic.c_str(), docString.c_str());
}

void mqttCallback(char *topic, byte *payload, unsigned short int length)
{
    // Convert payload to a JSON string
    String message;
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    // Parse JSON string to extract message details
    DynamicJsonDocument messageDoc(256);
    DeserializationError error = deserializeJson(messageDoc, message);
    if (error)
    {
        Serial.println("Not a valid JSON message");
        publish_error_message("Not a valid JSON message");
        return;
    }

    // Extract message details
    if (messageDoc["origin"].as<String>().equals(config.mqtt.clientID.c_str()))
        return;

    Serial.println("-----------------------");
    Serial.println("Message arrived in topic: " + String(topic));
    Serial.println("Message: " + String(message));

    // Handle change_state messages
    if (messageDoc["messageType"].as<String>().equals("change_state"))
    {
        Serial.println("Message type: change_state");

        String deviceId = messageDoc["message"]["device_id"].as<String>();
        String state = messageDoc["message"]["state"].as<String>();

        // Loop through the devices array in the JSON document
        for (size_t i = 0; i < config.devices.size(); i++)
        {
            if (deviceId.equals(String(config.devices[i].name.c_str())))
            {
                String name = config.devices[i].name.c_str();

                Serial.println("Device found in config file");
                Serial.println("Device name: " + name);
                Serial.println("Device ControlPin: " + String(config.devices[i].controlPin));
                Serial.println("Device StatusPin: " + String(config.devices[i].statusPin));

                if (state.equals("ON"))
                {
                    if (config.devices[i].state == 1)
                    {
                        Serial.println("Device is already ON");
                        publish_error_message("Device is already ON");
                        return;
                    }
                    trigerTTP223(config.devices[i]);
                    config.devices[i].state = 1;
                    publish_current_state_message(-1, 1, name);
                    return;
                }
                else if (state.equals("OFF"))
                {
                    if (config.devices[i].state == 0)
                    {
                        Serial.println("Device is already OFF");
                        publish_error_message("Device is already OFF");
                        return;
                    }
                    trigerTTP223(config.devices[i]);
                    config.devices[i].state = 0;
                    publish_current_state_message(-1, 0, name);
                    return;
                }
                else
                {
                    Serial.println("Invalid state" + state + ". Valid states are `ON` and `OFF`");
                    publish_error_message("Invalid state" + state + ". Valid states are `ON` and `OFF`");
                    return;
                }
            }
        }
        Serial.println("Device (" + deviceId + ") not found in config file");
        publish_error_message("Device (" + deviceId + ") not found in config file");
        return;
    }

    // Handle current_state messages
    if (messageDoc["messageType"].as<String>().equals("current_state"))
    {
        Serial.println("Message type: current_state");

        String deviceId = messageDoc["message"]["device_id"].as<String>();

        // Loop through the devices array in the JSON document
        for (size_t i = 0; i < config.devices.size(); i++)
        {
            String name = config.devices[i].name.c_str();

            if (deviceId.equals(name))
            {
                Serial.println("Device found in config file");
                Serial.println("Device name: " + name);
                Serial.println("Device ControlPin: " + String(config.devices[i].controlPin));
                Serial.println("Device StatusPin: " + String(config.devices[i].statusPin));

                publish_current_state_message(config.devices[i].statusPin, -1, name);
                return;
            }
        }
        Serial.println("Device (" + deviceId + ") not found in config file");
        return;
    }
}