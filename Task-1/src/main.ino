#include "battery_monitor.h"
#include "health_classifier.h"

// Instantiate Battery Monitor with the default ESP32 analog pins
BatteryMonitor monitor(34, 35, 32, 33);

unsigned long startTime;
unsigned long lastReportTime = 0;
const unsigned long REPORT_INTERVAL = 2000; // 2 seconds update rate matching screenshot loop

// Define cell voltages for the 4 demo states (matching user request)
const float demoHealthy[4]    = {4.01f, 4.00f, 4.02f, 4.00f}; 
const float demoMinor[4]      = {4.12f, 4.00f, 4.07f, 4.05f}; 
const float demoCritical[4]   = {4.21f, 3.98f, 4.15f, 3.96f}; 
const float demoFailure[4]    = {4.30f, 3.85f, 4.10f, 3.75f}; 

void setup() {
    Serial.begin(115200);
    monitor.begin();
    startTime = millis();
}

void loop() {
    unsigned long currentMillis = millis();
    
    if (currentMillis - lastReportTime >= REPORT_INTERVAL) {
        lastReportTime = currentMillis;
        
        unsigned long elapsedSec = (currentMillis - startTime) / 1000;
        bool demoMode = true;
        const float* activeDemoVoltages = nullptr;
        
        // Demo sequencing based on elapsed time (first 20 seconds)
        if (elapsedSec < 5) {
            activeDemoVoltages = demoHealthy;
        } else if (elapsedSec < 10) {
            activeDemoVoltages = demoMinor;
        } else if (elapsedSec < 15) {
            activeDemoVoltages = demoCritical;
        } else if (elapsedSec < 20) {
            activeDemoVoltages = demoFailure;
        } else {
            demoMode = false;
        }
        
        // Update readings
        monitor.update(demoMode, activeDemoVoltages);
        
        // Classify health
        HealthState health = HealthClassifier::classify(monitor);
        const char* healthStr = HealthClassifier::getStateString(health);
        
        // Print report in the exact format shown in the user's screenshot
        Serial.println(F("===== BATTERY REPORT ====="));
        for (int i = 0; i < 4; i++) {
            Serial.print(F("Cell "));
            Serial.print(i + 1);
            Serial.print(F(": "));
            Serial.print(monitor.getCellVoltage(i), 2);
            Serial.println(F(" V"));
        }
        
        Serial.print(F("Pack Voltage: "));
        Serial.println(monitor.getPackTotal(), 2);
        
        Serial.print(F("Average Voltage: "));
        Serial.println(monitor.getPackAverage(), 2);
        
        Serial.print(F("Strongest Cell: "));
        Serial.println(monitor.getStrongestCell());
        
        Serial.print(F("Weakest Cell: "));
        Serial.println(monitor.getWeakestCell());
        
        Serial.print(F("Imbalance: "));
        Serial.print(monitor.getImbalancePercentage(), 2);
        Serial.println(F(" %"));
        
        Serial.print(F("Health Status: "));
        Serial.println(healthStr);
        
        // Print warning flags if cell voltages exceed safety limits
        bool underVoltage = false;
        bool overVoltage = false;
        for (int i = 0; i < 4; i++) {
            float v = monitor.getCellVoltage(i);
            if (v < HealthClassifier::CELL_UNDERVOLTAGE_WARN) {
                underVoltage = true;
            }
            if (v > HealthClassifier::CELL_OVERVOLTAGE_WARN) {
                overVoltage = true;
            }
        }
        
        if (underVoltage) {
            Serial.println(F("WARNING: UNDER-VOLTAGE DETECTED!"));
        }
        if (overVoltage) {
            Serial.println(F("WARNING: OVER-VOLTAGE DETECTED!"));
        }
        
        Serial.println(F("=========================="));
    }
}
