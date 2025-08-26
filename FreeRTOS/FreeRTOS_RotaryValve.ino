#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <Stepper.h>

// --- Stepper Motor Setup ---
const int steps_per_rev = 1520;
Stepper motor(steps_per_rev, 11, 10, 9, 6); // IN1=11, IN2=10, IN3=9, IN4=7

// --- Hall Sensor & LED ---
const int hallPin = 3;
const int ledPin = 13;

// --- Timing Delays (ms) ---
const int LONG_DELAY = 3200;
const int SHORT_DELAY = 140;

// --- Max Steps for Homing Safety ---
const int MAX_HOMING_STEPS = 2000;

// --- Task Handle ---
TaskHandle_t StepperHandle = NULL;

// --- Homing Function ---
void homefunction() {
  int steps = 0;
  Serial.println("Homing...");

  while (digitalRead(hallPin) == HIGH) {
    motor.step(1);
    vTaskDelay(pdMS_TO_TICKS(1)); // yield
    steps++;

    if (steps >= MAX_HOMING_STEPS) {
      Serial.println("ERROR: Homing failed! Sensor not triggered.");
      Serial.print("Steps taken: "); Serial.println(steps);
      Serial.println("Check: Hall sensor, magnet, or mechanical blockage.");

      // Fatal error: blink LED rapidly forever
      while (true) {
        digitalWrite(ledPin, !digitalRead(ledPin));
        vTaskDelay(pdMS_TO_TICKS(200));
      }
    }
  }

  Serial.print("Homing successful in ");
  Serial.print(steps);
  Serial.println(" steps.");
}

// --- FreeRTOS Task: Mimics your original MotorControl() ---
void TaskStepper(void *pvParameters) {
  (void) pvParameters;

  // Initial homing (optional: ensures starting from known position)
  Serial.println("Initial homing...");
  homefunction();
  vTaskDelay(pdMS_TO_TICKS(140)); // match final delay

  for (;;) {
    // === Start of MotorControl() mimic ===
    motor.step(380); // 90°
    vTaskDelay(pdMS_TO_TICKS(3200));

    motor.step(380); // 90°
    vTaskDelay(pdMS_TO_TICKS(140));

    motor.step(380); // 90°
    vTaskDelay(pdMS_TO_TICKS(3200));

    homefunction(); // return to home (final 90° + sensor alignment)
    vTaskDelay(pdMS_TO_TICKS(140));
    // === End of mimic ===
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for Serial Monitor (useful on some boards)

  motor.setSpeed(18); // RPM

  pinMode(hallPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println("Rotary Valve FreeRTOS - Starting...");

  // Create the stepper task
  xTaskCreate(
    TaskStepper,
    "Stepper",
    256,
    NULL,
    1,
    &StepperHandle
  );

  // Start the FreeRTOS scheduler
  vTaskStartScheduler();
}

void loop() {
  // This will never run — FreeRTOS takes over
}