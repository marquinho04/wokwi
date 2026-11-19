/*
  Command.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Lightweight command value object. The Device (mediator) issues commands
  that actuators execute through the CommandHandler interface (CQRS-inspired).

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef COMMAND_H
#define COMMAND_H

/**
 * @brief Command descriptor used to route actuator actions.
 */
class Command {
public:
    int identifier; ///< Command type discriminator.

    /**
     * @brief Creates a Command with the provided identifier.
     * @param commandIdentifier Command type discriminator.
     */
    constexpr explicit Command(int commandIdentifier) : identifier(commandIdentifier) {}

    /**
     * @brief Compares two commands by identifier.
     */
    bool operator==(const Command& other) const { return identifier == other.identifier; }
};

#endif // COMMAND_H
