#include <Arduino_FreeRTOS.h>

// --- Sensor readings ---
float flowRate = 0;
float currentPurity = 0;

// Task handles (optional, useful for debugging)
TaskHandle_t OxygenFlowHandle = NULL;
TaskHandle_t LoggerHandle = NULL;

// --- Read oxygen purity & flow rate from sensor ---
void OxygenFlowTask(void *pvParameters) {
  byte incomingByte, LowerByte, UpperByte;
  float oxygen1, oxygen2, flow1, flow2;

  for (;;) {  // <- FreeRTOS task loop
    if (Serial.available()) {
      incomingByte = Serial.read();
      if (incomingByte == 0x16) {
        // Skip 2 bytes
        while (!Serial.available());
        Serial.read();
        while (!Serial.available());
        Serial.read();

        // Get purity
        while (!Serial.available());
        UpperByte = Serial.read();
        while (!Serial.available());
        LowerByte = Serial.read();
        oxygen1 = (UpperByte * 256);
        oxygen2 = LowerByte;
        currentPurity = (oxygen1 + oxygen2) / 10.0;

        // Get flow rate
        while (!Serial.available());
        UpperByte = Serial.read();
        while (!Serial.available());
        LowerByte = Serial.read();
        flow1 = UpperByte * 256;
        flow2 = LowerByte;
        flowRate = (flow1 + flow2) / 10.0;

        // Discard remaining 5 bytes
        for (int i = 0; i < 5; i++) {
          while (!Serial.available());
          Serial.read();
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50));  // sample every 50 ms
  }
}

// --- Logging task ---
void LoggerTask(void *pvParameters) {
  for (;;) {
    Serial.print("Oxygen Concentration: ");
    Serial.print(currentPurity);
    Serial.print(" %   |   Flow Rate: ");
    Serial.print(flowRate);
    Serial.println(" LPM");

    vTaskDelay(pdMS_TO_TICKS(1000));  // log every 1 second
  }
}

void setup() {
  Serial.begin(9600, SERIAL_8N1);

  // Create tasks
  xTaskCreate(OxygenFlowTask, "O2Sensor", 256, NULL, 2, &OxygenFlowHandle);
  xTaskCreate(LoggerTask, "Logger", 256, NULL, 1, &LoggerHandle);

  // FreeRTOS starts automatically after setup()
}

void loop() {
  // Nothing here, FreeRTOS takes over
}
