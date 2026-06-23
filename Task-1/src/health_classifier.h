#ifndef HEALTH_CLASSIFIER_H
#define HEALTH_CLASSIFIER_H

#include "battery_monitor.h"

enum HealthState {
    HEALTHY,
    MINOR_IMBALANCE,
    CRITICAL_IMBALANCE,
    PACK_FAILURE
};

class HealthClassifier {
public:
    // Imbalance thresholds in percentage
    static constexpr float THRESHOLD_MINOR_IMBALANCE = 1.50f;    // 1.5%
    static constexpr float THRESHOLD_CRITICAL_IMBALANCE = 5.00f; // 5.0%
    static constexpr float THRESHOLD_PACK_FAILURE = 10.00f;     // 10.0%

    // Physical threshold boundaries
    static constexpr float CELL_UNDERVOLTAGE_LIMIT = 2.80f;     // 2.80V
    static constexpr float CELL_OVERVOLTAGE_LIMIT = 4.25f;      // 4.25V

    // Warning boundaries
    static constexpr float CELL_UNDERVOLTAGE_WARN = 3.00f;      // 3.00V
    static constexpr float CELL_OVERVOLTAGE_WARN = 4.19f;       // 4.19V

    static HealthState classify(const BatteryMonitor& monitor);
    static const char* getStateString(HealthState state);
};

#endif // HEALTH_CLASSIFIER_H
