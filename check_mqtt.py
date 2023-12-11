import paho.mqtt.publish as publish
import json

# Read the config file and get the values
with open('data/config.json', 'r', encoding='utf-8') as json_data_file:
    config = json.load(json_data_file)
    
mqtt_host = config['mqtt']['host']
username = config['mqtt']['username']
password = config['mqtt']['password']
topic = config['mqtt']['topic']

current_state_message = {
    'origin': 'check_mqtt.py',
    'messageType': 'current_state',
    'message': {
        'device_id': 'YELLOW_LED'
    }
}

publish.single(topic, json.dumps(current_state_message), hostname=mqtt_host, auth={'username':username, 'password':password})

# change_state_message = {
#     'origin': 'check_mqtt.py',
#     'messageType': 'change_state',
#     'message': {
#         'device_id': 'BLUE_LED',
#         'state': 'ON'
#     }
# }

# publish.single(topic, json.dumps(change_state_message), hostname=mqtt_host, auth={'username':username, 'password':password})