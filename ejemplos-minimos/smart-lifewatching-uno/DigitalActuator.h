/*
  DigitalActuator.h
  Generic IoT kit - Embedded Application (Modest IoT based)

  On/off actuators: Led, Relay (valve, pump, lock...) and Buzzer.
  Commands: TURN_ON, TURN_OFF, TOGGLE. setState() only sends a command
  when the state really changes.

  Created 19 November 2025
  By <Developer Full Name>
*/

#ifndef DIGITAL_ACTUATOR_H
#define DIGITAL_ACTUATOR_H

#include "ModestIoT.h"

class DigitalActuator : public Actuator {
protected:
    bool turnedOn = false;

    /** Writes the state to the hardware. */
    virtual void apply(bool turnOn) {
        turnedOn = turnOn;
        digitalWrite(pin, turnedOn ? HIGH : LOW);
    }

public:
    explicit DigitalActuator(int signalPin) : Actuator(signalPin) {}

    void begin() override {
        pinMode(pin, OUTPUT);
        apply(false);
    }

    void handle(Command command) override {
        switch (command.type) {
            case Command::TURN_ON:  apply(true); break;
            case Command::TURN_OFF: apply(false); break;
            case Command::TOGGLE:   apply(!turnedOn); break;
            default:                break; // Unknown command: ignored
        }
    }

    void setState(bool shouldBeOn) {
        if (shouldBeOn == turnedOn) {
            return;
        }
        if (shouldBeOn) {
            handle(Command(Command::TURN_ON));
        } else {
            handle(Command(Command::TURN_OFF));
        }
    }

    bool isOn() const { return turnedOn; }
};

/** Light indicator. */
class Led : public DigitalActuator {
public:
    explicit Led(int signalPin) : DigitalActuator(signalPin) {}
};

/** Relay module (water valve, pump, door lock...). */
class Relay : public DigitalActuator {
public:
    explicit Relay(int signalPin) : DigitalActuator(signalPin) {}
};

/** Piezo buzzer: plays a tone while turned on. */
class Buzzer : public DigitalActuator {
private:
    unsigned int frequencyInHertz;

protected:
    void apply(bool turnOn) override {
        turnedOn = turnOn;
        if (turnedOn) {
            tone(pin, frequencyInHertz);
        } else {
            noTone(pin);
        }
    }

public:
    Buzzer(int signalPin, unsigned int frequency) : DigitalActuator(signalPin), frequencyInHertz(frequency) {}
};

#endif // DIGITAL_ACTUATOR_H
