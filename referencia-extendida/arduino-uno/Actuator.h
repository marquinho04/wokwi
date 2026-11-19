/*
  Actuator.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Abstract base class for output devices. An actuator executes a command
  (template method executeCommand) and optionally propagates it to a parent
  handler for monitoring.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "CommandHandler.h"

/**
 * @brief Base class for actuators that respond to commands.
 */
class Actuator : public CommandHandler {
protected:
    int pin;                        ///< GPIO pin assigned to the actuator.
    CommandHandler* commandHandler; ///< Optional parent handler for propagated commands.

    /**
     * @brief Template-method hook with the hardware-specific behavior.
     * @param command Command being executed.
     */
    virtual void executeCommand(Command command) = 0;

public:
    /**
     * @brief Constructs an Actuator.
     * @param signalPin GPIO pin for the actuator.
     * @param parentHandler Optional parent handler (default: nullptr).
     */
    explicit Actuator(int signalPin, CommandHandler* parentHandler = nullptr);

    /**
     * @brief Executes the command and forwards it to the parent handler.
     */
    void handle(Command command) override;

    /**
     * @brief Sets or replaces the parent handler.
     */
    void setHandler(CommandHandler* parentHandler);
};

#endif // ACTUATOR_H
