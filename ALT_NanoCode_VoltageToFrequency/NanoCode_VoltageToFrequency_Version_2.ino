// NanoCode_VoltageToFrequency Version 2
/* 
 --> Used Multiplier instead of the whole Voltage divider formula
 --> Changed frequency range from 500Hz - 10000Hz to 500Hz - 4900Hz
 
*/

// === Voltage Divider Constants ===
const float VDM = 3.0; // Voltage Divider Multiplier
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
    int freq = mapFloat(voltage, 10.0, 14.4, 500, 4900);
    freq = constrain(freq, 500, 4900);

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
  float vin = vOut * VDM;           // Reconstructed Vin
  return vin;
}

// === Like map() but for float inputs and int output ===
int mapFloat(float x, float in_min, float in_max, int out_min, int out_max) {
  return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}