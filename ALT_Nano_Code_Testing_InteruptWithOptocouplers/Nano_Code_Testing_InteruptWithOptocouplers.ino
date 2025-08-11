/*
# Overview
This Arduino Nano sketch listens for a signal from an Arduino Uno through an optocoupler on an interrupt pin.
When the signal is received, the Nano sends a short HIGH pulse back to the Uno through another optocoupler to acknowledge it.

## Hardware Connections
- `inputPin (D2)` → Connected to Uno output (via optocoupler 1), with an external pull-up resistor.
- `replyPin (D9)` → Output to Uno input (via optocoupler 2).
- Communication is isolated through optocouplers to protect both devices.

## Functional Flow
1. **Interrupt Listening:**  
   The Nano uses an interrupt on `inputPin` to instantly detect a signal from the Uno.
2. **Signal Handling:**  
   Once the signal is received, it sets a flag (`signalReceived = true`).
3. **Reply Pulse:**  
   In the main loop, when the flag is set, it sends a short HIGH pulse to the Uno via `replyPin` to confirm receipt.

## Notes
- Using interrupts ensures near-instant response without having to constantly poll the pin.
- The reply duration is fixed at `100 ms` — long enough for the Uno to detect reliably, but short enough not to cause overlap in communications.
- The `delay(10)` in the loop keeps CPU usage low without noticeably affecting responsiveness.

---

# Code
*/
const int inputPin = 2;  // Interrupt pin from UNO (via opto)
const int replyPin = 9;  // Output to UNO (via opto)

volatile bool signalReceived = false;

void setup() {
  pinMode(inputPin, INPUT); // External pull-up on D2
  pinMode(replyPin, OUTPUT);
  digitalWrite(replyPin, LOW); // Make sure LOW at start
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(inputPin), onSignal, FALLING);
}

void loop() {
  if (signalReceived) {
    Serial.println("NANO: Signal received from UNO!");

    // Send reply to UNO
    digitalWrite(replyPin, HIGH); // Activate Opto 2
    delay(100);
    digitalWrite(replyPin, LOW);  // End signal

    Serial.println("NANO: Reply sent to UNO.");
    signalReceived = false;
  }

  delay(10); // Short loop delay to reduce CPU usage
}

void onSignal() {
  signalReceived = true;
}
