#include "battery_monitor.h"

BatteryMonitor::BatteryMonitor(int pin1, int pin2, int pin3, int pin4) {
    cellPins[0] = pin1;
    cellPins[1] = pin2;
    cellPins[2] = pin3;
    cellPins[3] = pin4;
    
    emaAlpha = 0.20f; // 20% new reading, 80% history
    minCellVoltage = 0.00f; // 0 ADC = 0.0V
    maxCellVoltage = 4.20f; // 4095 ADC = 4.2V
    
    for (int i = 0; i < NUM_CELLS; i++) {
        cellVoltages[i] = 3.70f;
        rawVoltages[i] = 3.70f;
    }
}

void BatteryMonitor::begin() {
    for (int i = 0; i < NUM_CELLS; i++) {
        pinMode(cellPins[i], INPUT);
    }
}

void BatteryMonitor::update(bool demoMode, const float demoVoltages[NUM_CELLS]) {
    if (demoMode && demoVoltages != nullptr) {
        for (int i = 0; i < NUM_CELLS; i++) {
            cellVoltages[i] = demoVoltages[i];
            rawVoltages[i] = demoVoltages[i];
        }
        return;
    }
    
    for (int i = 0; i < NUM_CELLS; i++) {
        int adcVal = analogRead(cellPins[i]);
        // Map 12-bit ADC (0 - 4095) to simulated range (0.00V - 4.20V)
        float rawVolt = ((float)adcVal / 4095.0f) * maxCellVoltage;
        
        rawVoltages[i] = rawVolt;
        cellVoltages[i] = (emaAlpha * rawVolt) + ((1.0f - emaAlpha) * cellVoltages[i]);
    }
}

float BatteryMonitor::getCellVoltage(int cellIndex) const {
    if (cellIndex >= 0 && cellIndex < NUM_CELLS) {
        return cellVoltages[cellIndex];
    }
    return 0.0f;
}

float BatteryMonitor::getRawVoltage(int cellIndex) const {
    if (cellIndex >= 0 && cellIndex < NUM_CELLS) {
        return rawVoltages[cellIndex];
    }
    return 0.0f;
}

float BatteryMonitor::getPackAverage() const {
    float sum = 0.0f;
    for (int i = 0; i < NUM_CELLS; i++) {
        sum += cellVoltages[i];
    }
    return sum / (float)NUM_CELLS;
}

float BatteryMonitor::getPackTotal() const {
    float sum = 0.0f;
    for (int i = 0; i < NUM_CELLS; i++) {
        sum += cellVoltages[i];
    }
    return sum;
}

float BatteryMonitor::getAbsoluteImbalance() const {
    float vMin = cellVoltages[0];
    float vMax = cellVoltages[0];
    for (int i = 1; i < NUM_CELLS; i++) {
        if (cellVoltages[i] < vMin) vMin = cellVoltages[i];
        if (cellVoltages[i] > vMax) vMax = cellVoltages[i];
    }
    return vMax - vMin;
}

float BatteryMonitor::getImbalancePercentage() const {
    float vMax = getMaxVoltage();
    if (vMax < 0.1f) return 0.0f; // Avoid division by zero
    return (getAbsoluteImbalance() / vMax) * 100.0f;
}

int BatteryMonitor::getWeakestCell() const {
    float vMin = cellVoltages[0];
    int index = 0;
    for (int i = 1; i < NUM_CELLS; i++) {
        if (cellVoltages[i] < vMin) {
            vMin = cellVoltages[i];
            index = i;
        }
    }
    return index + 1;
}

int BatteryMonitor::getStrongestCell() const {
    float vMax = cellVoltages[0];
    int index = 0;
    for (int i = 1; i < NUM_CELLS; i++) {
        if (cellVoltages[i] > vMax) {
            vMax = cellVoltages[i];
            index = i;
        }
    }
    return index + 1;
}

float BatteryMonitor::getMinVoltage() const {
    float vMin = cellVoltages[0];
    for (int i = 1; i < NUM_CELLS; i++) {
        if (cellVoltages[i] < vMin) vMin = cellVoltages[i];
    }
    return vMin;
}

float BatteryMonitor::getMaxVoltage() const {
    float vMax = cellVoltages[0];
    for (int i = 1; i < NUM_CELLS; i++) {
        if (cellVoltages[i] > vMax) vMax = cellVoltages[i];
    }
    return vMax;
}

void BatteryMonitor::setFilterAlpha(float alpha) {
    if (alpha > 0.0f && alpha <= 1.0f) {
        emaAlpha = alpha;
    }
}
