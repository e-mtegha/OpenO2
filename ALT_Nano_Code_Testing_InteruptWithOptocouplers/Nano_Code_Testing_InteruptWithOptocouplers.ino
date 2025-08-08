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
