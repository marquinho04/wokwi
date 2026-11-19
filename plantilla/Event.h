/*
  Event.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Lightweight event value object. Sensors emit events that the Device
  (mediator) receives through the EventHandler interface.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef EVENT_H
#define EVENT_H

/**
 * @brief Event descriptor used to route sensor notifications.
 */
class Event {
public:
    int identifier; ///< Event type discriminator.

    /**
     * @brief Creates an Event with the provided identifier.
     * @param eventIdentifier Event type discriminator.
     */
    constexpr explicit Event(int eventIdentifier) : identifier(eventIdentifier) {}

    /**
     * @brief Compares two events by identifier.
     */
    bool operator==(const Event& other) const { return identifier == other.identifier; }
};

#endif // EVENT_H
