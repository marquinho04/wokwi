/*
  Led.cpp
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Implements the Led actuator.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#include <Arduino.h>
#include "Led.h"

const Command Led::TURN_ON_COMMAND(Led::TURN_ON_COMMAND_IDENTIFIER);
const Command Led::TURN_OFF_COMMAND(Led::TURN_OFF_COMMAND_IDENTIFIER);

Led::Led(int signalPin, CommandHandler* parentHandler)
    : Actuator(signalPin, parentHandler), turnedOn(false) {}

void Led::begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    turnedOn = false;
}

void Led::executeCommand(Command command) {
    if (command == TURN_ON_COMMAND) {
        turnedOn = true;
    } else if (command == TURN_OFF_COMMAND) {
        turnedOn = false;
    } else {
        return; // Unknown command: ignored, state is kept
    }
    digitalWrite(pin, turnedOn ? HIGH : LOW);
}

void Led::setState(bool shouldBeOn) {
    // Avoid redundant commands: act only on a real state change
    if (shouldBeOn == turnedOn) {
        return;
    }
    handle(shouldBeOn ? TURN_ON_COMMAND : TURN_OFF_COMMAND);
}

bool Led::isOn() const {
    return turnedOn;
}
