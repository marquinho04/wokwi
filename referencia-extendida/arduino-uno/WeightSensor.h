/*
  WeightSensor.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Weight sensor based on a load cell and the HX711 amplifier.
  Emits WEIGHT_MEASURED_EVENT on a valid reading and SCALE_FAULT_EVENT
  when the HX711 does not answer (disconnected or not ready).

  The circuit:
  * HX711 DT  -> Arduino digital pin 3
  * HX711 SCK -> Arduino digital pin 2

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include "HX711.h"
#include "Sensor.h"

/**
 * @brief Load cell sensor (HX711) that reports the patient's weight in kilograms.
 */
class WeightSensor : public Sensor {
private:
    HX711 scale;              ///< HX711 driver (HX711 Arduino Library).
    int clockPin;             ///< HX711 SCK pin.
    float calibrationFactor;  ///< Raw counts per kilogram.
    float weightInKilograms;  ///< Last valid weight reading.

    static const unsigned long READ_TIMEOUT_IN_MILLISECONDS = 200;   ///< Max wait for HX711 data.
    static const unsigned long STARTUP_TIMEOUT_IN_MILLISECONDS = 1000; ///< Max wait at startup.

protected:
    void processEvent(Event& event) override;

public:
    static const int WEIGHT_MEASURED_EVENT_IDENTIFIER = 30; ///< New weight available.
    static const int SCALE_FAULT_EVENT_IDENTIFIER = 31;     ///< HX711 not responding.

    /**
     * @brief Constructs the weight sensor.
     * @param dataPin HX711 DT pin.
     * @param sckPin HX711 SCK pin.
     * @param countsPerKilogram Calibration factor (raw counts per kg).
     * @param parentHandler Handler that receives the events.
     */
    WeightSensor(int dataPin, int sckPin, float countsPerKilogram, EventHandler* parentHandler = nullptr);

    /**
     * @brief Initializes the HX711, applies the calibration factor and tares the scale.
     * @return true if the HX711 answered, false otherwise.
     */
    bool begin();

    /**
     * @brief Returns the last valid weight in kilograms.
     */
    float getWeightInKilograms() const;
};

#endif // WEIGHT_SENSOR_H
