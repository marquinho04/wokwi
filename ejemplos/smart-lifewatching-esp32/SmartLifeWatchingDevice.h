/*
  SmartLifeWatchingDevice.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Device (mediator) built with the generic kit:
  * Weight > 80 kg -> overweight LED (blue) on.
  * Heart rate < 60 LOW (blue LED), 60-100 NORMAL (green), > 100 HIGH (red).
  * LCD 2004 shows weight, BPM and status.
  * The heart rate sensor is simulated with a potentiometer (0..200 BPM);
    below 30 BPM it is treated as "no signal".

  The circuit (ESP32 DevKit V1):
  * HX711 DT -> GPIO 19, SCK -> GPIO 18
  * Heart rate sensor (potentiometer) SIG -> GPIO 34
  * Overweight LED -> GPIO 4; low / normal / high LEDs -> GPIO 25 / 26 / 27
  * LCD 2004 I2C (0x27): SDA -> GPIO 21, SCL -> GPIO 22

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
    // Pins (ESP32 DevKit V1)
    static const int WEIGHT_DATA_PIN = 19;
    static const int WEIGHT_CLOCK_PIN = 18;
    static const int HEART_RATE_PIN = 34;  // ADC1, input only
    static const int OVERWEIGHT_LED_PIN = 4;
    static const int LOW_LED_PIN = 25;
    static const int NORMAL_LED_PIN = 26;
    static const int HIGH_LED_PIN = 27;
    static const unsigned char LCD_ADDRESS = 0x27;
    static const int LCD_COLUMNS = 20;
    static const int LCD_ROWS = 4;

    // Sensor identifiers
    static const int WEIGHT_SENSOR_ID = 1;
    static const int HEART_RATE_SENSOR_ID = 2;

    // Business rules and configuration
    static const unsigned long SERIAL_BAUD_RATE = 115200;
    static const unsigned long SAMPLING_INTERVAL_IN_MILLISECONDS = 500;
    static const int OVERWEIGHT_THRESHOLD_IN_KILOGRAMS = 80;
    static const int NORMAL_MINIMUM_BEATS_PER_MINUTE = 60;
    static const int NORMAL_MAXIMUM_BEATS_PER_MINUTE = 100;
    static const int MINIMUM_VALID_BEATS_PER_MINUTE = 30;
    static const int MAXIMUM_BEATS_PER_MINUTE = 200;
    static constexpr float WEIGHT_CALIBRATION_FACTOR = 105.0f;

    WeightSensor weightSensor;
    AnalogSensor heartRateSensor;
    Led overweightLed;
    Led lowLed;
    Led normalLed;
    Led highLed;
    LcdDisplay display;
    PeriodicTimer samplingTimer;
    LevelClassifier heartRateClassifier;

    bool scaleFault = false;
    bool heartRateSignal = false;
    Level heartRateLevel = Level::Normal;

    void printCompanyInformation() {
        Serial.println(F("=============================================="));
        Serial.println(F(" Smart LifeMonitor, inc. (founded in 2015)"));
        Serial.println(F(" Vital signs monitoring for chronic-disease"));
        Serial.println(F(" risk patients with IoT. Line: LifeWatching"));
        Serial.println(F(" Developer: <Developer Full Name> - <u20XXXXXXX>"));
        Serial.println(F("=============================================="));
    }

    /** Heart rate rounded to whole beats, used for both rules and display. */
    int beatsPerMinute() const {
        return (int) (heartRateSensor.getValue() + 0.5f);
    }

    const char* heartRateStatusText() const {
        return heartRateSignal ? LevelClassifier::toText(heartRateLevel) : "NO SIGNAL";
    }

    bool isOverweight() const {
        return !scaleFault && weightSensor.getValue() > OVERWEIGHT_THRESHOLD_IN_KILOGRAMS;
    }

    void updateHeartRate(bool hasSignal, Level newLevel) {
        if (hasSignal != heartRateSignal || newLevel != heartRateLevel) {
            heartRateSignal = hasSignal;
            heartRateLevel = newLevel;
            Serial.print(F("[STATE] Heart rate status: "));
            Serial.println(heartRateStatusText());
        }
        lowLed.setState(heartRateSignal && heartRateLevel == Level::Low);
        normalLed.setState(heartRateSignal && heartRateLevel == Level::Normal);
        highLed.setState(heartRateSignal && heartRateLevel == Level::High);
    }

    void updateDisplay() {
        char line[LcdDisplay::MAXIMUM_COLUMNS + 1];
        display.setLine(0, "Smart LifeWatching");

        if (scaleFault) {
            display.setLine(1, "Weight: SCALE ERROR");
        } else {
            char weightText[8];
            dtostrf(weightSensor.getValue(), 5, 1, weightText);
            snprintf(line, sizeof(line), "Weight: %s kg%s", weightText, isOverweight() ? " >80" : "");
            display.setLine(1, line);
        }

        if (heartRateSignal) {
            snprintf(line, sizeof(line), "Heart rate: %3d BPM", beatsPerMinute());
            display.setLine(2, line);
        } else {
            display.setLine(2, "Heart rate: --- BPM");
        }

        snprintf(line, sizeof(line), "Status: %s", heartRateStatusText());
        display.setLine(3, line);
        display.handle(Command(Command::REFRESH));
    }

public:
    SmartLifeWatchingDevice()
        : weightSensor(WEIGHT_SENSOR_ID, WEIGHT_DATA_PIN, WEIGHT_CLOCK_PIN, WEIGHT_CALIBRATION_FACTOR, this),
          heartRateSensor(HEART_RATE_SENSOR_ID, HEART_RATE_PIN, 0, MAXIMUM_BEATS_PER_MINUTE, this),
          overweightLed(OVERWEIGHT_LED_PIN),
          lowLed(LOW_LED_PIN),
          normalLed(NORMAL_LED_PIN),
          highLed(HIGH_LED_PIN),
          display(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS),
          samplingTimer(SAMPLING_INTERVAL_IN_MILLISECONDS),
          heartRateClassifier(NORMAL_MINIMUM_BEATS_PER_MINUTE, NORMAL_MAXIMUM_BEATS_PER_MINUTE) {}

    void setup() override {
        Serial.begin(SERIAL_BAUD_RATE);
        printCompanyInformation();

        overweightLed.begin();
        lowLed.begin();
        normalLed.begin();
        highLed.begin();
        display.begin();
        heartRateSensor.begin();
        heartRateSensor.setValidRange(MINIMUM_VALID_BEATS_PER_MINUTE, MAXIMUM_BEATS_PER_MINUTE);

        scaleFault = !weightSensor.begin();
        Serial.println(scaleFault ? F("[MODE] SCALE FAULT: HX711 not responding")
                                  : F("[MODE] MONITORING"));
    }

    void loop() override {
        if (samplingTimer.isDue()) {
            weightSensor.measure();
            heartRateSensor.measure();
            updateDisplay();
        }
    }

    /** Business rules: one branch per sensor and event type. */
    void on(Event event) override {
        switch (event.sourceId) {
            case WEIGHT_SENSOR_ID:
                if (event.type == Event::FAULT && !scaleFault) {
                    Serial.println(F("[MODE] SCALE FAULT: HX711 not responding"));
                } else if (event.type == Event::MEASURED && scaleFault) {
                    Serial.println(F("[MODE] MONITORING"));
                }
                scaleFault = (event.type == Event::FAULT);
                overweightLed.setState(isOverweight());
                break;

            case HEART_RATE_SENSOR_ID:
                if (event.type == Event::MEASURED) {
                    updateHeartRate(true, heartRateClassifier.classify(beatsPerMinute()));
                } else {
                    updateHeartRate(false, heartRateLevel);
                }
                break;

            default:
                break; // Unknown sensor: ignored
        }
    }
};

#endif // SMART_LIFE_WATCHING_DEVICE_H
