/*
  Led.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  LED actuator with TURN_ON / TURN_OFF commands.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include "ModestIoT.h"

class Led : public Actuator {
private:
    bool turnedOn = false;

public:
    static const int TURN_ON_COMMAND = 100;
    static const int TURN_OFF_COMMAND = 101;

    explicit Led(int signalPin) : Actuator(signalPin) {}

    void begin() {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    void handle(Command command) override {
        if (command.identifier == TURN_ON_COMMAND) {
            turnedOn = true;
        } else if (command.identifier == TURN_OFF_COMMAND) {
            turnedOn = false;
        } else {
            return; // Unknown command: ignored
        }
        digitalWrite(pin, turnedOn ? HIGH : LOW);
    }

    /** Sends a command only when the state really changes. */
    void setState(bool shouldBeOn) {
        if (shouldBeOn == turnedOn) {
            return;
        }
        if (shouldBeOn) {
            handle(Command(TURN_ON_COMMAND));
        } else {
            handle(Command(TURN_OFF_COMMAND));
        }
    }

    bool isOn() const { return turnedOn; }
};

#endif // LED_H
