/*
  WeightSensor.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Load cell + HX711 sensor. Reports the weight in kg, or a fault event
  when the HX711 does not answer.

  Calibration: the platform uses four 50 kg load cells (200 kg). Wokwi
  simulates it with one HX711 "50kg" part (raw 0..21000), so
  21000 / 200 = 105 counts per kg (slider 20 kg -> 80 kg).

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include "HX711.h"
#include "ModestIoT.h"

class WeightSensor : public Sensor {
private:
    static const unsigned long READ_TIMEOUT_IN_MILLISECONDS = 200;
    static const unsigned long STARTUP_TIMEOUT_IN_MILLISECONDS = 1000;
    HX711 scale;
    int clockPin;
    float countsPerKilogram;
    float weightInKilograms = 0.0f;

protected:
    int read() override {
        if (!scale.wait_ready_timeout(READ_TIMEOUT_IN_MILLISECONDS)) {
            return SCALE_FAULT_EVENT; // Exception: HX711 not responding
        }
        float reading = scale.get_units(1);
        weightInKilograms = (reading < 0.0f) ? 0.0f : reading; // Noise after tare
        return WEIGHT_MEASURED_EVENT;
    }

public:
    static const int WEIGHT_MEASURED_EVENT = 30;
    static const int SCALE_FAULT_EVENT = 31;

    WeightSensor(int dataPin, int sckPin, float calibrationFactor, EventHandler* handler)
        : Sensor(dataPin, handler), clockPin(sckPin), countsPerKilogram(calibrationFactor) {}

    /** Starts the HX711 and tares it. Returns false if it does not answer. */
    bool begin() {
        scale.begin(pin, clockPin);
        scale.set_scale(countsPerKilogram);
        if (!scale.wait_ready_timeout(STARTUP_TIMEOUT_IN_MILLISECONDS)) {
            return false;
        }
        scale.tare();
        return true;
    }

    float getWeightInKilograms() const { return weightInKilograms; }
};

#endif // WEIGHT_SENSOR_H
