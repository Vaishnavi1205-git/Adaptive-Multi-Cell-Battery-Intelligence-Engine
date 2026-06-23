#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>

class BatteryMonitor {
private:
    static const int NUM_CELLS = 4;
    int cellPins[NUM_CELLS];
    float cellVoltages[NUM_CELLS];
    float rawVoltages[NUM_CELLS];
    float emaAlpha; // Smoothing factor for EMA filter

    // Voltage mapping parameters (0.0V to 4.2V in simulation)
    float minCellVoltage; // Voltage at ADC = 0
    float maxCellVoltage; // Voltage at ADC = 4095

public:
    BatteryMonitor(int pin1 = 34, int pin2 = 35, int pin3 = 32, int pin4 = 33);

    void begin();
    void update(bool demoMode = false, const float demoVoltages[NUM_CELLS] = nullptr);

    float getCellVoltage(int cellIndex) const;
    float getRawVoltage(int cellIndex) const;
    float getPackAverage() const;
    float getPackTotal() const;
    float getAbsoluteImbalance() const;
    float getImbalancePercentage() const;
    
    int getWeakestCell() const;
    int getStrongestCell() const;

    float getMinVoltage() const;
    float getMaxVoltage() const;

    void setFilterAlpha(float alpha);
};

#endif // BATTERY_MONITOR_H
