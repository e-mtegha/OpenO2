// === Voltage Divider Constants ===
const float R1 = 19790.0; // Top resistor
const float R2 = 9830.0;  // Bottom resistor
const int analogPin = A1;

// === Communication Pins ===
const int interruptPin = 2;  // From Uno (via optocoupler 1)
const int tonePin = 9;       // Output to Uno (via optocoupler 2) - must be OC1A

// === Control ===
volatile bool sendVoltageFlag = false;  // Set by ISR

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("NANO: Starting voltage sender system...");

  pinMode(analogPin, INPUT);
  pinMode(interruptPin, INPUT);  // Use external pull-up resistor
  pinMode(tonePin, OUTPUT);

  // attachInterrupt(digitalPinToInterrupt(interruptPin), requestHandler, FALLING);

  Serial.println("NANO: Interrupt attached, system ready.");
}

void loop() {
  // Live voltage print (optional)
  // float vinLive = readVoltage();
  // Serial.print("NANO: Live Voltage = ");
  // Serial.print(vinLive, 3);
  // Serial.println(" V");

  // delay(1000);

  // if (sendVoltageFlag) {
  //   sendVoltageFlag = false;

  //   Serial.println("NANO: Interrupt received — sending voltage...");

    // 1. Read voltage
    float voltage = readVoltage();
    // Serial.print("NANO: Read voltage = ");
    // Serial.print(voltage, 3);
    // Serial.println(" V");

    // 2. Map voltage to frequency
    int freq = mapFloat(voltage, 10.0, 14.4, 500, 4900);
    freq = constrain(freq, 500, 4900);

    // Serial.print("NANO: Mapped frequency = ");
    // Serial.print(freq);
    // Serial.println(" Hz");

    // 3. Send frequency pulse using Timer1
    sendFrequency(tonePin, freq);  // send 20 cycles

    Serial.println(freq);
  // }
}

// === ISR: triggered by Uno request ===
// void requestHandler() {
//   sendVoltageFlag = true;
// }

// === Voltage Reading Function ===
float readVoltage() {
  const float vcc = 5.01;
  float sum = 0.0;
  for (int i = 0; i < 20; i++) {
    sum += analogRead(analogPin);
    delay(1);
  }
  // --- Step 3: Calculate Vout at A0 ---
  float vOut = (sum / 20.0) * (5.01 / 1023.0);
  float vin = vOut * ((R1 + R2) / R2);
  return vin;
}

// === Frequency Sender using Timer1 ===
void sendFrequency(int pin,int frequency) {
  tone(pin, frequency);
}

// === Float to Int Mapping ===
int mapFloat(float x, float in_min, float in_max, int out_min, int out_max) {
  return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}
