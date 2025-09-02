// with Switching Logic
#ifndef INDICATORS_H
#define INDICATORS_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

// ===== Extern variables (defined in main.ino) =====
extern float currentPurity;

// ================= LED Pins =================
extern int normalLedPin;
extern int lowLedPin;
extern int red;
extern int green;

// ================= Voltage Settings =================
extern float batteryVoltage;
extern float voltage;
extern int batterySwitch;
extern int alternatorSwitch;


// ================= Task: Indicators =================
inline void taskIndicators(void *pv) {
  pinMode(normalLedPin, OUTPUT);
  pinMode(lowLedPin, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);


  for (;;) {
    if (voltage > 12.3) {
      digitalWrite(batterySwitch, LOW);
      digitalWrite(alternatorSwitch, HIGH);

    } else {
      if (batteryVoltage > 12.6) {
        digitalWrite(alternatorSwitch, LOW);
        digitalWrite(batterySwitch, HIGH);
        analogWrite(red, 0);
        analogWrite(green, 255);
      } else {
        digitalWrite(alternatorSwitch, LOW);
        digitalWrite(batterySwitch, HIGH);
        analogWrite(green, 0);
        analogWrite(red, 255);
      }
    }
    if (currentPurity >= 82.0) {
      digitalWrite(normalLedPin, HIGH);
      digitalWrite(lowLedPin, LOW);
    } else {
      digitalWrite(normalLedPin, LOW);
      digitalWrite(lowLedPin, HIGH);
    }

    vTaskDelay(pdMS_TO_TICKS(500));  // check every 500 ms
  }
}
#endif
