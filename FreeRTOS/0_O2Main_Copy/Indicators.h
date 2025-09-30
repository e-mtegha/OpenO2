// with Switching Logic
#ifndef INDICATORS_H
#define INDICATORS_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

// ===== Extern variables (defined in main.ino) =====
extern float currentPurity;
extern float flowRate;

// ================= LED Pins =================
extern int normalLedPin;
extern int lowLedPin;
extern int serviceRequired;
extern int alternatorPower;
extern int RGBred;
extern int RGBgreen;

// ================= Voltage Settings =================
extern float batteryVoltage;
extern float alternatorVoltage;
extern int batterySwitch;
extern int alternatorSwitch;
extern int alternatorSwitch2;


// ================= Task: Indicators =================
inline void taskIndicators(void *pv) {
  pinMode(normalLedPin, OUTPUT);
  pinMode(lowLedPin, OUTPUT);
  pinMode(RGBred, OUTPUT);
  pinMode(RGBgreen, OUTPUT);
  pinMode(serviceRequired, OUTPUT);
  pinMode(alternatorPower, OUTPUT);
  pinMode(alternatorSwitch, OUTPUT);
  pinMode(alternatorSwitch2, OUTPUT);
  pinMode(batterySwitch, OUTPUT);


  for (;;) {
    digitalWrite(alternatorSwitch2, LOW);
    if (alternatorVoltage > 12.3) {
      digitalWrite(batterySwitch, LOW);
      digitalWrite(alternatorSwitch, HIGH);
      // digitalWrite(alternatorSwitch2, HIGH);
      digitalWrite(alternatorPower, HIGH);
      digitalWrite(RGBgreen, 0);
      digitalWrite(RGBred, 0);

    } else {
      if (batteryVoltage > 12.6) {
        digitalWrite(alternatorSwitch, LOW);
        // digitalWrite(alternatorSwitch2, LOW);
        digitalWrite(batterySwitch, HIGH);
        digitalWrite(alternatorPower, LOW);
        digitalWrite(RGBred, 0);
        digitalWrite(RGBgreen, 1);
      } else {
        digitalWrite(alternatorSwitch, LOW);
        digitalWrite(batterySwitch, HIGH);
        digitalWrite(alternatorPower, LOW);
        digitalWrite(RGBgreen, 0);
        digitalWrite(RGBred, 1);
      }
    }
    if (currentPurity >= 82.0) {
      digitalWrite(normalLedPin, HIGH);
      digitalWrite(lowLedPin, LOW);
    } else {
      digitalWrite(normalLedPin, LOW);
      digitalWrite(lowLedPin, HIGH);
    }
    if (currentPurity <= 60.0 || flowRate == 0.0) {
      digitalWrite(serviceRequired, HIGH);
    } else {
      digitalWrite(serviceRequired, LOW);
    }

    vTaskDelay(pdMS_TO_TICKS(500));  // check every 500 ms
  }
}
#endif
