/*
  AnalogSensor.h
  Generic IoT kit - Embedded Application (Modest IoT based)

  Any analog sensor: potentiometer, LDR, gas, soil moisture, simulated
  pulse sensor... The raw reading (0..ANALOG_MAXIMUM_VALUE) is mapped to a
  physical range (for example 0..200 BPM). Readings outside the valid range
  are reported as Event::FAULT.

  Created 19 November 2025
  By <Developer Full Name>
*/

#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H

#include "ModestIoT.h"

class AnalogSensor : public Sensor {
private:
    float minimumValue;      ///< Physical value for raw 0.
    float maximumValue;      ///< Physical value for raw ANALOG_MAXIMUM_VALUE.
    float minimumValidValue; ///< Below this: fault (e.g. sensor disconnected).
    float maximumValidValue; ///< Above this: fault.

protected:
    int read() override {
        int rawValue = analogRead(pin);
        float measuredValue = minimumValue
            + (maximumValue - minimumValue) * rawValue / ANALOG_MAXIMUM_VALUE;
        if (measuredValue < minimumValidValue || measuredValue > maximumValidValue) {
            return Event::FAULT;
        }
        value = measuredValue;
        return Event::MEASURED;
    }

public:
    AnalogSensor(int sensorId, int analogPin, float minimum, float maximum, EventHandler* handler)
        : Sensor(sensorId, analogPin, handler),
          minimumValue(minimum), maximumValue(maximum),
          minimumValidValue(minimum), maximumValidValue(maximum) {}

    /** Restricts the accepted values; outside them the sensor reports Event::FAULT. */
    void setValidRange(float minimumValid, float maximumValid) {
        minimumValidValue = minimumValid;
        maximumValidValue = maximumValid;
    }
};

#endif // ANALOG_SENSOR_H
