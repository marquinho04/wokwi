/*
  SmartLifeWatchingDevice.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Device (mediator): receives sensor events, applies the business rules
  and sends commands to the actuators.
  * Weight > 80 kg -> overweight LED (blue) on.
  * Heart rate < 60 LOW (blue), 60-100 NORMAL (green), > 100 HIGH (red).
  * LCD 2004 shows weight, BPM and status.

  The circuit (Arduino Uno):
  * HX711 DT -> D3, SCK -> D2
  * Heart rate sensor (potentiometer) SIG -> A0
  * Overweight LED -> D8; low / normal / high LEDs -> D9 / D10 / D11
  * LCD 2004 I2C (0x27): SDA -> A4, SCL -> A5

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef SMART_LIFE_WATCHING_DEVICE_H
#define SMART_LIFE_WATCHING_DEVICE_H

#include <Arduino.h>
#include "ModestIoT.h"
#include "WeightSensor.h"
#include "HeartRateSensor.h"
#include "Led.h"
#include "LcdDisplay.h"

enum class OperationMode { Initializing, Monitoring, ScaleFault };

class SmartLifeWatchingDevice : public Device {
private:
    // Pins (Arduino Uno)
    static const int WEIGHT_DATA_PIN = 3;
    static const int WEIGHT_CLOCK_PIN = 2;
    static const int HEART_RATE_PIN = A0;
    static const int OVERWEIGHT_LED_PIN = 8;
    static const int LOW_LED_PIN = 9;
    static const int NORMAL_LED_PIN = 10;
    static const int HIGH_LED_PIN = 11;
    static const unsigned char LCD_ADDRESS = 0x27;

    // Configuration
    static const unsigned long SERIAL_BAUD_RATE = 9600;
    static const unsigned long SAMPLING_INTERVAL_IN_MILLISECONDS = 500;
    static const int OVERWEIGHT_THRESHOLD_IN_KILOGRAMS = 80;
    static constexpr float WEIGHT_CALIBRATION_FACTOR = 105.0f; // counts per kg (see WeightSensor.h)

    WeightSensor weightSensor;
    HeartRateSensor heartRateSensor;
    Led overweightLed;
    Led lowLed;
    Led normalLed;
    Led highLed;
    LcdDisplay display;

    OperationMode mode = OperationMode::Initializing;
    HeartRateStatus status = HeartRateStatus::NoSignal;
    float weightInKilograms = 0.0f;
    int beatsPerMinute = 0;
    unsigned long lastSamplingTimestamp = 0;

    static const char* toText(OperationMode operationMode) {
        switch (operationMode) {
            case OperationMode::Initializing: return "INITIALIZING";
            case OperationMode::Monitoring:   return "MONITORING";
            default:                          return "SCALE FAULT";
        }
    }

    void printCompanyInformation() {
        Serial.println(F("=============================================="));
        Serial.println(F(" Smart LifeMonitor, inc. (founded in 2015)"));
        Serial.println(F(" Vital signs monitoring for chronic-disease"));
        Serial.println(F(" risk patients with IoT. Line: LifeWatching"));
        Serial.println(F(" Developer: <Developer Full Name> - <u20XXXXXXX>"));
        Serial.println(F("=============================================="));
    }

    void changeMode(OperationMode newMode) {
        if (newMode == mode) return;
        Serial.print(F("[MODE] "));
        Serial.print(toText(mode));
        Serial.print(F(" -> "));
        Serial.println(toText(newMode));
        mode = newMode;
    }

    bool isOverweight() const {
        return mode != OperationMode::ScaleFault
               && weightInKilograms > OVERWEIGHT_THRESHOLD_IN_KILOGRAMS;
    }

    void changeHeartRateStatus(HeartRateStatus newStatus) {
        if (newStatus != status) {
            Serial.print(F("[STATE] Heart rate: "));
            Serial.print(HeartRateSensor::toText(status));
            Serial.print(F(" -> "));
            Serial.println(HeartRateSensor::toText(newStatus));
            status = newStatus;
        }
        lowLed.setState(status == HeartRateStatus::Low);
        normalLed.setState(status == HeartRateStatus::Normal);
        highLed.setState(status == HeartRateStatus::High);
    }

    void updateDisplay() {
        char line[LcdDisplay::COLUMN_COUNT + 1];
        display.setLine(0, "Smart LifeWatching");

        if (mode == OperationMode::ScaleFault) {
            display.setLine(1, "Weight: SCALE ERROR");
        } else {
            char weightText[8];
            dtostrf(weightInKilograms, 5, 1, weightText);
            snprintf(line, sizeof(line), "Weight: %s kg%s", weightText, isOverweight() ? " >80" : "");
            display.setLine(1, line);
        }

        if (status == HeartRateStatus::NoSignal) {
            display.setLine(2, "Heart rate: --- BPM");
        } else {
            snprintf(line, sizeof(line), "Heart rate: %3d BPM", beatsPerMinute);
            display.setLine(2, line);
        }

        snprintf(line, sizeof(line), "Status: %s", HeartRateSensor::toText(status));
        display.setLine(3, line);
        display.handle(Command(LcdDisplay::REFRESH_COMMAND));
    }

public:
    SmartLifeWatchingDevice()
        : weightSensor(WEIGHT_DATA_PIN, WEIGHT_CLOCK_PIN, WEIGHT_CALIBRATION_FACTOR, this),
          heartRateSensor(HEART_RATE_PIN, this),
          overweightLed(OVERWEIGHT_LED_PIN),
          lowLed(LOW_LED_PIN),
          normalLed(NORMAL_LED_PIN),
          highLed(HIGH_LED_PIN),
          display(LCD_ADDRESS) {}

    void setup() override {
        Serial.begin(SERIAL_BAUD_RATE);
        printCompanyInformation();
        overweightLed.begin();
        lowLed.begin();
        normalLed.begin();
        highLed.begin();
        display.begin();

        if (weightSensor.begin()) {
            changeMode(OperationMode::Monitoring);
        } else {
            Serial.println(F("[ERROR] Scale (HX711) not responding."));
            changeMode(OperationMode::ScaleFault);
        }
    }

    void loop() override {
        unsigned long now = millis();
        if (now - lastSamplingTimestamp >= SAMPLING_INTERVAL_IN_MILLISECONDS) {
            lastSamplingTimestamp = now;
            weightSensor.measure();
            heartRateSensor.measure();
            updateDisplay();
        }
    }

    /** Business rules: one case per sensor event. */
    void on(Event event) override {
        switch (event.identifier) {
            case WeightSensor::WEIGHT_MEASURED_EVENT:
                changeMode(OperationMode::Monitoring);
                weightInKilograms = weightSensor.getWeightInKilograms();
                overweightLed.setState(isOverweight());
                break;
            case WeightSensor::SCALE_FAULT_EVENT:
                if (mode != OperationMode::ScaleFault) {
                    Serial.println(F("[ERROR] Scale (HX711) not responding."));
                }
                changeMode(OperationMode::ScaleFault);
                overweightLed.setState(false);
                break;
            case HeartRateSensor::HEART_RATE_MEASURED_EVENT:
                beatsPerMinute = heartRateSensor.getBeatsPerMinute();
                changeHeartRateStatus(HeartRateSensor::classify(beatsPerMinute));
                break;
            case HeartRateSensor::NO_SIGNAL_EVENT:
                beatsPerMinute = 0;
                changeHeartRateStatus(HeartRateStatus::NoSignal);
                break;
            default:
                break; // Unknown event: ignored
        }
    }
};

#endif // SMART_LIFE_WATCHING_DEVICE_H
