/*
  SmartLifeWatchingDevice.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Device (mediator) of the Smart LifeWatching.
  Business rules:
  * Weight greater than 80 kg -> blue overweight LED on.
  * Heart rate: less than 60 BPM LOW (blue LED), 60 to 100 NORMAL
    (green LED), more than 100 HIGH (red LED).
  * The LCD 2004 shows the weight, the heart rate (BPM) and the status.
  * At startup the console shows the company and the developer; status
    changes are also printed.
  Exceptions: scale (HX711) not responding -> "SCALE ERROR"; heart rate
  below 30 BPM -> "NO SIGNAL" (sensor not placed or disconnected).
  The heart rate sensor is simulated with a potentiometer (0-200 BPM)
  because Wokwi has no pulse sensor.

  The circuit (Arduino Uno):
  * HX711 DT -> D3, SCK -> D2
  * Heart rate sensor (potentiometer) SIG -> A0
  * Overweight LED -> D8; low / normal / high LEDs -> D9 / D10 / D11
  * LCD 2004 I2C (0x27): SDA -> A4, SCL -> A5

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

// Include guard: prevents this header from being compiled twice
#ifndef SMART_LIFE_WATCHING_DEVICE_H
#define SMART_LIFE_WATCHING_DEVICE_H

#include "ModestIoT.h"        // Core: Device, Event, Command, timers, LevelClassifier
#include "AnalogSensor.h"     // Heart rate sensor (potentiometer)
#include "WeightSensor.h"     // Load cell + HX711
#include "DigitalActuator.h"  // LEDs
#include "LcdDisplay.h"       // LCD 2004 (I2C)

/**
 * @brief Smart LifeWatching device: coordinates the weight and heart rate
 *        sensors with the LED indicators and the LCD.
 */
class SmartLifeWatchingDevice : public Device {
private:
    // ----- Pin configuration (Arduino Uno) -----
    static const int SCALE_DATA_PIN = 3, SCALE_CLOCK_PIN = 2, HEART_RATE_PIN = A0;
    static const int OVERWEIGHT_LED_PIN = 8, LOW_LED_PIN = 9, NORMAL_LED_PIN = 10, HIGH_LED_PIN = 11;

    // ----- Sensor identifiers (tell on() which sensor reported) -----
    static const int WEIGHT_SENSOR_ID = 1, HEART_RATE_SENSOR_ID = 2;

    // ----- Business rules taken from the case -----
    static const int OVERWEIGHT_KILOGRAMS = 80;   // Overweight LED above this weight
    static const int MINIMUM_VALID_BPM = 30;      // Below this: no pulse signal
    static const int MAXIMUM_BPM = 200;           // Full range of the potentiometer
    static const unsigned long SAMPLING_MS = 500; // Read the sensors every 0.5 s

    // ----- Components: each one is created here with its pins -----
    // "this" registers the device as the receiver of the sensor events
    WeightSensor weightSensor{WEIGHT_SENSOR_ID, SCALE_DATA_PIN, SCALE_CLOCK_PIN, 105.0f, this}; // 105 counts/kg (200 kg platform)
    AnalogSensor heartRateSensor{HEART_RATE_SENSOR_ID, HEART_RATE_PIN, 0, MAXIMUM_BPM, this}; // 0..200 BPM
    Led overweightLed{OVERWEIGHT_LED_PIN}, lowLed{LOW_LED_PIN}, normalLed{NORMAL_LED_PIN}, highLed{HIGH_LED_PIN};
    LcdDisplay display{0x27, 20, 4};                  // I2C address 0x27, 20 columns x 4 rows
    PeriodicTimer samplingTimer{SAMPLING_MS};         // When to measure
    LevelClassifier heartRateClassifier{60, 100};     // < 60 LOW, 60-100 NORMAL, > 100 HIGH

    /**
     * @brief Shows the heart rate status on the LCD and prints the change on the console.
     * @param status Status text (LOW, NORMAL, HIGH or NO SIGNAL).
     */
    void showHeartRateStatus(const char* status) {
        char line[21];                                     // One LCD row (20 chars + end of text)
        snprintf(line, sizeof(line), "Status: %s", status); // Build the text safely
        display.setLine(2, line);                          // Row 3 of the LCD
        Serial.print(F("[STATE] Heart rate: "));
        Serial.println(status);
    }

public:
    /**
     * @brief Runs once at startup: console, company information and hardware setup.
     */
    void setup() override {
        Serial.begin(9600); // Arduino Uno serial console speed
        Serial.println(F("Smart LifeMonitor, inc. | Developer: <Developer Full Name>"));
        overweightLed.begin(); lowLed.begin(); normalLed.begin(); highLed.begin(); // LED pins as outputs, off
        display.begin();                                  // Turn on the LCD and its backlight
        display.setLine(3, "Smart LifeWatching");         // Device name on the last row
        heartRateSensor.setValidRange(MINIMUM_VALID_BPM, MAXIMUM_BPM); // < 30 BPM reports FAULT
        if (!weightSensor.begin()) Serial.println(F("[ERROR] Scale (HX711) not responding")); // Start and tare
    }

    /**
     * @brief Runs continuously: measures periodically without blocking (no delay()).
     */
    void loop() override {
        if (samplingTimer.isDue()) {
            weightSensor.measure();                    // Calls on() with the weight result
            heartRateSensor.measure();                 // Calls on() with the heart rate result
            display.handle(Command(Command::REFRESH)); // Write the changed rows on the LCD
        }
    }

    /**
     * @brief Business rules: called by a sensor every time it reports.
     * @param event Sensor event (source sensor and MEASURED or FAULT).
     */
    void on(Event event) override {
        char line[21]; // Buffer for one LCD row

        // ----- Weight sensor -----
        if (event.sourceId == WEIGHT_SENSOR_ID) {
            if (event.type == Event::FAULT) {               // Exception: scale not responding
                display.setLine(0, "Weight: SCALE ERROR");
                overweightLed.setState(false);              // Safe state: no valid weight
                return;
            }
            char kilograms[8];
            dtostrf(weightSensor.getValue(), 5, 1, kilograms); // Float to text (snprintf has no %f on Uno)
            snprintf(line, sizeof(line), "Weight: %s kg", kilograms);
            display.setLine(0, line);                           // Row 1 of the LCD
            // Rule: "greater than 80 kg" -> strictly greater (80.0 does not turn it on)
            overweightLed.setState(weightSensor.getValue() > OVERWEIGHT_KILOGRAMS);

        // ----- Heart rate sensor -----
        } else if (event.sourceId == HEART_RATE_SENSOR_ID) {
            if (event.type == Event::FAULT) {               // Exception: no pulse signal
                display.setLine(1, "Heart rate: --- BPM");
                // Print the status only if it changed (some LED was on)
                if (lowLed.isOn() || normalLed.isOn() || highLed.isOn()) showHeartRateStatus("NO SIGNAL");
                lowLed.setState(false); normalLed.setState(false); highLed.setState(false); // All off
                return;
            }
            int bpm = (int) (heartRateSensor.getValue() + 0.5f); // Round to whole beats
            Level level = heartRateClassifier.classify(bpm);     // LOW, NORMAL or HIGH (60 and 100 are NORMAL)
            snprintf(line, sizeof(line), "Heart rate: %d BPM", bpm);
            display.setLine(1, line);                            // Row 2 of the LCD

            // The status changed if the LED of the new level is not the one currently on
            bool changed = (level == Level::Low) != lowLed.isOn()
                        || (level == Level::Normal) != normalLed.isOn()
                        || (level == Level::High) != highLed.isOn();

            // Exactly one LED on, the one of the current level
            lowLed.setState(level == Level::Low);
            normalLed.setState(level == Level::Normal);
            highLed.setState(level == Level::High);
            if (changed) showHeartRateStatus(LevelClassifier::toText(level));
        }
    }
};

#endif // SMART_LIFE_WATCHING_DEVICE_H
