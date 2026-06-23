#include "health_classifier.h"

HealthState HealthClassifier::classify(const BatteryMonitor& monitor) {
    float imbalance = monitor.getImbalancePercentage();

    // 1. Pack Failure checks (Safety Critical)
    for (int i = 0; i < 4; i++) {
        float volt = monitor.getCellVoltage(i);
        if (volt < CELL_UNDERVOLTAGE_LIMIT || volt > CELL_OVERVOLTAGE_LIMIT) {
            return PACK_FAILURE;
        }
    }
    
    if (imbalance >= THRESHOLD_PACK_FAILURE) {
        return PACK_FAILURE;
    }

    // 2. Critical Imbalance check
    if (imbalance >= THRESHOLD_CRITICAL_IMBALANCE) {
        return CRITICAL_IMBALANCE;
    }

    // 3. Minor Imbalance check
    if (imbalance >= THRESHOLD_MINOR_IMBALANCE) {
        return MINOR_IMBALANCE;
    }

    // 4. Healthy State
    return HEALTHY;
}

const char* HealthClassifier::getStateString(HealthState state) {
    switch (state) {
        case HEALTHY:
            return "HEALTHY";
        case MINOR_IMBALANCE:
            return "MINOR IMBALANCE";
        case CRITICAL_IMBALANCE:
            return "CRITICAL IMBALANCE";
        case PACK_FAILURE:
            return "PACK FAILURE";
        default:
            return "UNKNOWN";
    }
}
