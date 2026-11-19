/*
  SmartLifeWatchingDevice.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Weight > 80 kg turns the blue LED on. Heart rate < 60 LOW (blue LED),
  60-100 NORMAL (green), > 100 HIGH (red). The LCD 2004 shows weight,
  BPM and status. Heart rate sensor simulated with a potentiometer
  (0-200 BPM); below 30 BPM means "no signal".

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef SMART_LIFE_WATCHING_DEVICE_H
#define SMART_LIFE_WATCHING_DEVICE_H

#include "ModestIoT.h"
#include "AnalogSensor.h"
#include "WeightSensor.h"
#include "DigitalActuator.h"
#include "LcdDisplay.h"

class SmartLifeWatchingDevice : public Device {
private:
    // Pins (Arduino Uno), identifiers and business rules
    static const int SCALE_DATA_PIN = 3, SCALE_CLOCK_PIN = 2, HEART_RATE_PIN = A0;
    static const int OVERWEIGHT_LED_PIN = 8, LOW_LED_PIN = 9, NORMAL_LED_PIN = 10, HIGH_LED_PIN = 11;
    static const int WEIGHT_SENSOR_ID = 1, HEART_RATE_SENSOR_ID = 2;
    static const int OVERWEIGHT_KILOGRAMS = 80, MINIMUM_VALID_BPM = 30, MAXIMUM_BPM = 200;
    static const unsigned long SAMPLING_MS = 500;

    // Components
    WeightSensor weightSensor{WEIGHT_SENSOR_ID, SCALE_DATA_PIN, SCALE_CLOCK_PIN, 105.0f, this}; // 105 counts/kg
    AnalogSensor heartRateSensor{HEART_RATE_SENSOR_ID, HEART_RATE_PIN, 0, MAXIMUM_BPM, this};
    Led overweightLed{OVERWEIGHT_LED_PIN}, lowLed{LOW_LED_PIN}, normalLed{NORMAL_LED_PIN}, highLed{HIGH_LED_PIN};
    LcdDisplay display{0x27, 20, 4};                  // I2C address 0x27, 20 columns x 4 rows
    PeriodicTimer samplingTimer{SAMPLING_MS};
    LevelClassifier heartRateClassifier{60, 100};     // < 60 LOW, 60-100 NORMAL, > 100 HIGH

    void showHeartRateStatus(const char* status) {
        char line[21];
        snprintf(line, sizeof(line), "Status: %s", status);
        display.setLine(2, line);
        Serial.print(F("[STATE] Heart rate: "));
        Serial.println(status);
    }

public:
    void setup() override {
        Serial.begin(9600);
        Serial.println(F("Smart LifeMonitor, inc. | Developer: <Developer Full Name>"));
        overweightLed.begin(); lowLed.begin(); normalLed.begin(); highLed.begin();
        display.begin();
        display.setLine(3, "Smart LifeWatching");
        heartRateSensor.setValidRange(MINIMUM_VALID_BPM, MAXIMUM_BPM);
        if (!weightSensor.begin()) Serial.println(F("[ERROR] Scale (HX711) not responding"));
    }

    void loop() override {
        if (samplingTimer.isDue()) {
            weightSensor.measure();
            heartRateSensor.measure();
            display.handle(Command(Command::REFRESH));
        }
    }

    // Business rules: called every time a sensor reports
    void on(Event event) override {
        char line[21];
        if (event.sourceId == WEIGHT_SENSOR_ID) {
            if (event.type == Event::FAULT) {               // Exception: scale not responding
                display.setLine(0, "Weight: SCALE ERROR");
                overweightLed.setState(false);
                return;
            }
            char kilograms[8];
            dtostrf(weightSensor.getValue(), 5, 1, kilograms);
            snprintf(line, sizeof(line), "Weight: %s kg", kilograms);
            display.setLine(0, line);
            overweightLed.setState(weightSensor.getValue() > OVERWEIGHT_KILOGRAMS);
        } else if (event.sourceId == HEART_RATE_SENSOR_ID) {
            if (event.type == Event::FAULT) {               // Exception: no pulse signal
                display.setLine(1, "Heart rate: --- BPM");
                if (lowLed.isOn() || normalLed.isOn() || highLed.isOn()) showHeartRateStatus("NO SIGNAL");
                lowLed.setState(false); normalLed.setState(false); highLed.setState(false);
                return;
            }
            int bpm = (int) (heartRateSensor.getValue() + 0.5f);
            Level level = heartRateClassifier.classify(bpm);
            snprintf(line, sizeof(line), "Heart rate: %d BPM", bpm);
            display.setLine(1, line);
            bool changed = (level == Level::Low) != lowLed.isOn()
                        || (level == Level::Normal) != normalLed.isOn()
                        || (level == Level::High) != highLed.isOn();
            lowLed.setState(level == Level::Low);
            normalLed.setState(level == Level::Normal);
            highLed.setState(level == Level::High);
            if (changed) showHeartRateStatus(LevelClassifier::toText(level));
        }
    }
};

#endif // SMART_LIFE_WATCHING_DEVICE_H
