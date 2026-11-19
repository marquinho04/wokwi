/*
  WeightSensor.h
  Generic IoT kit - Embedded Application (Modest IoT based)

  Load cell + HX711 amplifier. Value in kilograms.
  Requires the "HX711 Arduino Library" in the Wokwi Library Manager.
  Non-blocking: a new value is taken only when the HX711 has data ready;
  Event::FAULT is reported when no data arrives for 2 seconds.

  calibrationFactor = raw counts per kg. The Wokwi "50kg" part gives
  0..21000 counts for 0..50 kg (420 per kg); use 105 to simulate a
  200 kg platform of four 50 kg cells (slider 20 kg -> 80 kg).
  No tare is done: the Wokwi load cell already reads 0 counts at 0 kg.

  Created 19 November 2025
  By <Developer Full Name>
*/

#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include "HX711.h"
#include "ModestIoT.h"

class WeightSensor : public Sensor {
private:
    static const unsigned long FAULT_TIMEOUT_IN_MILLISECONDS = 2000; // No data for 2 s -> fault
    HX711 scale;
    int clockPin;
    float calibrationFactor;
    unsigned long lastReadingTimestamp = 0; // Last time valid data arrived

protected:
    int read() override {
        if (!scale.is_ready()) {
            // No new data yet: keep the last value, unless it has been too long
            if (millis() - lastReadingTimestamp > FAULT_TIMEOUT_IN_MILLISECONDS) {
                return Event::FAULT; // Exception: HX711 not responding
            }
            return Event::NONE;
        }
        float reading = scale.get_units(1);
        value = (reading < 0.0f) ? 0.0f : reading; // Negative noise shown as 0 kg
        lastReadingTimestamp = millis();
        return Event::MEASURED;
    }

public:
    WeightSensor(int sensorId, int dataPin, int sckPin, float countsPerKilogram, EventHandler* handler)
        : Sensor(sensorId, dataPin, handler), clockPin(sckPin), calibrationFactor(countsPerKilogram) {}

    /** Configures the HX711 pins and the calibration factor. Never blocks. */
    bool begin() override {
        scale.begin(pin, clockPin);
        scale.set_scale(calibrationFactor);
        lastReadingTimestamp = millis();
        return true;
    }
};

#endif // WEIGHT_SENSOR_H
