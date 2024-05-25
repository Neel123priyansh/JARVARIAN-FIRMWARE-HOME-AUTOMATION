
#define STATUS_BUZZER 27
#define dataPin 14
#define latchPin 27
#define clockPin 26
#define WIFI_SSID_ADDR 0
#define WIFI_PASS_ADDR 100
#define EEPROM_SIZE 512

uint8_t outputState = 0; 

void statusBuzzer(int times, int delayTime)
{
    for (int i = 0; i < times; i++)
    {
        digitalWrite(STATUS_BUZZER, HIGH);
        delay(delayTime);
        digitalWrite(STATUS_BUZZER, LOW);
        delay(delayTime);
    }
}



void updateShiftRegister() {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, outputState);
  digitalWrite(latchPin, HIGH);

}

void setOutput(uint8_t pin) {
  if (pin < 8) {
    bitSet(outputState, pin);  // Set the bit corresponding to the pin
    updateShiftRegister();     // Update the shift register with the new state
  }
}

void clearOutput(uint8_t pin) {
  if (pin < 8) {
    bitClear(outputState, pin);  // Clear the bit corresponding to the pin
    updateShiftRegister();       // Update the shift register with the new state
  }
}


void trigerTTP223(Device &device)
{
    Serial.println("-----------------------");
    Serial.println("Trigerring TTP223 sensor...");
    digitalWrite(device.controlPin, HIGH);
    delay(100);
    digitalWrite(device.controlPin, LOW);
    Serial.println("TTP223 sensor triggered");
}