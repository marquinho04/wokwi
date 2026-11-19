/*
  WeightSensor.h
  Generic IoT kit - Embedded Application (Modest IoT based)

  Load cell + HX711 amplifier. Value in kilograms.
  Requires the "HX711 Arduino Library" in the Wokwi Library Manager.
  Reports Event::FAULT when the HX711 does not answer.

  calibrationFactor = raw counts per kg. The Wokwi "50kg" part gives
  0..21000 counts for 0..50 kg (420 per kg); use 105 to simulate a
  200 kg platform of four 50 kg cells (slider 20 kg -> 80 kg).

  Created 19 November 2025
  By <Developer Full Name>
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
    float calibrationFactor;

protected:
    int read() override {
        if (!scale.wait_ready_timeout(READ_TIMEOUT_IN_MILLISECONDS)) {
            return Event::FAULT;
        }
        float reading = scale.get_units(1);
        value = (reading < 0.0f) ? 0.0f : reading; // Noise after tare
        return Event::MEASURED;
    }

public:
    WeightSensor(int sensorId, int dataPin, int sckPin, float countsPerKilogram, EventHandler* handler)
        : Sensor(sensorId, dataPin, handler), clockPin(sckPin), calibrationFactor(countsPerKilogram) {}

    /** Starts and tares the scale. Returns false if the HX711 does not answer. */
    bool begin() override {
        scale.begin(pin, clockPin);
        scale.set_scale(calibrationFactor);
        if (!scale.wait_ready_timeout(STARTUP_TIMEOUT_IN_MILLISECONDS)) {
            return false;
        }
        scale.tare();
        return true;
    }
};

#endif // WEIGHT_SENSOR_H
