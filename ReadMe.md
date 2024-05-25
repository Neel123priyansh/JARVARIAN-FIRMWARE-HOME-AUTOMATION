# HOME AUTOMATION FIRMWARE

- In ESP32, pin `27` is for `STATUS_BUZZER` and pin `2` is for `BUILTIN_LED`.
- In ESP8266, pin `D0` is for `STATUS_BUZZER` and pin `D4` is for `BUILTIN_LED`.

## Setup Device

- Firmware

```python
python -m esptool ...
```

- File System

```python
python -m esptool ...
```

## ESP8266 Pinout

```c
    D0  = 16; // STATUS_BUZZER
    D1  = 5;
    D2  = 4;
    D3  = 0; # FLASH BUTTON
    D4  = 2;  // BUILTIN_LED
    D5  = 14; # HSCLK
    D6  = 12; # HMISO
    D7  = 13; # HMOSI
    D8  = 15; # HCS
```

![NodeMCU Pinout](extras/pinout_diagrams/esp8266-nodemcu/pinout.png)
[Learn More about NodeMCU Pins](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/)

## ESP32 Pinout

```c

```

## STATUS_BUZZER ERROR CODES

- Continous Beep -> Updating Firmware
- `1 Beep` -> API Call Received
- `2 Beeps` -> Connecting to WiFi
- `3 Beeps:`
  1. Failed to mount LittleFS
  2. Failed to open `config.json` OR `config.json` not found
  3. Failed to parse `config.json`
- `4 Beeps` -> Invalid pin found in `config.json`
- `5 Beeps` -> Failed to connect to MQTT Server.

## MQTT

- reconnection if disconnected from server, every `10 seconds`. (5 Beeps)
- `keep-alive` message every 10s
- `Invalid state`, if server sends invalid `state` in message.
- `Not a valid JSON message`, if server sends invalid JSON message.

### MQTT Message Format

- `keep_alive` message:

  ```json
  {
    "origin": "$wifi_hostname", // from config.json
    "messageType": "keep_alive",
    "message": {
      "keep_alive_counter": 10, // number of keep-alive messages sent from last reboot (starting from 1)
      "uptime": 100 // uptime in seconds
    }
  }
  ```

- `current_state` message:

  - From server to device:

    ```json
    {
      "origin": "controller",
      "messageType": "current_state",
      "message": {
        "device_id": "Bulb" // device id should be defined in config.json (case sensitive)
      }
    }
    ```

  - From device to server:

    ```json
    {
    "origin": "$wifi_hostname",
    "messageType": "current_state",
    "message": {
      "device_id": "Bulb",
      "state": "ON" // current state of device ('ON' or 'OFF')
    }
    ```

- `change_state` message:

  - From server to device:

    ```json
    {
      "origin": "controller",
      "messageType": "change_state",
      "message": {
        "device_id": "Bulb",
        "state": "ON" // target state ('ON' or 'OFF')
      }
    }
    ```

  - From device to server:

    ```json
    {
      "origin": "$wifi_hostname",
      "messageType": "current_state",
      "message": {
        "device_id": "Bulb",
        "state": "ON" // current state ('ON' or 'OFF')
      }
    }
    ```

- `error` message:

  - From device to server:

    ```json
    {
      "origin": "$wifi_hostname",
      "messageType": "error",
      "message": {
        "message": "ERROR_MESSAGE"
      }
    }
    ```

    > `TODO: Add all possible error messages here.`
    >
  - If `state` is changed using physical buttons: (Not Implemented Yet)

    - From device to server:

      ```json
      {
        "origin": "$wifi_hostname",
        "messageType": "current_state",
        "message": {
          "device_id": "Bulb",
          "state": "ON"
        }
      }
      ```

## WiFi

- WiFi reconnects, if disconnected.

## See Team Contributions here

[https://github.com/Jarvis-AI-project/JARVARIAN-FIRMWARE-HOME-AUTOMATION/graphs/contributors](https://github.com/Jarvis-AI-project/JARVARIAN-FIRMWARE-HOME-AUTOMATION/graphs/contributors)
