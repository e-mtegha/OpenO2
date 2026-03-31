//This code Moves the rotary & reads flow and purity from the oxygen sensor
#include <Arduino_FreeRTOS.h>
#include <Stepper.h>

// ================= Shared Variables =================
float g_voltage = 12.0;
float flowRate = 10.0;
float currentPurity = 95.0;

// ================= Stepper Motor Setup =================
const int steps_per_rev = 1520;
Stepper motor(steps_per_rev, 11, 10, 9, 6);

const int hallPin = 3;
const int ledPin = 13;

const int LONG_DELAY = 3200;
const int SHORT_DELAY = 140;
const int MAX_HOMING_STEPS = 2000;

// ================= Task Prototypes =================
void taskOxygen(void *pv);
void taskLogger(void *pv);
void taskStepper(void *pv);

// ================= Homing =================
void homefunction() {
  int steps = 0;
  while (digitalRead(hallPin) == HIGH) {
    motor.step(1);
    steps++;
    if (steps >= MAX_HOMING_STEPS) {
      while (true) {
        digitalWrite(ledPin, !digitalRead(ledPin));
        vTaskDelay(pdMS_TO_TICKS(200));
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

// ================= Task: Stepper =================
void taskStepper(void *pv) {
  motor.setSpeed(20);
  homefunction();
  vTaskDelay(pdMS_TO_TICKS(SHORT_DELAY));

  for (;;) {
    motor.step(380); vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));
    motor.step(380); vTaskDelay(pdMS_TO_TICKS(SHORT_DELAY));
    motor.step(380); vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));
    homefunction(); vTaskDelay(pdMS_TO_TICKS(SHORT_DELAY));
  }
}

// ================= Task: Oxygen (Minimal) =================
void taskOxygen(void *pv) {
  for (;;) {
    if (Serial.available()) {
      byte incomingByte = Serial.read();
      if (incomingByte == 0x16) {
        // Skip 2 bytes
        for (int i = 0; i < 2; i++) {
          while (!Serial.available()) vTaskDelay(1);
          Serial.read();
        }

        // Purity
        while (!Serial.available()) vTaskDelay(1);
        byte u = Serial.read();
        while (!Serial.available()) vTaskDelay(1);
        byte l = Serial.read();
        currentPurity = (u * 256.0 + l) / 10.0;

        // Flow
        while (!Serial.available()) vTaskDelay(1);
        u = Serial.read();
        while (!Serial.available()) vTaskDelay(1);
        l = Serial.read();
        flowRate = (u * 256.0 + l) / 10.0;

        // Skip 5
        for (int i = 0; i < 5; i++) {
          while (!Serial.available()) vTaskDelay(1);
          Serial.read();
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// ================= Task: Logger =================
void taskLogger(void *pv) {
  for (;;) {
    Serial.print("V:"); Serial.print(g_voltage, 2);
    Serial.print("V O2:"); Serial.print(currentPurity);
    Serial.print("% F:"); Serial.print(flowRate);
    Serial.println("LPM");

    // vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// ================= Setup =================
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(100);
  }

  Serial.println("✅ System Starting...");

  motor.setSpeed(18);
  pinMode(hallPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // ✅ Reduced stack sizes
  xTaskCreate(taskOxygen,  "O2",   240, NULL, 2, NULL);
  xTaskCreate(taskLogger,  "Log",  224, NULL, 1, NULL);
  xTaskCreate(taskStepper, "Step", 200, NULL, 2, NULL);

  Serial.println("✅ FreeRTOS: Starting scheduler...");
  vTaskStartScheduler();

  Serial.println("❌ FATAL: Out of memory!");
  while (true) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(200);
  }
}

void loop() {}
