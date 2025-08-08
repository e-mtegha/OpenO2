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
