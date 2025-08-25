/*
# Arduino UNO – Voltage Request & Frequency-to-Voltage Conversion from Nano

## Overview
This Arduino UNO sketch communicates with an Arduino Nano to request voltage data.  
The Nano responds with a square wave whose frequency corresponds to a measured voltage.  
The UNO measures the wave’s period via an interrupt, converts it into a voltage, and prints the result to Serial.

---

## Features
- Requests voltage data from Nano every 3 seconds.
- Measures incoming square wave frequency using hardware interrupts.
- Converts frequency to voltage using a float-based mapping function.
- Filters invalid pulse measurements.
- Outputs voltage reading over Serial.

---

## Pin Assignments
- requestPin (7): Output pin to send request to Nano via optocoupler 1.
- inputPin (2): Interrupt input to receive square wave from Nano via optocoupler 2.

---

## Operation Sequence
1. Every 3 seconds, send a short request pulse to the Nano.
2. Wait for Nano’s square wave response.
3. Using interrupts:
   - Record the time between two rising edges.
   - Calculate the square wave’s period.
4. Convert the frequency into voltage using a `mapFloat()` function.
5. Output the voltage value via Serial.

---

## Frequency-to-Voltage Conversion
The formula used:  
Voltage = mapFloat(frequency, 500 Hz, 10,000 Hz, 10.0 V, 14.4 V)
  - Frequencies below 500 Hz are mapped to 10.0 V.
- Frequencies above 10 kHz are mapped to 14.4 V.
- Results are constrained between 10.0 V and 14.4 V.

---
  */
const int requestPin = 7;        // Sends request to Nano (via optocoupler 1)
const int inputPin = 2;          // Receives square wave from Nano (via optocoupler 2)

volatile unsigned long lastRiseTime = 0;
volatile unsigned long pulsePeriod = 0;
volatile int pulseCount = 0;

unsigned long lastRequestTime = 0;
const unsigned long requestInterval = 3000; // Request every 3 seconds

bool dataReady = false;

void setup() {
  Serial.begin(9600);
  delay(1000);  // Allow serial monitor to sync

  pinMode(requestPin, OUTPUT);
  digitalWrite(requestPin, LOW); // Ensure LOW initially

  pinMode(inputPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(inputPin), pulseISR, RISING);
}

void loop() {
  unsigned long currentTime = millis();

  // Request voltage from Nano every few seconds
  if (currentTime - lastRequestTime >= requestInterval) {
    lastRequestTime = currentTime;

    // Reset pulse tracking before new request
    pulsePeriod = 0;
    pulseCount = 0;
    dataReady = false;

    requestVoltageFromNano();
    delay(50); // Allow tone signal to stabilize
  }

  // Process only if 2 valid pulses were received
  if (dataReady && pulsePeriod > 0 && pulsePeriod < 2000) {
    float frequency = 1000000.0 / pulsePeriod;
    float voltage = mapFloat(frequency, 500.0, 10000.0, 10.0, 14.4);
    voltage = constrain(voltage, 10.0, 14.4);

    Serial.print("Voltage: ");
    Serial.print(voltage, 3);
    Serial.println(" V");

    dataReady = false; // Wait for next full set
  }
}

// === Request voltage from Nano ===
void requestVoltageFromNano() {
  digitalWrite(requestPin, HIGH);
  delayMicroseconds(100); // Short pulse for optocoupler
  digitalWrite(requestPin, LOW);
  Serial.println(" signal sent");
}

// === Interrupt: measures pulse period ===
void pulseISR() {
  unsigned long now = micros();
  if (lastRiseTime != 0) {
    pulsePeriod = now - lastRiseTime;
    pulseCount++;
    if (pulseCount >= 2) {
      dataReady = true;
    }
  }
  lastRiseTime = now;
}

// === Float-based map function ===
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
