/*
  EventHandler.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Abstract interface for components that react to Event objects.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "Event.h"

/**
 * @brief Interface for event-driven components.
 */
class EventHandler {
public:
    /**
     * @brief Handles an incoming event.
     * @param event Event instance to process.
     */
    virtual void on(Event event) = 0;

    virtual ~EventHandler() {}
};

#endif // EVENT_HANDLER_H
