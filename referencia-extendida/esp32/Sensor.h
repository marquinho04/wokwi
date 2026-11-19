/*
  Sensor.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Abstract base class for input devices. A sensor receives a measure
  request event, performs its specific reading (template method
  processEvent) and propagates the resulting event to its parent handler
  (the Device).

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef SENSOR_H
#define SENSOR_H

#include "EventHandler.h"

/**
 * @brief Base class for sensors that generate events.
 */
class Sensor : public EventHandler {
protected:
    int pin;                    ///< Main GPIO pin assigned to the sensor.
    EventHandler* eventHandler; ///< Parent handler that receives propagated events.

    /**
     * @brief Template-method hook: reads the hardware and turns the request
     *        event into the specific result event.
     * @param event Event being processed (modified in place).
     */
    virtual void processEvent(Event& event) = 0;

public:
    static const int MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER = 20; ///< Request a new reading.
    static const Event MEASURE_DATA_REQUESTED_EVENT;               ///< Prebuilt request event.

    /**
     * @brief Constructs a Sensor.
     * @param signalPin GPIO pin for the sensor.
     * @param parentHandler Handler that receives the events (default: nullptr).
     */
    Sensor(int signalPin, EventHandler* parentHandler = nullptr);

    /**
     * @brief Processes the event locally and forwards it to the parent handler.
     */
    void on(Event event) override;

    /**
     * @brief Sets or replaces the parent handler.
     */
    void setHandler(EventHandler* parentHandler);
};

#endif // SENSOR_H
