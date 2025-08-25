/*
# Overview
This Arduino sketch measures an external voltage using a voltage divider and automatically compensates for variations in the Arduino’s own supply voltage (`Vcc`).  
It does this by:
1. Measuring `Vcc` precisely using the internal 1.1V reference.
2. Reading the analog input from a voltage divider.
3. Reconstructing the original external voltage based on the divider ratio.

## Hardware Connections
- Voltage Divider: 
  - `R1` (top resistor) connected from Vin (voltage to measure) → A0.
  - `R2` (bottom resistor) connected from A0 → GND.
- Analog Pin: 
  - `A0` reads the divided voltage.
- This allows measurement of voltages higher than `Vcc` (up to the safe limit of A0) by scaling them down.

## Voltage Measurement Method
1. Read Vcc:
   Uses the ATmega’s internal 1.1V reference to measure the actual board voltage supply.  
   This improves accuracy when USB power or regulator voltage fluctuates.
2. Read Analog Value:  
   Uses `analogRead(A0)` to capture the divided voltage.
3. Reconstruct Vin:
   Applies the voltage divider formula:  
   
   V{in} = V{out} * ({R1 + R2}{R2})
   
4. Print Data:
   Displays `Vcc`, raw ADC value, measured `Vout`, and reconstructed `Vin`.

## Notes
- This method is much more accurate than assuming `Vcc = 5.00V`.
- The formula in `readVcc()` (`1125300L / result`) is based on the calibrated internal reference of 1.1V.
- Ensure `R1` and `R2` are chosen to keep `Vout` < `Vcc` at all times.

---

# Code
*/
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
/*
Explanation of the readVcc () Function

long readVcc() {
Defines a function named readVcc that returns a long integer representing the supply voltage in millivolts.

    // Configure ADC to read internal 1.1V reference
  ADMUX = _BV(REFS0)                // Use Vcc as reference
        | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // Select 1.1V as input
ADMUX is the ADC Multiplexer Selection Register, which controls what the ADC measures and its reference voltage.
_BV(bit) is a macro that means “bit value,” or (1 << bit).
_BV(REFS0) sets the ADC reference voltage to Vcc (the Arduino’s supply voltage).
_BV(MUX3) | _BV(MUX2) | _BV(MUX1) selects the internal 1.1V reference as the input channel for the ADC instead of an external pin.
Together, this configures the ADC to measure the internal 1.1V reference voltage relative to Vcc as the ADC reference.

  delay(2);                         // Let voltage settle
Waits 2 milliseconds to allow the ADC input voltage to stabilize before starting conversion.

  ADCSRA |= _BV(ADSC);              // Start conversion
ADCSRA is the ADC Control and Status Register A.
Setting the ADSC bit starts an Analog-to-Digital Conversion.

    while (bit_is_set(ADCSRA, ADSC)); // Wait for conversion to finish
Loops continuously while the ADSC bit remains set, meaning the ADC is busy converting.
When the conversion is complete, ADSC bit clears, and the loop ends.

    uint16_t result = ADC;
Reads the 10-bit ADC conversion result from the ADC register into a 16-bit unsigned integer variable result.

  long vcc = 1125300L / result;     // Calculate Vcc in millivolts
Calculates the supply voltage vcc in millivolts.
1125300L is a constant calculated as: (1.1 V * 1023 * 1000)
1.1 V is the internal reference voltage in volts
1023 is the max ADC value (10-bit ADC)
1000 converts volts to millivolts
The formula is:
Vcc = (1.1 V * 1023 * 1000) / ADC reading
Because the ADC measured the internal 1.1V reference using Vcc as the ADC reference, the ADC result inversely relates to Vcc.
This formula derives the actual supply voltage Vcc from the ADC reading.

  return vcc;
}
Returns the calculated supply voltage in millivolts to wherever the function was called.

Summary:
This function cleverly uses the Arduino's internal 1.1V reference to measure how much voltage the Arduino is actually powered by (Vcc), without needing an external voltage sensor.
It configures the ADC to measure the internal reference voltage relative to Vcc, performs an ADC conversion, then calculates the supply voltage from the ADC value. 
The result is returned as a long integer in millivolts.
*/
