// Voltage divider values
const float R1 = 2; // Top resistor (connected to Vin)
const float R2 = 1; // Bottom resistor (connected to GND)
const int analogPin = A0;

void setup() {

  Serial.begin(9600);
}

void loop() {
  //Step 1: Read actual Vcc
  long vccMillivolts = readVcc();       // returns Vcc in millivolts
  float vcc = vccMillivolts / 1000.0;   // convert to volts
  // float vcc = 5.01;

  // Step 2: Read analog value from A0
  int adcValue = analogRead(analogPin);

  // Step 3: Calculate voltage at A0
  float vOut = adcValue * (vcc / 1023.0);  // in volts

  // Step 4: Reconstruct original Vin from divider
  float vin = vOut * ((R1 + R2) / R2);     // based on divider formula

  // Step 5: Print all values
  Serial.print("Vcc: ");
  Serial.print(vcc, 3);
  Serial.print(" V\tADC: ");
  Serial.print(adcValue);
  Serial.print("\tA0 Voltage: ");
  Serial.print(vOut, 3);
  Serial.print(" V\tEstimated Vin: ");
  Serial.print(vin, 3);
  Serial.println(" V");

  delay(1000);
}

// Function to read actual Vcc using internal 1.1V reference
long readVcc() {
  // Configure ADC to read internal 1.1V reference
  ADMUX = _BV(REFS0)                // Use Vcc as reference
        | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // Select 1.1V as input
  delay(2);                         // Let voltage settle
  ADCSRA |= _BV(ADSC);              // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // Wait for conversion to finish
  uint16_t result = ADC;
  long vcc = 1125300L / result;     // Calculate Vcc in millivolts
  return vcc;
}
