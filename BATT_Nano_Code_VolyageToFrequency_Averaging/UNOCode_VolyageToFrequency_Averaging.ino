const int requestPin = 7;
const int inputPin = 2;

volatile unsigned long lastRiseTime = 0;
volatile unsigned long pulsePeriods[5];  // Store pulse periods
volatile int pulseIndex = 0;
volatile int pulseCount = 0;

unsigned long lastRequestTime = 0;
const unsigned long requestInterval = 3000;

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

  // Send request every 3 seconds
  if (currentTime - lastRequestTime >= requestInterval) {
    lastRequestTime = currentTime;

    // Reset before new measurement
    pulseCount = 0;
    pulseIndex = 0;
    lastRiseTime = 0;
    dataReady = false;
    isMeasuring = true;

    requestVoltageFromNano();
    delay(50);  // Allow tone to stabilize
  }

  // Process after N valid pulses
  if (dataReady && isMeasuring) {
    isMeasuring = false;

    // Compute average period
    float sum = 0;
    int validCount = 0;

    for (int i = 0; i < pulseIndex; i++) {
      unsigned long p = pulsePeriods[i];
      if (p > 100 && p < 2000) { // filter out noise
        sum += p;
        validCount++;
      }
    }

    if (validCount > 0) {
      float avgPeriod = sum / validCount;
      float freq = 1000000.0 / avgPeriod;
      float voltage = mapFloat(freq, 500.0, 10000.0, 10.0, 14.4);
      voltage = constrain(voltage, 10.0, 14.4);

      Serial.print("Voltage: ");
      Serial.print(voltage, 3);
      Serial.println(" V");
    } else {
      Serial.println("UNO: Invalid or noisy signal — no valid pulses.");
    }

    dataReady = false;
  }
}

void requestVoltageFromNano() {
  digitalWrite(requestPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(requestPin, LOW);
}

void pulseISR() {
  if (!isMeasuring) return;

  unsigned long now = micros();

  if (lastRiseTime != 0 && pulseIndex < 10) {
    unsigned long period = now - lastRiseTime;

    if (period >= 50 && period <= 5000) {  // reasonable bounds
      pulsePeriods[pulseIndex++] = period;
      pulseCount++;
    }

    if (pulseCount >= 5) {
      dataReady = true;
    }
  }

  lastRiseTime = now;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
