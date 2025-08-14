// === Voltage Divider Constants ===
const float R1 = 19790.0; // Top resistor
const float R2 = 9830.0;  // Bottom resistor
const int analogPin = A0;

// === Communication Pins ===
const int interruptPin = 2;  // From Uno (via optocoupler 1)
const int tonePin = 9;       // Output to Uno (via optocoupler 2) - must be OC1A

// === Control ===
volatile bool sendVoltageFlag = false;  // Set by ISR

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("NANO: Starting voltage sender system...");

  pinMode(analogPin, INPUT);
  pinMode(interruptPin, INPUT);  // Use external pull-up resistor
  pinMode(tonePin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(interruptPin), requestHandler, FALLING);

  Serial.println("NANO: Interrupt attached, system ready.");
}

void loop() {
  // Live voltage print (optional)
  float vinLive = readVoltage();
  Serial.print("NANO: Live Voltage = ");
  Serial.print(vinLive, 3);
  Serial.println(" V");

  delay(1000);

  if (sendVoltageFlag) {
    sendVoltageFlag = false;

    Serial.println("NANO: Interrupt received — sending voltage...");

    // 1. Read voltage
    float voltage = readVoltage();
    Serial.print("NANO: Read voltage = ");
    Serial.print(voltage, 3);
    Serial.println(" V");

    // 2. Map voltage to frequency
    int freq = mapFloat(voltage, 10.0, 14.4, 500, 10000);
    freq = constrain(freq, 500, 10000);

    Serial.print("NANO: Mapped frequency = ");
    Serial.print(freq);
    Serial.println(" Hz");

    // 3. Send frequency pulse using Timer1
    sendFrequency(freq, 20);  // send 20 cycles

    Serial.println("NANO: Frequency burst complete.");
  }
}

// === ISR: triggered by Uno request ===
void requestHandler() {
  sendVoltageFlag = true;
}

// === Voltage Reading Function ===
float readVoltage() {
  const float vcc = 5.00;
  int adcValue = analogRead(analogPin);
  float vOut = adcValue * (vcc / 1023.0);
  float vin = vOut * ((R1 + R2) / R2);
  return vin;
}

// === Frequency Sender using Timer1 ===
void sendFrequency(int frequency, int numCycles) {
  // Calculate compare match register for desired frequency
  int toggleCount = numCycles * 2; // Each cycle = 2 toggles (high + low)
  unsigned long ocrValue = (16000000UL / (2UL * frequency)) - 1;

  // Configure Timer1
  TCCR1A = _BV(COM1A0);             // Toggle OC1A (D9) on compare match
  TCCR1B = _BV(WGM12) | _BV(CS10);  // CTC mode, prescaler = 1 (no prescale)
  OCR1A = ocrValue;

  // Wait until toggleCount is reached
  for (int i = 0; i < toggleCount; i++) {
    while (!(TIFR1 & _BV(OCF1A))); // Wait for compare match
    TIFR1 |= _BV(OCF1A);           // Clear the flag manually
  }

  // Stop Timer1
  TCCR1A = 0;
  TCCR1B = 0;
  digitalWrite(tonePin, LOW); // Ensure output is low
}

// === Float to Int Mapping ===
int mapFloat(float x, float in_min, float in_max, int out_min, int out_max) {
  return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}
