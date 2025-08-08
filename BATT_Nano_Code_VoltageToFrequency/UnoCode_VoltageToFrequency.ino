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
