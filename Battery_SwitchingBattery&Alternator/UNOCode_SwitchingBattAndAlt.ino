/*
# Arduino UNO – Automatic Alternator/Battery Switching System

## Overview
This Arduino UNO sketch manages automatic switching between alternator and battery power based on measured alternator voltage.  
It works with an Arduino Nano that sends a tone signal representing alternator voltage. The UNO measures this tone frequency, maps it to a voltage, and switches relays accordingly.

---

## Features
- Requests alternator voltage signal from the Nano periodically.
- Measures tone frequency using interrupts.
- Maps frequency to voltage range (10.0–14.4 V).
- Switches between battery and alternator relays based on threshold (12.3 V).
- Frequent checks to avoid power interruptions.

---

## Pin Assignments
- requestPin (7): Signal output to request voltage from Nano.
- inputPin (2): Interrupt input for tone signal from Nano.
- batterySwitchPin (5): Controls battery relay.
- alternatorSwitchPin (4): Controls alternator relay.
- analogPin (A0): Reserved for voltage divider measurement (not directly used here).

---

## Voltage Divider Setup
- R1 = 2.0 Ω, R2 = 1.0 Ω — Configured for battery voltage measurement.
- VCC = 5.00 V — Arduino reference voltage for analog reads.

---

## Pulse Measurement
- Measures time between rising edges of tone signal to determine frequency.
- Uses `pulsePeriods[]` to store recent valid measurements.
- `mapFloat()` function converts frequency to voltage.

---

## Switching Logic
1. If alternator voltage > 12.3 V → Switch to alternator relay ON, battery relay OFF.
2. Else → Switch to battery relay ON, alternator relay OFF.
3. If no valid tone is received → Report error.

---*/

// === Pins ===
const int requestPin = 7;     // Output to request alternator data
const int inputPin = 2;       // Interrupt input for alternator signal
const int batterySwitchPin = 5;
const int alternatorSwitchPin = 4;

// === Voltage Divider for Battery ===
const int analogPin = A0;
const float R1 = 2.0;
const float R2 = 1.0;
const float VCC = 5.00;

// === Pulse measurement ===
volatile unsigned long lastRiseTime = 0;
volatile unsigned long pulsePeriods[5];
volatile int pulseIndex = 0;
volatile int pulseCount = 0;

unsigned long lastRequestTime = 0;
const unsigned long requestInterval = 200; // Fast enough to avoid load shutdown

bool dataReady = false;
bool isMeasuring = false;

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(requestPin, OUTPUT);
  digitalWrite(requestPin, LOW);

  pinMode(inputPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(inputPin), pulseISR, RISING);

  pinMode(batterySwitchPin, OUTPUT);
  pinMode(alternatorSwitchPin, OUTPUT);

  digitalWrite(batterySwitchPin, LOW);
  digitalWrite(alternatorSwitchPin, LOW);

  Serial.println("UNO: Auto switch system ready.");
}

void loop() {
  unsigned long currentTime = millis();

  // Request alternator voltage frequently
  if (currentTime - lastRequestTime >= requestInterval) {
    lastRequestTime = currentTime;

    pulseCount = 0;
    pulseIndex = 0;
    lastRiseTime = 0;
    dataReady = false;
    isMeasuring = true;

    requestVoltageFromNano();
    delay(50);  // Allow tone to stabilize
  }

  // When tone data is ready
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

      // === Switching Logic ===
      if (alternatorVoltage > 12.3) {
        digitalWrite(alternatorSwitchPin, HIGH);
        digitalWrite(batterySwitchPin, LOW);
        Serial.println("Switching to Alternator.");
      } else {
        digitalWrite(alternatorSwitchPin, LOW);
        digitalWrite(batterySwitchPin, HIGH);
        Serial.println("Switching to Battery.");
      }

    } else {
      Serial.println("UNO: Invalid tone signal.");
    }

    dataReady = false;
  }
}

void requestVoltageFromNano() {
  digitalWrite(requestPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(requestPin, LOW);
}

// ISR for pulse measurement
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

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
