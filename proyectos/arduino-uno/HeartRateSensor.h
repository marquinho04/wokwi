/*
  HeartRateSensor.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Heart rate sensor that reports beats per minute (BPM).
  Wokwi has no heart rate part, so the analog output of the sensor is
  simulated with a potentiometer: 0..1023 (ADC) -> 0..200 BPM.
  Readings below the physiological minimum (30 BPM) are reported as
  NO_SIGNAL_EVENT (sensor not placed or disconnected).

  The circuit:
  * Heart rate sensor (potentiometer) SIG -> Arduino analog pin A0

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef HEART_RATE_SENSOR_H
#define HEART_RATE_SENSOR_H

#include "Sensor.h"

/**
 * @brief Analog heart rate sensor that reports beats per minute.
 */
class HeartRateSensor : public Sensor {
private:
    int beatsPerMinute; ///< Last valid heart rate in BPM.

    static const int MAXIMUM_ANALOG_VALUE = 1023;                 ///< 10-bit ADC (Arduino Uno).
    static const int MAXIMUM_MEASURABLE_BEATS_PER_MINUTE = 200;   ///< Upper end of the sensor scale.
    static const int MINIMUM_PHYSIOLOGICAL_BEATS_PER_MINUTE = 30; ///< Below this: no valid signal.

protected:
    void processEvent(Event& event) override;

public:
    static const int HEART_RATE_MEASURED_EVENT_IDENTIFIER = 40; ///< New BPM available.
    static const int NO_SIGNAL_EVENT_IDENTIFIER = 41;           ///< No valid pulse detected.

    /**
     * @brief Constructs the heart rate sensor.
     * @param analogPin Analog input pin.
     * @param parentHandler Handler that receives the events.
     */
    HeartRateSensor(int analogPin, EventHandler* parentHandler = nullptr);

    /**
     * @brief Returns the last valid heart rate in BPM.
     */
    int getBeatsPerMinute() const;
};

#endif // HEART_RATE_SENSOR_H
