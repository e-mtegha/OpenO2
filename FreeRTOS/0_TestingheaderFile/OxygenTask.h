#ifndef OXYGEN_TASK_H
#define OXYGEN_TASK_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

// Declare globals so they can be shared
extern float currentPurity;
extern float flowRate;  extern int PWMPin;

// Define the task inline in the header
inline void taskOxygen(void *pv) {
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
        // flowRate = (u * 256.0 + l) / 10.0;
        flowRate = round((u * 256.0 + l) / 10.0 * 10.0 )/10.0;

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

#endif
