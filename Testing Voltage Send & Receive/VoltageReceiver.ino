// === Pins ===
const int inputPin = 2;  // Interrupt input for alternator signal

// === Pulse measurement ===
volatile unsigned long lastRiseTime = 0;
volatile unsigned long pulsePeriods[50];
volatile int pulseIndex = 0;
volatile int pulseCount = 0;

bool dataReady = false;


void setup() {
  Serial.begin(9600);

  pinMode(inputPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(inputPin), pulseISR, RISING);

  Serial.println("UNO: Frequency-to-voltage monitoring ready.");
  interrupts();
}

void loop() {
  if (dataReady) {
    dataReady = false;

    float sum = 0;
    int validCount = 0;

    // Average
    for (int i = 0; i < pulseIndex; i++) {
      unsigned long p = pulsePeriods[i];
      if (p > 100 && p < 2000) {
        sum += p;
        validCount++;
      }
    }

    if (validCount > 0) {
      float avgPeriod = sum / validCount;
      float freq = 1000000.0 / avgPeriod;  // Frequency in Hz
      float alternatorVoltage = mapFloat(freq, 500.0, 4900.0, 10.0, 14.4);
      alternatorVoltage = constrain(alternatorVoltage, 10.0, 14.4);

      Serial.print("Voltage (from frequency): ");
      Serial.print(alternatorVoltage, 2);
      Serial.println(" V");
      delay(100);
    }

    pulseIndex = 0;
    pulseCount = 0;
    interrupts();  // Re-enables all interrupts globally
  }
}

// ISR for pulse measurement
void pulseISR() {
  unsigned long now = micros();

  if (lastRiseTime != 0 && pulseIndex < 50) {
    unsigned long period = now - lastRiseTime;
    if (period >= 50 && period <= 5000) {
      pulsePeriods[pulseIndex++] = period;
      pulseCount++;
    }

    if (pulseCount >= 5) {
      dataReady = true;
      noInterrupts();  // Disables all interrupts globally
    }
  }

  lastRiseTime = now;
}

// Helper function to map float values
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
