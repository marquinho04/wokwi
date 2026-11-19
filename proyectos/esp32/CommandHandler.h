/*
  CommandHandler.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Abstract interface for components that execute Command objects.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "Command.h"

/**
 * @brief Interface for command-driven components.
 */
class CommandHandler {
public:
    /**
     * @brief Processes an incoming command.
     * @param command Command instance to execute.
     */
    virtual void handle(Command command) = 0;

    virtual ~CommandHandler() {}
};

#endif // COMMAND_HANDLER_H
