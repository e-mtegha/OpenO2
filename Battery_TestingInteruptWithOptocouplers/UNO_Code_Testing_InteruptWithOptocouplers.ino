/*
# Arduino UNO – Simple Signal Exchange with Nano

## Overview
This Arduino UNO sketch tests communication with an Arduino Nano through opto-isolated signal lines.  
It sends a short trigger signal to the Nano, waits for a response, and reports the result via serial.

---

## Features
- Sends a HIGH pulse to the Nano to request a response.
- Waits for a reply within a fixed timeout period (1 second).
- Reports success or failure over Serial.
- Includes a pause between cycles to avoid continuous triggering.

---

## Pin Assignments
- triggerPin (7): Output pin sending request signal to Nano via optocoupler.
- receivePin (6): Input pin receiving response signal from Nano via optocoupler (with external pull-up).

---

## Operation Sequence
1. Send trigger signal:
   - Set `triggerPin` HIGH for 100 ms.
   - Then return it LOW.
2. Wait for Nano's response:
   - Continuously check `receivePin` for LOW state (response signal).
   - If received within 1 second → print success.
   - If no signal within 1 second → print failure.
3. Delay 2 seconds before repeating.

---
*/
const int triggerPin = 7; // Output to Nano (via opto)
const int receivePin = 6; // Input from Nano (via opto)

void setup() {
  pinMode(triggerPin, OUTPUT);
  pinMode(receivePin, INPUT); // External pull-up on D6
  digitalWrite(triggerPin, LOW); // Make sure it's LOW at start
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  Serial.println("UNO: Sending signal to Nano...");
  digitalWrite(triggerPin, HIGH);  // Turn on Opto 1
  delay(100);                      // Keep HIGH briefly
  digitalWrite(triggerPin, LOW);   // Back to LOW

  // Wait for Nano to respond
  unsigned long startTime = millis();
  bool received = false;

  while (millis() - startTime < 1000) { // 1 second timeout
    if (digitalRead(receivePin) == LOW) {
      Serial.println("UNO: Signal received back from Nano!");
      received = true;
      break;
    }
  }

  if (!received) {
    Serial.println("UNO: No response from Nano.");
  }

  delay(2000); // Wait before repeating the cycle
}
