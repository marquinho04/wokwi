/*
  WeightSensor.cpp
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Implements the WeightSensor (HX711 load cell).

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#include "WeightSensor.h"

WeightSensor::WeightSensor(int dataPin, int sckPin, float countsPerKilogram, EventHandler* parentHandler)
    : Sensor(dataPin, parentHandler),
      clockPin(sckPin),
      calibrationFactor(countsPerKilogram),
      weightInKilograms(0.0f) {}

bool WeightSensor::begin() {
    scale.begin(pin, clockPin);
    scale.set_scale(calibrationFactor);

    // Tare only if the HX711 answers, to avoid blocking forever
    if (!scale.wait_ready_timeout(STARTUP_TIMEOUT_IN_MILLISECONDS)) {
        return false;
    }
    scale.tare();
    return true;
}

void WeightSensor::processEvent(Event& event) {
    if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) {
        return;
    }

    // Exception: HX711 not ready / disconnected
    if (!scale.wait_ready_timeout(READ_TIMEOUT_IN_MILLISECONDS)) {
        event.identifier = SCALE_FAULT_EVENT_IDENTIFIER;
        return;
    }

    float reading = scale.get_units(1);

    // Small negative values (noise after tare) are reported as 0 kg
    weightInKilograms = (reading < 0.0f) ? 0.0f : reading;
    event.identifier = WEIGHT_MEASURED_EVENT_IDENTIFIER;
}

float WeightSensor::getWeightInKilograms() const {
    return weightInKilograms;
}
