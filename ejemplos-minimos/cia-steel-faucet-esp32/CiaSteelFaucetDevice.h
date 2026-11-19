/*
  CiaSteelFaucetDevice.h
  Cia Steel Faucet - Embedded Application (Modest IoT based)

  Device (mediator) of the Cia Steel Faucet with MotionSense Wave.
  Business rules:
  * Hands closer than 10 cm -> blue LED on; farther -> LED off.
  * When the hands reach the threshold, the relay opens the water valve
    for 5 seconds and then closes it.
  * Every 2.5 seconds the console shows the distance (cm) and the valve
    state (OPEN / CLOSED).
  * At startup the console shows the company and the developer.

  The circuit (ESP32 DevKit V1):
  * HC-SR04 TRIG -> GPIO 5, ECHO -> GPIO 18
  * Blue LED (with 220 ohm resistor) -> GPIO 4
  * Relay module IN -> GPIO 23

  Created 19 November 2025
  By <Developer Full Name> - Moen, inc.
*/

// Include guard: prevents this header from being compiled twice
#ifndef CIA_STEEL_FAUCET_DEVICE_H
#define CIA_STEEL_FAUCET_DEVICE_H

#include "ModestIoT.h"         // Core: Device, Event, Command, timers
#include "UltrasonicSensor.h"  // HC-SR04 distance sensor
#include "DigitalActuator.h"   // Led and Relay actuators

/**
 * @brief Touchless faucet device: coordinates the proximity sensor,
 *        the LED indicator and the water valve relay.
 */
class CiaSteelFaucetDevice : public Device {
private:
    // ----- Pin configuration (ESP32 DevKit V1) -----
    static const int TRIGGER_PIN = 5, ECHO_PIN = 18, LED_PIN = 4, RELAY_PIN = 23;

    // ----- Sensor identifiers (tell on() which sensor reported) -----
    static const int PROXIMITY_SENSOR_ID = 1;

    // ----- Business rules taken from the case -----
    static const int THRESHOLD_IN_CENTIMETERS = 10;  // Hands detected below 10 cm
    static const unsigned long SAMPLING_MS = 200;    // Read the distance every 0.2 s
    static const unsigned long REPORT_MS = 2500;     // Console report every 2.5 s
    static const unsigned long VALVE_OPEN_MS = 5000; // Keep the valve open 5 s

    // ----- Components: each one is created here with its pins -----
    // "this" registers the device as the receiver of the sensor events
    UltrasonicSensor proximitySensor{PROXIMITY_SENSOR_ID, TRIGGER_PIN, ECHO_PIN, this};
    Led proximityLed{LED_PIN};               // Blue LED: hands detected
    Relay valveRelay{RELAY_PIN};             // Relay that opens the water valve
    PeriodicTimer samplingTimer{SAMPLING_MS}; // When to measure
    PeriodicTimer reportTimer{REPORT_MS};     // When to print the report
    OneShotTimer valveTimer{VALVE_OPEN_MS};   // How long the valve stays open

    // ----- State -----
    bool sensorFault = false; // True while the sensor receives no echo

public:
    /**
     * @brief Runs once at startup: console, company information and hardware setup.
     */
    void setup() override {
        Serial.begin(115200); // ESP32 serial console speed
        Serial.println(F("Moen, inc. - Cia Steel Faucet | Developer: <Developer Full Name>"));
        proximitySensor.begin(); // Configure TRIG as output and ECHO as input
        proximityLed.begin();    // Configure the LED pin and turn it off
        valveRelay.begin();      // Configure the relay pin and close the valve
    }

    /**
     * @brief Runs continuously: checks the timers without blocking (no delay()).
     */
    void loop() override {
        // 1. Measure periodically; the sensor then calls on() with the result
        if (samplingTimer.isDue()) proximitySensor.measure();

        // 2. Close the valve when its 5 seconds are over
        if (valveTimer.hasExpired()) {
            valveRelay.setState(false);
            Serial.println(F("[VALVE] CLOSED"));
        }

        // 3. Periodic report: distance and valve state
        if (reportTimer.isDue()) {
            Serial.print(F("[REPORT] Distance: "));
            Serial.print(proximitySensor.getValue(), 1); // One decimal
            Serial.println(valveRelay.isOn() ? F(" cm | Valve: OPEN") : F(" cm | Valve: CLOSED"));
        }
    }

    /**
     * @brief Business rules: called by the sensor every time it reports.
     * @param event Sensor event (MEASURED or FAULT).
     */
    void on(Event event) override {
        // Exception handling: no echo (out of range or disconnected sensor)
        if (event.type == Event::FAULT) {
            if (!sensorFault) Serial.println(F("[ERROR] Proximity sensor without echo")); // Print only once
            sensorFault = true;
            proximityLed.setState(false); // Safe state: no detection
            return;
        }
        sensorFault = false; // Valid reading again

        // Rule 1: hands are near when the distance is below the threshold
        bool handsNear = proximitySensor.getValue() < THRESHOLD_IN_CENTIMETERS;

        // Rule 2: open the valve only when the hands just arrived
        // (LED still off), so it does not reopen while the hands stay near
        if (handsNear && !proximityLed.isOn()) {
            valveRelay.setState(true);
            valveTimer.start(); // Start the 5-second countdown
            Serial.println(F("[VALVE] OPEN for 5 s"));
        }

        // Rule 1: the LED follows the detection (setState only acts on changes)
        proximityLed.setState(handsNear);
    }
};

#endif // CIA_STEEL_FAUCET_DEVICE_H
