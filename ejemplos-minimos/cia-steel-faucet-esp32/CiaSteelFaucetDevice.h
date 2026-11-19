/*
  CiaSteelFaucetDevice.h
  Cia Steel Faucet - Embedded Application (Modest IoT based)

  Touchless faucet: hands closer than 10 cm turn the blue LED on and open
  the water valve (relay) for 5 s. Distance and valve state are printed
  every 2.5 s.

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
    // Pins (ESP32), identifiers and business rules
    static const int TRIGGER_PIN = 5, ECHO_PIN = 18, LED_PIN = 4, RELAY_PIN = 23;
    static const int PROXIMITY_SENSOR_ID = 1;
    static const int THRESHOLD_IN_CENTIMETERS = 10;
    static const unsigned long SAMPLING_MS = 200, REPORT_MS = 2500, VALVE_OPEN_MS = 5000;

    // Components
    UltrasonicSensor proximitySensor{PROXIMITY_SENSOR_ID, TRIGGER_PIN, ECHO_PIN, this};
    Led proximityLed{LED_PIN};
    Relay valveRelay{RELAY_PIN};
    PeriodicTimer samplingTimer{SAMPLING_MS};
    PeriodicTimer reportTimer{REPORT_MS};
    OneShotTimer valveTimer{VALVE_OPEN_MS};
    bool sensorFault = false;

public:
    void setup() override {
        Serial.begin(115200);
        Serial.println(F("Moen, inc. - Cia Steel Faucet | Developer: <Developer Full Name>"));
        proximitySensor.begin();
        proximityLed.begin();
        valveRelay.begin();
    }

    void loop() override {
        if (samplingTimer.isDue()) proximitySensor.measure();
        if (valveTimer.hasExpired()) {
            valveRelay.setState(false);
            Serial.println(F("[VALVE] CLOSED"));
        }
        if (reportTimer.isDue()) {
            Serial.print(F("[REPORT] Distance: "));
            Serial.print(proximitySensor.getValue(), 1);
            Serial.println(valveRelay.isOn() ? F(" cm | Valve: OPEN") : F(" cm | Valve: CLOSED"));
        }
    }

    // Business rules: called every time the sensor reports
    void on(Event event) override {
        if (event.type == Event::FAULT) {          // Exception: no echo
            if (!sensorFault) Serial.println(F("[ERROR] Proximity sensor without echo"));
            sensorFault = true;
            proximityLed.setState(false);
            return;
        }
        sensorFault = false;
        bool handsNear = proximitySensor.getValue() < THRESHOLD_IN_CENTIMETERS;
        if (handsNear && !proximityLed.isOn()) {   // Hands just arrived
            valveRelay.setState(true);
            valveTimer.start();
            Serial.println(F("[VALVE] OPEN for 5 s"));
        }
        proximityLed.setState(handsNear);
    }
};

#endif // CIA_STEEL_FAUCET_DEVICE_H
