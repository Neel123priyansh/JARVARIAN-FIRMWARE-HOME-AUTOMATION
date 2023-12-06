import paho.mqtt.publish as publish

# MQTT broker details
broker_address = "192.168.0.254"
port = 1883
username = "mosquitto"
password = "mosquitto"

# Topic and message
topic = "home/2/bedroom"
message = '{"pin":"D0","state":"ON"}'

# Publish the message
publish.single(topic, payload=message, hostname=broker_address, port=port, auth={'username': username, 'password': password})
