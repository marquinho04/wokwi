/*
  SmartLifeWatchingDevice.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Concrete Device (mediator) of the Smart LifeWatching prototype.
  It owns the sensors (weight, heart rate) and actuators (LEDs, LCD 2004),
  receives sensor events, applies the business rules and issues commands:
  * Weight greater than 80 kg -> overweight LED (blue) on.
  * Heart rate < 60 BPM -> blue LED (LOW), 60..100 -> green LED (NORMAL),
    > 100 -> red LED (HIGH).
  * LCD shows weight, heart rate in BPM and the measurement status.

  The circuit (Arduino Uno):
  * HX711 DT -> D3, HX711 SCK -> D2
  * Heart rate sensor (potentiometer) SIG -> A0
  * Overweight LED (blue) -> D8
  * Low (blue) / Normal (green) / High (red) heart rate LEDs -> D9 / D10 / D11
  * LCD 2004 I2C: SDA -> A4, SCL -> A5

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef SMART_LIFE_WATCHING_DEVICE_H
#define SMART_LIFE_WATCHING_DEVICE_H

#include <Arduino.h>
#include "Device.h"
#include "WeightSensor.h"
#include "HeartRateSensor.h"
#include "HeartRateStatus.h"
#include "Led.h"
#include "LcdDisplay.h"

/**
 * @brief Operation modes of the device.
 */
enum class OperationMode {
    Initializing,
    Monitoring,
    ScaleFault
};

/**
 * @brief Smart LifeWatching device: weight and heart rate monitoring with LED signaling.
 */
class SmartLifeWatchingDevice : public Device {
public:
    /** @name Pin configuration (Arduino Uno) */
    //@{
    static const int WEIGHT_SENSOR_DATA_PIN = 3;
    static const int WEIGHT_SENSOR_CLOCK_PIN = 2;
    static const int HEART_RATE_SENSOR_PIN = A0;
    static const int OVERWEIGHT_LED_PIN = 8;
    static const int LOW_HEART_RATE_LED_PIN = 9;
    static const int NORMAL_HEART_RATE_LED_PIN = 10;
    static const int HIGH_HEART_RATE_LED_PIN = 11;
    static const unsigned char LCD_I2C_ADDRESS = 0x27;
    //@}

    SmartLifeWatchingDevice();

    void setup() override;
    void loop() override;

    /**
     * @brief Receives sensor events and applies the business rules.
     */
    void on(Event event) override;

    /**
     * @brief Receives the commands executed by the LED actuators (monitoring hook).
     */
    void handle(Command command) override;

private:
    WeightSensor weightSensor;
    HeartRateSensor heartRateSensor;
    Led overweightLed;
    Led lowHeartRateLed;
    Led normalHeartRateLed;
    Led highHeartRateLed;
    LcdDisplay display;

    OperationMode operationMode;
    HeartRateStatus heartRateStatus;
    float weightInKilograms;
    int beatsPerMinute;
    unsigned long lastSamplingTimestamp;
    unsigned long lastReportTimestamp;

    static const unsigned long SERIAL_BAUD_RATE = 9600;
    static const unsigned long SAMPLING_INTERVAL_IN_MILLISECONDS = 500;
    static const unsigned long REPORT_INTERVAL_IN_MILLISECONDS = 2000;
    static const unsigned long SPLASH_DURATION_IN_MILLISECONDS = 1500;
    static const int OVERWEIGHT_THRESHOLD_IN_KILOGRAMS = 80;

    void printCompanyInformation() const;
    void changeOperationMode(OperationMode newMode);
    void updateWeight(float newWeightInKilograms);
    void updateHeartRate(HeartRateStatus newStatus, int newBeatsPerMinute);
    void updateHeartRateIndicators();
    void updateDisplay();
    void printTelemetry() const;
    bool isOverweight() const;
    static const char* toText(OperationMode mode);
};

#endif // SMART_LIFE_WATCHING_DEVICE_H
