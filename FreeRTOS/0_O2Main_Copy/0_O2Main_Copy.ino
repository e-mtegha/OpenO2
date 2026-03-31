#include "OxygenTask.h"
#include "StepperTask.h"
#include "Indicators.h"
#include "VoltageSystem.h"

//Globals Definition (storage for the externs)
float currentPurity = 0.0;
float flowRate = 0.0;
float VCC = 5.06;  // Assume 5V reference for ADC
float batteryVoltage = 0.0;
float alternatorVoltage = 0.0;  // From frequency input
float EMA_voltage = 0.0;
float ALPHA = 0.1;

// Stepper Globals
const int MAX_HOMING_STEPS = 5000;
const int SHORT_DELAY = 140;
const int LONG_DELAY = 2200;
const int steps_per_rev = 1520;

//  Digital Pin Globals
uint8_t freq_pin = 2;
const int hallPin = 3;
Stepper motor(steps_per_rev, 4, 5, 6, 7);
int batterySwitch = 8;
int alternatorSwitch = 9;
int alternatorSwitch2 = 10;
int PWMPin = 11;
int pwmValue = 0;

//Analog pin Globlas
int ANALOG_PIN = A6;    // Analog pin for battery voltage
int lowLedPin = A1;     // Red LED for low O2
int normalLedPin = A2;  // Green LED for normal O2
int serviceRequired = A3;
int alternatorPower = A4;
int RGBred = A5;
int RGBgreen = A0;

int x = 0;



// Logger task
void taskLogger(void *pv) {
  for (;;) {
    if (flowRate > 10.0) {
      pwmValue = 255;
    } else {
      // Map flowRate (0–10) to PWM duty cycle (90–125)
      pwmValue = (int)mapFloat(flowRate, 0.0, 10.0, 90.0, 255.0);

      analogWrite(PWMPin, pwmValue);
    }

    Serial.print("Purity: ");
    Serial.print(currentPurity);
    Serial.print(" Flow: ");
    Serial.print(flowRate);
    Serial.print(" Bat: ");
    Serial.print(batteryVoltage, 2);
    Serial.print(" ALT: ");
    Serial.println(alternatorVoltage, 2);

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(hallPin, INPUT_PULLUP);
  pinMode(freq_pin, INPUT);

  TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20);  // no prescaler
  attachInterrupt(digitalPinToInterrupt(freq_pin), edgeISR, RISING);



  // Create the FreeRTOS tasks
  xTaskCreate(taskOxygen, "O2 Task", 60, NULL, 2, NULL);
  xTaskCreate(taskLogger, "Logger", 128, NULL, 1, NULL);
  xTaskCreate(taskStepper, "Stepper Task", 100, NULL, 3, NULL);
  xTaskCreate(taskIndicators, "Indicators", 60, NULL, 1, NULL);
  xTaskCreate(taskBatteryVoltage, "BatteryVolt", 70, NULL, 2, NULL);
  xTaskCreate(taskVoltageCapture, "FreqVolt", 70, NULL, 2, NULL);
  // xTaskCreate(taskControlVoltage, "ControlVoltage", 70, NULL, 2, NULL);
}

void loop() {
  // FreeRTOS handles scheduling
}
