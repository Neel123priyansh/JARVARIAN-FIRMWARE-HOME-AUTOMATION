### STATUS_BUZZER ERROR CODES

* Continous Beep -> Updating Firmware
* `1 Beep` -> API Call Received
* `2 Beeps` -> Connecting to WiFi
* `3 Beeps:`
  1. Failed to mount LittleFS
  2. Failed to open `config.json` OR `config.json` not found
  3. Failed to parse `config.json`
* `4 Beeps` -> Invalid pin found in `config.json`
* `5 Beeps` -> Failed to connect to MQTT Server.

### MQTT

* reconnection if disconnected from server, every `10 seconds`. (5 Beeps)
* `keep-alive` message every 10s
* for `status` message, responds with `OK`.
* Server Message:
  ```json
  {
  "pin": "12",	// Pin Number
  "state": "ON" 	// ["ON", "OFF"]
  }
  ```
  Client Message: `OK`
* `Invalid state`, if server sends invalid `state` in message.
* `Not a valid JSON message`, if server sends invalid JSON message.
