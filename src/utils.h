
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