#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <Stepper.h>

// --- Stepper Motor Setup ---
const int steps_per_rev = 1520;
Stepper motor(steps_per_rev, 11, 10, 9, 7); // IN1=11, IN2=10, IN3=9, IN4=7

// --- Timing Delays (ms) ---
const int LONG_DELAY = 3200;
const int SHORT_DELAY = 140;

// --- Task Handle ---
TaskHandle_t StepperHandle = NULL;

// --- FreeRTOS Task: Pure step + delay (no homing) ---
void TaskStepper(void *pvParameters) {
  (void) pvParameters;

  Serial.println("Stepper task started - NO HOMING (oscilloscope test mode)");

  for (;;) {
    // === Mimic your original MotorControl() ===
    motor.step(380); // 90°
    Serial.println("Moved +90°");
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));

    motor.step(380); // 90°
    Serial.println("Moved +90°");
    vTaskDelay(pdMS_TO_TICKS(SHORT_DELAY));

    motor.step(380); // 90°
    Serial.println("Moved +90°");
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));

    motor.step(380); // 90° back to home (instead of homing)
    Serial.println("Returned to home (open-loop)");
    vTaskDelay(pdMS_TO_TICKS(SHORT_DELAY));
    // ==========================================
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for Serial Monitor (if needed)

  motor.setSpeed(18); // 18 RPM

  // No need to set up hallPin or LED anymore (not used)

  Serial.println("Stepper Oscilloscope Test - Starting...");

  // Create the stepper task
  xTaskCreate(
    TaskStepper,
    "Stepper",
    256,
    NULL,
    1,
    &StepperHandle
  );

  // Start FreeRTOS scheduler
  vTaskStartScheduler();
}

void loop() {
  // Never reached — FreeRTOS takes over
}