// === Pins ===
const int requestPin = 7;     // Output to request alternator data
const int inputPin = 2;       // Interrupt input for alternator signal
const int analogPin = A0;     // Analog pin for battery voltage

// === Voltage Divider for Battery ===
const float R1 = 2.0; // Top resistor
const float R2 = 1.0; // Bottom resistor
const float VCC = 5.00; // Assume 5V reference for ADC

// === Pulse measurement variables ===
volatile unsigned long lastRiseTime = 0;
volatile unsigned long pulsePeriods[5];
volatile int pulseIndex = 0;
volatile int pulseCount = 0;

unsigned long lastRequestTime = 0;
unsigned long lastBatteryTime = 0;

const unsigned long requestInterval = 3000;
const unsigned long batteryInterval = 1000;

bool dataReady = false;
bool isMeasuring = false;

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(requestPin, OUTPUT);
  digitalWrite(requestPin, LOW);

  pinMode(inputPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(inputPin), pulseISR, RISING);

  Serial.println("UNO: Ready.");
}

void loop() {
  unsigned long currentTime = millis();

  // === Battery Voltage Display Every 1 Second ===
  if (currentTime - lastBatteryTime >= batteryInterval) {
    lastBatteryTime = currentTime;

    int adcValue = analogRead(analogPin);
    float vOut = adcValue * (VCC / 1023.0);
    float batteryVoltage = vOut * ((R1 + R2) / R2);

    Serial.print("Battery Voltage: ");
    Serial.print(batteryVoltage, 3);
    Serial.println(" V");
  }

  // === Alternator Request Every 3 Seconds ===
  if (currentTime - lastRequestTime >= requestInterval) {
    lastRequestTime = currentTime;

    pulseCount = 0;
    pulseIndex = 0;
    lastRiseTime = 0;
    dataReady = false;
    isMeasuring = true;

    requestVoltageFromNano();
    delay(50);  // Wait for tone to stabilize
  }

  // === Process Alternator Data When Ready ===
  if (dataReady && isMeasuring) {
    isMeasuring = false;

    float sum = 0;
    int validCount = 0;

    for (int i = 0; i < pulseIndex; i++) {
      unsigned long p = pulsePeriods[i];
      if (p > 100 && p < 2000) {
        sum += p;
        validCount++;
      }
    }

    if (validCount > 0) {
      float avgPeriod = sum / validCount;
      float freq = 1000000.0 / avgPeriod;
      float alternatorVoltage = mapFloat(freq, 500.0, 10000.0, 10.0, 14.4);
      alternatorVoltage = constrain(alternatorVoltage, 10.0, 14.4);

      Serial.print("Alternator Voltage: ");
      Serial.print(alternatorVoltage, 3);
      Serial.println(" V");
    } else {
      Serial.println("UNO: Invalid or noisy alternator signal.");
    }

    dataReady = false;
  }
}

// === Request a Pulse from Nano ===
void requestVoltageFromNano() {
  digitalWrite(requestPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(requestPin, LOW);
}

// === ISR to Measure Pulse Periods ===
void pulseISR() {
  if (!isMeasuring) return;

  unsigned long now = micros();

  if (lastRiseTime != 0 && pulseIndex < 10) {
    unsigned long period = now - lastRiseTime;

    if (period >= 50 && period <= 5000) {
      pulsePeriods[pulseIndex++] = period;
      pulseCount++;
    }

    if (pulseCount >= 5) {
      dataReady = true;
    }
  }

  lastRiseTime = now;
}

// === Map Float Values (like Arduino's map but for floats) ===
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
