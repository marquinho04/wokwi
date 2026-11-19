/*
  DigitalSensor.h
  Generic IoT kit - Embedded Application (Modest IoT based)

  Any on/off sensor: push button, PIR motion sensor, switch, tilt sensor.
  Notifies Event::CHANGED only when the state changes. Use activeLow = true
  for buttons wired to GND (internal pull-up).

  Created 19 November 2025
  By <Developer Full Name>
*/

#ifndef DIGITAL_SENSOR_H
#define DIGITAL_SENSOR_H

#include "ModestIoT.h"

class DigitalSensor : public Sensor {
private:
    bool activeLow;
    bool active = false;

protected:
    int read() override {
        bool currentlyActive = (digitalRead(pin) == (activeLow ? LOW : HIGH));
        if (currentlyActive == active) {
            return Event::NONE;
        }
        active = currentlyActive;
        value = active ? 1.0f : 0.0f;
        return Event::CHANGED;
    }

public:
    DigitalSensor(int sensorId, int signalPin, bool isActiveLow, EventHandler* handler)
        : Sensor(sensorId, signalPin, handler), activeLow(isActiveLow) {}

    bool begin() override {
        pinMode(pin, activeLow ? INPUT_PULLUP : INPUT);
        return true;
    }

    bool isActive() const { return active; }
};

#endif // DIGITAL_SENSOR_H
