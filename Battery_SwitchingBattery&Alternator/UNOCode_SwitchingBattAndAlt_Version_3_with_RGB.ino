// UNOCode_SwitchingBattAndAlt_Version_3_with_RGB

/*
 --> Multiplier instead of full voltage divider formula
 --> Frequency range from 500Hz - 4900Hz
 --> Added battery voltage measurement
 --> Added fast failover to battery if no alternator data is received
*/

// === Pins ===
const int requestPin = 7;     // Output to request alternator data
const int inputPin = 2;       // Interrupt input for alternator signal
const int batterySwitchPin = 5;
const int alternatorSwitchPin = 4;

// === Voltage Divider for Battery ===
const int analogPin = A0;
const float VDM = 3.0; // Voltage Divider Multiplier
const float VCC = 5.02;

//Defining  variable and the GPIO pin on Arduino
int redPin= 8;
int greenPin = 9;

// === Pulse measurement ===
volatile unsigned long lastRiseTime = 0;
volatile unsigned long pulsePeriods[5];
volatile int pulseIndex = 0;
volatile int pulseCount = 0;

unsigned long lastRequestTime = 0;
const unsigned long requestInterval = 100; // Request data every 200ms
const unsigned long noDataTimeout = 800;   // Timeout for no data (ms) - FAST SWITCH

unsigned long lastDataTime = 0;

bool dataReady = false;
bool isMeasuring = false;

void setup() {
  Serial.begin(9600);
  delay(1000);

    //Defining the RGB pins as OUTPUT
 pinMode(redPin,  OUTPUT);              
 pinMode(greenPin, OUTPUT);

  pinMode(requestPin, OUTPUT);
  digitalWrite(requestPin, LOW);

  pinMode(inputPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(inputPin), pulseISR, RISING);

  pinMode(batterySwitchPin, OUTPUT);
  pinMode(alternatorSwitchPin, OUTPUT);

  digitalWrite(batterySwitchPin, LOW);
  digitalWrite(alternatorSwitchPin, LOW);

  lastDataTime = millis();

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
    delay(20);  // Allow tone to stabilize
  }

  // When tone data is ready
  if (dataReady && isMeasuring) {
    isMeasuring = false;
    lastDataTime = millis(); // Update last data time

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
      float alternatorVoltage = mapFloat(freq, 500.0, 4900.0, 10.0, 14.4);
      alternatorVoltage = constrain(alternatorVoltage, 10.0, 14.4);

      // Serial.print("Alternator Voltage: ");
      // Serial.print(alternatorVoltage, 3);
      // Serial.println(" V");

      // --- Measure battery voltage ---
      int rawBattery = analogRead(analogPin);
      float batteryVoltage = rawBattery * (VCC / 1023.0) * VDM;

      // Serial.print("Battery Voltage: ");
      // Serial.print(batteryVoltage, 3);
      // Serial.println(" V");

      // --- Switching Logic ---
      if (alternatorVoltage > 12.3) {
        digitalWrite(alternatorSwitchPin, HIGH);
        digitalWrite(batterySwitchPin, LOW);
        // Serial.println("Switching to Alternator.");
        setColor(0,  255, 0); // Green Color
      } else if (batteryVoltage > 12.3) {
        digitalWrite(alternatorSwitchPin, LOW);
        digitalWrite(batterySwitchPin, HIGH);
        // Serial.println("Switching to Battery.");
        setColor(255, 0, 0); // Red Color
      } else {
        digitalWrite(alternatorSwitchPin, LOW);
        digitalWrite(batterySwitchPin, LOW);
        // Serial.println("Both sources below 12.3V. No switching.");
      }

    } else {
      // Serial.println("UNO: Invalid tone signal.");
    }

    dataReady = false;
  }

  // --- Failover check for missing alternator data ---
  if (millis() - lastDataTime > noDataTimeout) {
    int rawBattery = analogRead(analogPin);
    float batteryVoltage = rawBattery * (VCC / 1023.0) * VDM;

    // Serial.println("No alternator data - Timeout!");

    if (batteryVoltage > 12.3) {
      digitalWrite(alternatorSwitchPin, LOW);
      digitalWrite(batterySwitchPin, HIGH);
      // Serial.println("Switching to Battery (timeout).");
      setColor(255, 0, 0); // Red Color
    } else {
      digitalWrite(alternatorSwitchPin, LOW);
      digitalWrite(batterySwitchPin, LOW);
      // Serial.println("Both sources below 12.3V (timeout). No switching.");
    }

    lastDataTime = millis(); // Prevent repeating instantly
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

void setColor(int redValue, int greenValue,  int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin,  greenValue);
  }
