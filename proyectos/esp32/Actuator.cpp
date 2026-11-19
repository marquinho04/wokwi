/*
  Actuator.cpp
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Implements the Actuator base class.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#include "Actuator.h"

Actuator::Actuator(int signalPin, CommandHandler* parentHandler)
    : pin(signalPin), commandHandler(parentHandler) {}

void Actuator::handle(Command command) {
    // Execute the hardware-specific behavior
    executeCommand(command);

    // Propagate the command to the parent handler for monitoring
    if (commandHandler != nullptr) {
        commandHandler->handle(command);
    }
}

void Actuator::setHandler(CommandHandler* parentHandler) {
    commandHandler = parentHandler;
}
