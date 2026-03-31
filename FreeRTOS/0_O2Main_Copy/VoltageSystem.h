// ----------------------------------------------------------------------
// VoltageSystem.h
// ----------------------------------------------------------------------
#ifndef VOLTAGE_SYSTEM_H
#define VOLTAGE_SYSTEM_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <string.h> // for memcpy

// ================= Pin and Divider Config =================
extern int ANALOG_PIN;     // Analog pin for battery voltage
extern float VCC;               // ADC reference voltage
extern uint8_t freq_pin;        // Frequency/voltage input pin (interrupt capable)

// ================= Shared Variables =================
extern float batteryVoltage;  // From analog read
extern float alternatorVoltage;         // From frequency input
extern float EMA_voltage;

// ================= Constants for Frequency Capture =================
constexpr uint8_t MEDIAN_SIZE = 5;
extern float ALPHA;

volatile uint32_t periodBuffer[MEDIAN_SIZE];  // microseconds
volatile uint8_t bufferIndex = 0;
volatile bool dataReady = false;
volatile uint32_t lastCapture = 0;

// ================= Forward Declarations =================
void edgeISR();
uint32_t medianFilter(volatile uint32_t buffer[]);
float computeEMA(float input);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
float periodToVoltage(uint32_t period_us);

// ================= Task Functions =================

// Task: Read battery voltage from analog pin
inline void taskBatteryVoltage(void *pv) {
    (void) pv;
    for (;;) {
        int adcValue = analogRead(ANALOG_PIN);
        float vOut = adcValue * (VCC / 1023.0);
        batteryVoltage = vOut * 4.0; // Assuming 1:4 voltage divider
        vTaskDelay(pdMS_TO_TICKS(1000)); // Every 1 second
    }
}

// Task: Compute voltage from frequency/edge input
inline void taskVoltageCapture(void *pv) {
    (void) pv;

    TickType_t lastUpdateTick = xTaskGetTickCount(); // track last update time

    for (;;) {
        if (dataReady) {
            dataReady = false;
            uint32_t medianPeriod = medianFilter(periodBuffer);
            alternatorVoltage = computeEMA(periodToVoltage(medianPeriod));
            lastUpdateTick = xTaskGetTickCount(); // reset timeout
        } else {
            // If no edges for > 500 ms, reset alternator voltage
            if ((xTaskGetTickCount() - lastUpdateTick) > pdMS_TO_TICKS(500)) {
                alternatorVoltage = 0.0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50)); // run every 50 ms
    }
}



// // Task: Compute voltage from frequency/edge input
// inline void taskVoltageCapture(void *pv) {
//     (void) pv;
//     for (;;) {
//         if (dataReady) {
//             dataReady = false;
//             uint32_t medianPeriod = medianFilter(periodBuffer);
//             alternatorVoltage = computeEMA(periodToVoltage(medianPeriod));
//         }
//         vTaskDelay(pdMS_TO_TICKS(50)); // Every 50 ms
//     }
// }

// Task: Logger
// inline void taskLogger(void *pv) {
//     (void) pv;
//     for (;;) {
//         Serial.print("Analog Battery Voltage: ");
//         Serial.print(batteryVoltage, 2);
//         Serial.print(" V, Frequency Voltage: ");
//         Serial.println(voltage, 2);
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Every 1 second
//     }
// }

// ================= Edge ISR =================
inline void edgeISR() {
    uint32_t now = micros();
    uint32_t period = now - lastCapture;
    lastCapture = now;

    if (period > 0) {
        periodBuffer[bufferIndex++] = period;
        if (bufferIndex >= MEDIAN_SIZE) {
            dataReady = true;
            bufferIndex = 0;
        }
    }
}

// ================= Median Filter =================
inline uint32_t medianFilter(volatile uint32_t buffer[]) {
    uint32_t a[MEDIAN_SIZE];
    memcpy((void*)a, (const void*)buffer, MEDIAN_SIZE * sizeof(uint32_t));

    #define MINMAX(i,j) { uint32_t x=a[i],y=a[j]; a[i]=(x<y)?x:y; a[j]=(x>y)?x:y; }
    MINMAX(0,1); MINMAX(3,4);
    MINMAX(2,4); MINMAX(2,3);
    MINMAX(1,4); MINMAX(0,3);
    MINMAX(0,2); MINMAX(1,3);
    MINMAX(1,2);
    #undef MINMAX

    return a[2]; // Median
}

// ================= EMA & Voltage Conversion =================
inline float computeEMA(float input) {
    EMA_voltage = ALPHA * input + (1 - ALPHA) * EMA_voltage;
    return EMA_voltage;
}

inline float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline float periodToVoltage(uint32_t period_us) {
    float freq = 1000000.0 / period_us;
    float voltage = mapFloat(freq, 500.0, 4900.0, 10.0, 14.4);
    return constrain(voltage, 10.0, 14.4);
}

#endif // VOLTAGE_SYSTEM_H
