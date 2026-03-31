/*
# Arduino Nano – Voltage Measurement & Frequency Response for UNO(Or other Microcontroller)

## Overview
This Arduino Nano sketch measures a DC voltage using a resistor divider, then outputs a square wave whose frequency corresponds to that voltage.  
The UNO requests a reading via an optocoupler pulse, triggering the Nano to send the encoded frequency.

---

## Features
- Measures DC voltage through an analog pin and a voltage divider.
- Maps the measured voltage to a frequency (500 Hz – 10 kHz).
- Uses an interrupt to respond instantly when the UNO requests data.
- Sends frequency via `tone()` function for the UNO to measure.
- Includes live serial monitoring of measured voltage.

---

## Pin Assignments
- analogPin (A0): Measures voltage via voltage divider.
- interruptPin (2): Input from UNO (via optocoupler) to trigger voltage send.
- tonePin (9): Output to UNO (via optocoupler) carrying square wave.

---

## Measurement Process
1. Continuously measures voltage for live monitoring (once per second).
2. When the UNO sends a short HIGH pulse to `interruptPin` the nano does the following:
   - Reads the current voltage.
   - Maps it to a frequency between 500 Hz and 10 kHz.
   - Sends the tone for 100 ms, then stops.
3. Voltage reconstruction formula:
V_in = V_out × ( (R1 + R2) / R2 )
Where `V_out` is measured at A0, `R1` is the top resistor, and `R2` is the bottom resistor.

---

## Voltage-to-Frequency Mapping
The formula used:  
Frequency = mapFloat(voltage, 10.0 V, 14.4 V, 500 Hz, 10000 Hz)
- Voltages below 10.0 V map to 500 Hz.
- Voltages above 14.4 V map to 10 kHz.
- Output is constrained within 500–10,000 Hz.

---
*/
// === Voltage Divider Constants ===
const float R1 = 2; // Top resistor
const float R2 = 1;  // Bottom resistor
const int analogPin = A0;

// === Communication Pins ===
const int interruptPin = 2;  // Connected to optocoupler 1 (from Uno)
const int tonePin = 9;      // Output to optocoupler 2 (to Uno)

// === Control ===
volatile bool sendVoltageFlag = false;  // Set by ISR

void setup() {
  Serial.begin(9600);
  delay(1000); // Let serial monitor sync

  Serial.println("NANO: Starting voltage sender system...");

  pinMode(analogPin, INPUT);
  pinMode(interruptPin, INPUT);  // Use external pull-up resistor
  pinMode(tonePin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(interruptPin), requestHandler, FALLING);

  Serial.println("NANO: Interrupt attached, system ready.");
}

void loop() {
  // Optional: Print live voltage (for monitoring)
  float vinLive = readVoltage();
  Serial.print("NANO: Live Voltage = ");
  Serial.print(vinLive, 3);
  Serial.println(" V");

  delay(1000); // Adjust as needed

  // Check if Uno requested a voltage
  if (sendVoltageFlag) {
    sendVoltageFlag = false;

    Serial.println("NANO: Interrupt received — sending voltage...");

    // 1. Read voltage
    float voltage = readVoltage();
    Serial.print("NANO: Read voltage = ");
    Serial.print(voltage, 3);
    Serial.println(" V");

    // 2. Map to frequency
    int freq = mapFloat(voltage, 10.0, 14.4, 500, 10000);
    freq = constrain(freq, 500, 10000);

    Serial.print("NANO: Mapped frequency = ");
    Serial.print(freq);
    Serial.println(" Hz");

    // 3. Send tone
    tone(tonePin, freq);
    Serial.println("NANO: Tone started.");

    delay(100);  // Let Uno read the frequency

    noTone(tonePin);
    Serial.println("NANO: Tone stopped.");
  }
}

// === ISR: Triggered by Uno's pulse on D2 ===
void requestHandler() {
  sendVoltageFlag = true;
}

// === Reads voltage from analog pin ===
float readVoltage() {
  const float vcc = 4.86;  // Replace with actual measured Vcc if needed
  int adcValue = analogRead(analogPin);
  float vOut = adcValue * (vcc / 1023.0);        // Voltage at A0
  float vin = vOut * ((R1 + R2) / R2);           // Reconstructed Vin
  return vin;
}

// === Like map() but for float inputs and int output ===
int mapFloat(float x, float in_min, float in_max, int out_min, int out_max) {
  return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}
