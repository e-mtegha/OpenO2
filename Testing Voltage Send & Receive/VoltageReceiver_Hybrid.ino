const int inputPin = 2;

// === Pulse measurement ===
const int MAX_PULSES = 50;
volatile unsigned long pulsePeriods[MAX_PULSES];
volatile int pulseIndex = 0;
volatile unsigned long lastRiseTime = 0;
volatile bool dataReady = false;

// Filters
float smoothedVoltage = 12.0;  // start near nominal
float alpha = 0.2;             // exponential smoothing factor (0=very smooth, 1=very reactive)

void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(inputPin), pulseISR, RISING);
  Serial.println("UNO: Hybrid Frequency-to-Voltage monitoring ready.");
}

void loop() {
  if (dataReady) {
    noInterrupts(); // protect shared data
    int count = pulseIndex;
    unsigned long periods[MAX_PULSES];
    memcpy(periods, (const void*)pulsePeriods, count * sizeof(unsigned long));
    pulseIndex = 0;  // reset for next batch
    dataReady = false;
    interrupts();

    if (count > 0) {
      float sum = 0;
      int validCount = 0;
      for (int i = 0; i < count; i++) {
        if (periods[i] > 100 && periods[i] < 5000) {  // filter outliers
          sum += periods[i];
          validCount++;
        }
      }
      if (validCount > 0) {
        float avgPeriod = sum / validCount;
        float freq = 1000000.0 / avgPeriod;  // Hz
        float voltage = mapFloat(freq, 500.0, 4900.0, 10.0, 14.4);
        voltage = constrain(voltage, 10.0, 14.4);

        // === Rolling average buffer ===
        static float buffer[MAX_PULSES] = {0};
        static int bufferIndex = 0;
        static int bufferCount = 0;

        buffer[bufferIndex] = voltage;
        bufferIndex = (bufferIndex + 1) % MAX_PULSES;
        if (bufferCount < MAX_PULSES) bufferCount++;

        float rollingSum = 0;
        for (int i = 0; i < bufferCount; i++) rollingSum += buffer[i];
        float rollingAvg = rollingSum / bufferCount;

        // === Hybrid step: exponential smoothing ===
        smoothedVoltage = alpha * rollingAvg + (1 - alpha) * smoothedVoltage;

        // Serial.print("Voltage (hybrid filter): ");
        Serial.println(smoothedVoltage, 2);
        // Serial.println(" V");
      }
    }
  }
}

// ISR for pulse measurement
void pulseISR() {
  unsigned long now = micros();
  if (lastRiseTime != 0) {
    unsigned long period = now - lastRiseTime;
    if (period >= 50 && period <= 5000) {
      pulsePeriods[pulseIndex++] = period;
      if (pulseIndex >= MAX_PULSES) {
        pulseIndex = MAX_PULSES; // stop overflow
        dataReady = true;
      }
    }
  }
  lastRiseTime = now;
}

// Map float helper
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
