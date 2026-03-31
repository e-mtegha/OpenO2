#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// ================= Shared Variables =================
float g_voltage = 0.0;
float flowRate = 0.0;
float currentPurity = 0.0;

SemaphoreHandle_t dataMutex;   // for g_voltage only

// ================= Timer1 Input Capture (voltage) =================
const uint8_t MEDIAN_SIZE = 5;
const float ALPHA = 0.1;

volatile uint16_t periodBuffer[MEDIAN_SIZE];
volatile uint8_t bufferIndex = 0;
volatile bool dataReady = false;
volatile uint16_t lastCapture = 0;
float EMA_voltage = 0.0;

// ================= FreeRTOS Tasks =================

// Voltage processing task
void taskVoltage(void *pv) {
  for (;;) {
    if (dataReady) {
      dataReady = false;
      uint16_t medianPeriod = medianFilter((uint16_t*)periodBuffer);
      float voltage = computeEMA(ticksToVoltage(medianPeriod));

      xSemaphoreTake(dataMutex, portMAX_DELAY);
      g_voltage = voltage;
      xSemaphoreGive(dataMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // run every 50 ms
  }
}

// Oxygen sensor reading task
void taskOxygen(void *pvParameters) {
  byte incomingByte, LowerByte, UpperByte;
  float oxygen1, oxygen2, flow1, flow2;

  for (;;) {
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

// Logger task (prints everything every 1 sec)
void taskLogger(void *pv) {
  for (;;) {
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    float localVoltage = g_voltage;
    xSemaphoreGive(dataMutex);

    Serial.print("Voltage: ");
    Serial.print(localVoltage, 2);
    Serial.print(" V   |   Oxygen Concentration: ");
    Serial.print(currentPurity);
    Serial.print(" %   |   Flow Rate: ");
    Serial.print(flowRate);
    Serial.println(" LPM");

    // vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// ================= Setup =================
void setup() {
  Serial.begin(9600, SERIAL_8N1);
  dataMutex = xSemaphoreCreateMutex();

  setupTimer1();

  // Create tasks
  xTaskCreate(taskVoltage, "Volt", 256, NULL, 3, NULL);
  xTaskCreate(taskOxygen, "O2Sensor", 256, NULL, 2, NULL);
  xTaskCreate(taskLogger, "Log", 256, NULL, 1, NULL);

  vTaskStartScheduler();
}

// ================= Timer1 Setup =================
void setupTimer1() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << ICES1);   // rising edge
  TCCR1B |= (1 << CS11);    // prescaler 8 → 0.5 µs per tick
  TIMSK1 |= (1 << ICIE1);   // enable input capture interrupt
  interrupts();
}

// ================= Timer1 ISR =================
ISR(TIMER1_CAPT_vect) {
  uint16_t capture = ICR1;
  uint16_t period = capture - lastCapture;
  lastCapture = capture;

  periodBuffer[bufferIndex++] = period;
  if (bufferIndex >= MEDIAN_SIZE) {
    dataReady = true;
    bufferIndex = 0;
  }
}

// ================= Branchless Median Filter =================
uint16_t medianFilter(uint16_t buffer[]) {
  uint16_t a[5];
  memcpy(a, buffer, 5 * sizeof(uint16_t));
  #define MINMAX(i,j) { uint16_t x=a[i],y=a[j]; a[i]=(x<y)?x:y; a[j]=(x>y)?x:y; }
  MINMAX(0,1); MINMAX(3,4);
  MINMAX(2,4); MINMAX(2,3);
  MINMAX(1,4); MINMAX(0,3);
  MINMAX(0,2); MINMAX(1,3);
  MINMAX(1,2);
  return a[2];
}

// ================= EMA & Voltage Conversion =================
float computeEMA(float input) {
  EMA_voltage = ALPHA * input + (1 - ALPHA) * EMA_voltage;
  return EMA_voltage;
}

float ticksToVoltage(uint16_t ticks) {
  float period_us = ticks * 0.5;
  float freq = 1000000.0 / period_us;
  float voltage = mapFloat(freq, 500.0, 4900.0, 10.0, 14.4);
  return constrain(voltage, 10.0, 14.4);
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop() {
  // Empty because FreeRTOS tasks are running
}
