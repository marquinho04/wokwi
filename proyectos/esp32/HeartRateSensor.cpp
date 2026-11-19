/*
  HeartRateSensor.cpp
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Implements the HeartRateSensor.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#include <Arduino.h>
#include "HeartRateSensor.h"

HeartRateSensor::HeartRateSensor(int analogPin, EventHandler* parentHandler)
    : Sensor(analogPin, parentHandler), beatsPerMinute(0) {}

void HeartRateSensor::processEvent(Event& event) {
    if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) {
        return;
    }

    int analogValue = analogRead(pin);
    int measuredBeatsPerMinute = map(analogValue, 0, MAXIMUM_ANALOG_VALUE,
                                     0, MAXIMUM_MEASURABLE_BEATS_PER_MINUTE);

    // Exception: no valid pulse (sensor not placed or disconnected)
    if (measuredBeatsPerMinute < MINIMUM_PHYSIOLOGICAL_BEATS_PER_MINUTE) {
        beatsPerMinute = 0;
        event.identifier = NO_SIGNAL_EVENT_IDENTIFIER;
        return;
    }

    beatsPerMinute = measuredBeatsPerMinute;
    event.identifier = HEART_RATE_MEASURED_EVENT_IDENTIFIER;
}

int HeartRateSensor::getBeatsPerMinute() const {
    return beatsPerMinute;
}
