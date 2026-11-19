/*
  Device.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Abstract base class for IoT devices. A Device is the mediator that owns
  its sensors and actuators: it receives events from sensors (EventHandler)
  and issues commands to actuators (CommandHandler).

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef DEVICE_H
#define DEVICE_H

#include "EventHandler.h"
#include "CommandHandler.h"

/**
 * @brief Base class for devices that coordinate sensors and actuators.
 */
class Device : public EventHandler, public CommandHandler {
public:
    /**
     * @brief Initializes hardware and shows startup information. Call from setup().
     */
    virtual void setup() = 0;

    /**
     * @brief Runs one non-blocking iteration of the device behavior. Call from loop().
     */
    virtual void loop() = 0;

    virtual ~Device() {}
};

#endif // DEVICE_H
