#include "OxygenTask.h"
#include "StepperTask.h"
#include "Indicators.h"
#include "VoltageSystem.h"
// Define globals here (storage for the externs)
float currentPurity = 0.0;
float flowRate = 0.0;

//  Voltage Globals
int ANALOG_PIN = A0;     // Analog pin for battery voltage
float VCC = 4.5;       // Assume 5V reference for ADC
uint8_t SIGNAL_PIN = 2;
float batteryVoltage = 0.0;  // Shared voltage value
float voltage = 0.0;         // From frequency input
float EMA_voltage = 0.0;

int red = 8;
int green = 7;

int batterySwitch = 5;
int alternatorSwitch = 4; 

// User decides which pins to use
int normalLedPin = 5;  // Green LED for normal O2
int lowLedPin    = 7;  // Red LED for low O2

// ====== Globals (real definitions) ======
const int steps_per_rev = 1520;
Stepper motor(steps_per_rev, 9, 10, 5, 6);

const int hallPin = 3;

 int PWMPin = 11;
int x =0;
const int MAX_HOMING_STEPS = 5000;
const int SHORT_DELAY = 140;
const int LONG_DELAY  = 2200;
// Logger task
void taskLogger(void *pv) {
  for (;;) {
        x = x + 1;
    if(x > 80){
      x = 0;
    analogWrite(PWMPin, 255);
    }
    Serial.print("Purity: ");
    Serial.print(currentPurity);
    Serial.print(" Flow: ");
    Serial.print(flowRate);
        Serial.print(" Bat: ");
        Serial.print(batteryVoltage, 2);
        Serial.print(" ALT: ");
        Serial.println(voltage, 2);

    vTaskDelay(pdMS_TO_TICKS(10)); 
  }
}

void setup() {
  Serial.begin(9600);
   pinMode(hallPin, INPUT_PULLUP);
    pinMode(SIGNAL_PIN, INPUT);
  
  TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20);   // no prescaler
    attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), edgeISR, RISING);



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
