/*
  Arduino Nano: Measure external voltage with divider,
  display Vin and calibrated Vcc on 16x2 I2C LCD.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Voltage divider resistors
const float R1 = 30000.0;   // Top resistor (ohms)
const float R2 = 10000.0;   // Bottom resistor (ohms)
const int analogPin = A0;

// LCD on I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Calibration ===
// Nano printed 3.22V, measured was 3.16V
// => Calibration factor = 3.16/3.22 = ~0.981
const float VCC_CAL = 1.033123028391167;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Labels (printed only once to reduce flicker)
  lcd.setCursor(0, 0); lcd.print("Vin:");
  lcd.setCursor(0, 1); lcd.print("Vcc:");
}

void loop() {
  // --- Step 1: Measure Vcc ---
  long vccMillivolts = readVcc();
  float vcc = (vccMillivolts / 1000.0) * VCC_CAL;

  // --- Step 2: Average ADC readings on A0 ---
  float sum = 0.0;
  for (int i = 0; i < 20; i++) {
    sum += analogRead(analogPin);
    delay(1);
  }

  // --- Step 3: Calculate Vout at A0 ---
  float vOut = (sum / 20.0) * (5.01 / 1023.0);

  // --- Step 4: Reconstruct Vin ---
  float vin = vOut * ((R1 + R2) / R2);

  // --- Step 5: Display results ---
  lcd.setCursor(5, 0);
  lcd.print("       ");            // Clear old Vin
  lcd.setCursor(5, 0);
  lcd.print(vin, 2); lcd.print("V");

  lcd.setCursor(5, 1);
  lcd.print("       ");            // Clear old Vcc
  lcd.setCursor(5, 1);
  lcd.print(vcc, 2); lcd.print("V");

  delay(500); // update every 0.5s
}

// === Function: Read actual Vcc ===
// Works on ATmega328P (Nano/Uno)
long readVcc() {
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // measure 1.1V bandgap
  delay(2);
  ADCSRA |= _BV(ADSC); // start conversion
  while (bit_is_set(ADCSRA, ADSC));
  uint16_t result = ADC;
  long vcc = 1125300L / result; // in millivolts
  return vcc;
}
