// Simple oxygen purity and flow rate reader
// Non-FreeRTOS version for troubleshooting

// Variables
float currentPurity = 0.0;
float rawFlowRate = 0.0;
float flowRate = 0.0;

// Exponential moving average constant (smoothing factor)
const float ALPHA = 0.3;  // Adjust between 0 (very smooth) and 1 (very responsive)

void setup() {
  Serial.begin(9600);  // Change to match your sensor's baud rate
  Serial.println("Oxygen sensor test started...");
}

void loop() {
  if (Serial.available()) {
    byte incomingByte = Serial.read();

    if (incomingByte == 0x16) {
      // Skip 2 bytes
      for (int i = 0; i < 2; i++) {
        waitForData();
        Serial.read();
      }

      // ---- Purity ----
      waitForData();
      byte u = Serial.read();
      waitForData();
      byte l = Serial.read();
      currentPurity = (u * 256.0 + l) / 10.0;

      // ---- Flow ----
      waitForData();
      u = Serial.read();
      waitForData();
      l = Serial.read();
      rawFlowRate = round((u * 256.0 + l) / 10.0 * 10.0) / 10.0;

      // Exponential Moving Average
      flowRate = ALPHA * rawFlowRate + (1 - ALPHA) * flowRate;

      // Skip 5 bytes
      for (int i = 0; i < 5; i++) {
        waitForData();
        Serial.read();
      }

      // ---- Display results ----
      Serial.print("Purity: ");
      Serial.print(currentPurity);
      Serial.print("% | Flow Rate: ");
      Serial.print(flowRate);
      Serial.println(" L/min");
    }
  }

  delay(50);  // Short pause between reads
}

// Wait until at least 1 byte is available
void waitForData() {
  while (!Serial.available()) {
    delay(1);
  }
}
