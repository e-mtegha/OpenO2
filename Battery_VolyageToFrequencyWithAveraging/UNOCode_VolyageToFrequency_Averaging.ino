/*
# Arduino UNO – Averaged Frequency-to-Voltage Measurement from Nano

## Overview
This Arduino UNO sketch requests voltage data from an Arduino Nano and measures the frequency of the returned square wave.  
Unlike the single-pulse version, this code collects multiple pulses, averages their periods, and then converts the result into a voltage for improved accuracy and noise filtering.

---

## Features
- Sends a voltage request to Nano every 3 seconds.
- Collects multiple (up to 5) pulse periods for averaging.
- Filters out invalid/noisy measurements.
- Converts the average frequency to voltage using `mapFloat()`.
- Outputs clean, stable voltage readings over Serial.

---

## Pin Assignments
- requestPin (7): Output pin to send request to Nano via optocoupler.
- inputPin (2): Interrupt input to receive square wave from Nano via optocoupler.

---

## Measurement Process
1. Every 3 seconds:
   - Reset measurement variables.
   - Send a short request pulse to Nano.
   - Begin capturing rising edges from Nano’s response signal.
2. On each rising edge:
   - Measure the time since the last edge.
   - Store valid pulse periods (ignoring out-of-range noise).
3. Once 5 pulses are collected:
   - Compute the average period.
   - Convert to frequency, then voltage.
4. Output the voltage to the Serial Monitor.

---

## Frequency-to-Voltage Conversion
The formula used:  
Voltage = mapFloat(frequency, 500 Hz, 10,000 Hz, 10.0 V, 14.4 V)
  - Frequencies below 500 Hz map to 10.0 V.
- Frequencies above 10 kHz map to 14.4 V.
- Output constrained between 10.0 V and 14.4 V.

---
*/
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
