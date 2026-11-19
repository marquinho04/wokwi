/*
  CiaSteelFaucetDevice.h
  Cia Steel Faucet - Embedded Application (Modest IoT based)

  Touchless faucet (MotionSense Wave) built with the generic kit:
  * Hands closer than 10 cm -> blue LED on; farther -> LED off.
  * When the hands reach the threshold, the relay opens the water valve
    for 5 seconds and then closes it.
  * Every 2.5 s the console shows the distance (cm) and the valve state.

  The circuit (ESP32 DevKit V1):
  * HC-SR04 TRIG -> GPIO 5, ECHO -> GPIO 18
  * Blue LED -> GPIO 4
  * Relay module IN -> GPIO 23

  Created 19 November 2025
  By <Developer Full Name> - Moen, inc.
*/

#ifndef CIA_STEEL_FAUCET_DEVICE_H
#define CIA_STEEL_FAUCET_DEVICE_H

#include "ModestIoT.h"
#include "UltrasonicSensor.h"
#include "DigitalActuator.h"

class CiaSteelFaucetDevice : public Device {
private:
    // Pins (ESP32 DevKit V1)
    static const int TRIGGER_PIN = 5;
    static const int ECHO_PIN = 18;
    static const int PROXIMITY_LED_PIN = 4;
    static const int VALVE_RELAY_PIN = 23;

    // Sensor identifiers
    static const int PROXIMITY_SENSOR_ID = 1;

    // Business rules and configuration
    static const unsigned long SERIAL_BAUD_RATE = 115200;
    static const unsigned long SAMPLING_INTERVAL_IN_MILLISECONDS = 200;
    static const unsigned long REPORT_INTERVAL_IN_MILLISECONDS = 2500;
    static const unsigned long VALVE_OPEN_DURATION_IN_MILLISECONDS = 5000;
    static const int PROXIMITY_THRESHOLD_IN_CENTIMETERS = 10;

    UltrasonicSensor proximitySensor;
    Led proximityLed;
    Relay valveRelay;
    PeriodicTimer samplingTimer;
    PeriodicTimer reportTimer;
    OneShotTimer valveTimer;

    bool handsDetected = false;
    bool sensorFault = false;

    void printCompanyInformation() {
        Serial.println(F("=============================================="));
        Serial.println(F(" Moen, inc. - Smart Water Network"));
        Serial.println(F(" Cia Steel Faucet with MotionSense Wave"));
        Serial.println(F(" Developer: <Developer Full Name> - <u20XXXXXXX>"));
        Serial.println(F("=============================================="));
    }

    const char* valveStateText() const {
        return valveRelay.isOn() ? "OPEN" : "CLOSED";
    }

    void openValve() {
        valveRelay.setState(true);
        valveTimer.start();
        Serial.println(F("[VALVE] OPEN for 5 s"));
    }

    void closeValve() {
        valveRelay.setState(false);
        valveTimer.stop();
        Serial.println(F("[VALVE] CLOSED"));
    }

    void updateProximity(bool detected) {
        if (detected == handsDetected) {
            return;
        }
        handsDetected = detected;
        proximityLed.setState(handsDetected);
        Serial.println(handsDetected ? F("[STATE] Hands detected (< 10 cm)")
                                     : F("[STATE] Hands removed"));
        if (handsDetected && !valveRelay.isOn()) {
            openValve(); // Opens once each time the threshold is reached
        }
    }

    void printReport() {
        Serial.print(F("[REPORT] Proximity: "));
        if (sensorFault) {
            Serial.print(F("N/A (sensor fault)"));
        } else {
            Serial.print(proximitySensor.getValue(), 1);
            Serial.print(F(" cm"));
        }
        Serial.print(F(" | Valve: "));
        Serial.println(valveStateText());
    }

public:
    CiaSteelFaucetDevice()
        : proximitySensor(PROXIMITY_SENSOR_ID, TRIGGER_PIN, ECHO_PIN, this),
          proximityLed(PROXIMITY_LED_PIN),
          valveRelay(VALVE_RELAY_PIN),
          samplingTimer(SAMPLING_INTERVAL_IN_MILLISECONDS),
          reportTimer(REPORT_INTERVAL_IN_MILLISECONDS),
          valveTimer(VALVE_OPEN_DURATION_IN_MILLISECONDS) {}

    void setup() override {
        Serial.begin(SERIAL_BAUD_RATE);
        printCompanyInformation();
        proximitySensor.begin();
        proximityLed.begin();
        valveRelay.begin();
        Serial.println(F("[MODE] ACTIVE: waiting for hands"));
    }

    void loop() override {
        if (samplingTimer.isDue()) {
            proximitySensor.measure();
        }
        if (valveTimer.hasExpired()) {
            closeValve();
        }
        if (reportTimer.isDue()) {
            printReport();
        }
    }

    /** Business rules: one branch per sensor and event type. */
    void on(Event event) override {
        switch (event.sourceId) {
            case PROXIMITY_SENSOR_ID:
                if (event.type == Event::FAULT) {
                    if (!sensorFault) {
                        Serial.println(F("[ERROR] Proximity sensor without echo"));
                    }
                    sensorFault = true;
                    updateProximity(false);
                } else {
                    sensorFault = false;
                    updateProximity(proximitySensor.getValue() < PROXIMITY_THRESHOLD_IN_CENTIMETERS);
                }
                break;

            default:
                break; // Unknown sensor: ignored
        }
    }
};

#endif // CIA_STEEL_FAUCET_DEVICE_H
