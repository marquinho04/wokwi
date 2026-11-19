/*
  Led.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  LED actuator controlled through TURN_ON / TURN_OFF commands.
  Used for the overweight indicator (blue) and for the heart rate
  indicators (blue = low, green = normal, red = high).

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef LED_H
#define LED_H

#include "Actuator.h"

/**
 * @brief Digital output actuator that controls an LED state.
 */
class Led : public Actuator {
private:
    bool turnedOn; ///< Current logical state of the LED.

protected:
    void executeCommand(Command command) override;

public:
    static const int TURN_ON_COMMAND_IDENTIFIER = 100;  ///< Turn the LED on.
    static const int TURN_OFF_COMMAND_IDENTIFIER = 101; ///< Turn the LED off.
    static const Command TURN_ON_COMMAND;               ///< Prebuilt turn-on command.
    static const Command TURN_OFF_COMMAND;              ///< Prebuilt turn-off command.

    /**
     * @brief Constructs an LED actuator.
     * @param signalPin Output pin connected to the LED (through a resistor).
     * @param parentHandler Optional parent handler for propagated commands.
     */
    explicit Led(int signalPin, CommandHandler* parentHandler = nullptr);

    /**
     * @brief Configures the output pin and turns the LED off.
     */
    void begin();

    /**
     * @brief Turns the LED on or off, issuing the command only when the state changes.
     * @param shouldBeOn Desired LED state.
     */
    void setState(bool shouldBeOn);

    /**
     * @brief Reports the current LED state.
     * @return true when the LED is on.
     */
    bool isOn() const;
};

#endif // LED_H
