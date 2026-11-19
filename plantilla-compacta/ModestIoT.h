/*
  ModestIoT.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Core abstractions of the Modest IoT approach:
  * Event / Command: value objects (events inform, commands order).
  * Sensor: reads hardware and notifies events to its handler (the Device).
  * Actuator: executes commands.
  * Device: mediator that owns sensors and actuators.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef MODEST_IOT_H
#define MODEST_IOT_H

/** Event emitted by a sensor. */
struct Event {
    int identifier;
    explicit Event(int eventIdentifier) : identifier(eventIdentifier) {}
};

/** Command executed by an actuator. */
struct Command {
    int identifier;
    explicit Command(int commandIdentifier) : identifier(commandIdentifier) {}
};

/** Interface for components that react to events. */
class EventHandler {
public:
    virtual void on(Event event) = 0;
    virtual ~EventHandler() {}
};

/** Interface for components that execute commands. */
class CommandHandler {
public:
    virtual void handle(Command command) = 0;
    virtual ~CommandHandler() {}
};

/** Base class for sensors: read() fills the event, then it goes to the handler. */
class Sensor {
protected:
    int pin;
    EventHandler* eventHandler;

    /** Reads the hardware and returns the resulting event identifier. */
    virtual int read() = 0;

public:
    Sensor(int signalPin, EventHandler* handler) : pin(signalPin), eventHandler(handler) {}
    virtual ~Sensor() {}

    /** Takes a reading and notifies the handler. */
    void measure() {
        Event event(read());
        if (eventHandler != nullptr) {
            eventHandler->on(event);
        }
    }
};

/** Base class for actuators. */
class Actuator : public CommandHandler {
protected:
    int pin;

public:
    explicit Actuator(int signalPin) : pin(signalPin) {}
};

/** Base class for devices: mediator between sensors and actuators. */
class Device : public EventHandler {
public:
    virtual void setup() = 0;
    virtual void loop() = 0;
};

#endif // MODEST_IOT_H
