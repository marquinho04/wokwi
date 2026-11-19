/*
  Sensor.cpp
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Implements the Sensor base class.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#include "Sensor.h"

const Event Sensor::MEASURE_DATA_REQUESTED_EVENT(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER);

Sensor::Sensor(int signalPin, EventHandler* parentHandler)
    : pin(signalPin), eventHandler(parentHandler) {}

void Sensor::on(Event event) {
    // Execute the sensor-specific reading logic
    processEvent(event);

    // Push the resulting event upstream to the mediator (Device)
    if (eventHandler != nullptr) {
        eventHandler->on(event);
    }
}

void Sensor::setHandler(EventHandler* parentHandler) {
    eventHandler = parentHandler;
}
