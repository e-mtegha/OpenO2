#ifndef STEPPER_TASK_H
#define STEPPER_TASK_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <Stepper.h>

// ===== Extern variables (defined in main.ino) =====
extern Stepper motor;
extern const int hallPin;
// extern const int ledPin;
extern const int MAX_HOMING_STEPS;
extern const int SHORT_DELAY;
extern const int LONG_DELAY;
// extern  int PWMPin;

// ================= Homing =================
inline void homefunction() {
  int steps = 0;
  while (digitalRead(hallPin) == HIGH) {
    motor.step(1);
    steps++;
    // if (steps >= MAX_HOMING_STEPS) {
    //   // Error condition: failed to home
    //   // while (true) {
    //   //   digitalWrite(ledPin, !digitalRead(ledPin));
    //   //   vTaskDelay(pdMS_TO_TICKS(200));
    //   // }
    // }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

// ================= Task: Stepper =================
inline void taskStepper(void *pv) {
  motor.setSpeed(20);
  homefunction();
  vTaskDelay(pdMS_TO_TICKS(SHORT_DELAY));

  for (;;) {
    motor.step(380);
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));
    motor.step(380);
    vTaskDelay(pdMS_TO_TICKS(SHORT_DELAY));
    motor.step(380);
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));
    homefunction();
    vTaskDelay(pdMS_TO_TICKS(SHORT_DELAY));
  }

 

}

// inline void taskControlVoltage(void *pv){
//     pinMode(PWMPin, OUTPUT);
//   // Fast PWM, TOP=255 (mode 3)
//   TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
//   TCCR2B = _BV(CS20);   // no prescaler

//   // Frequency = 16 MHz / (256 * 1) = 62.5 kHz
//   // To get ~31.25 kHz, we need phase-correct PWM instead of fast PWM:
//   // → Frequency = 16 MHz / (510 * 1) ≈ 31.37 kHz
//    for (;;){
//   // Duty cycle control (0–255)
//   OCR2A = 64;    // ~25%
//   delay(2000);
//   OCR2A = 128;   // ~50%
//   delay(2000);
//     OCR2A = 192;   // ~50%
//   delay(2000);
//   OCR2A = 255;   // ~100%
//   delay(2000);
//   OCR2A = 0;     // 0%
//   delay(2000);
//   }

// }


#endif
