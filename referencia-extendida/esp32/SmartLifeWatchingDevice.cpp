/*
  SmartLifeWatchingDevice.cpp
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Implements the SmartLifeWatchingDevice (mediator of sensors and actuators).

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#include "SmartLifeWatchingDevice.h"

namespace {
// Developer information shown at startup (replace with your data)
const char DEVELOPER_FULL_NAME[] = "<Developer Full Name>";
const char DEVELOPER_STUDENT_CODE[] = "<u20XXXXXXX>";

// Calibration of the load cell (raw HX711 counts per kilogram).
// The patient platform uses four 50 kg load cells (200 kg capacity).
// Wokwi simulates the bridge with one HX711 "50kg" part whose raw output
// goes from 0 to 21000; 21000 counts / 200 kg = 105 counts per kg.
const float WEIGHT_CALIBRATION_FACTOR = 105.0f;
}

SmartLifeWatchingDevice::SmartLifeWatchingDevice()
    : weightSensor(WEIGHT_SENSOR_DATA_PIN, WEIGHT_SENSOR_CLOCK_PIN, WEIGHT_CALIBRATION_FACTOR, this),
      heartRateSensor(HEART_RATE_SENSOR_PIN, this),
      overweightLed(OVERWEIGHT_LED_PIN, this),
      lowHeartRateLed(LOW_HEART_RATE_LED_PIN, this),
      normalHeartRateLed(NORMAL_HEART_RATE_LED_PIN, this),
      highHeartRateLed(HIGH_HEART_RATE_LED_PIN, this),
      display(LCD_I2C_ADDRESS),
      operationMode(OperationMode::Initializing),
      heartRateStatus(HeartRateStatus::NoSignal),
      weightInKilograms(0.0f),
      beatsPerMinute(0),
      lastSamplingTimestamp(0),
      lastReportTimestamp(0) {}

void SmartLifeWatchingDevice::setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    printCompanyInformation();

    // Actuators
    overweightLed.begin();
    lowHeartRateLed.begin();
    normalHeartRateLed.begin();
    highHeartRateLed.begin();
    display.begin();

    display.setLine(0, "Smart LifeMonitor");
    display.setLine(1, "Smart LifeWatching");
    display.setLine(2, "Initializing...");
    display.handle(LcdDisplay::REFRESH_COMMAND);

    Serial.print(F("[MODE] "));
    Serial.println(toText(operationMode));

    // Sensors
    if (weightSensor.begin()) {
        changeOperationMode(OperationMode::Monitoring);
    } else {
        Serial.println(F("[ERROR] Scale (HX711) not responding at startup."));
        changeOperationMode(OperationMode::ScaleFault);
    }

    delay(SPLASH_DURATION_IN_MILLISECONDS);
    display.handle(LcdDisplay::CLEAR_COMMAND);
}

void SmartLifeWatchingDevice::loop() {
    unsigned long now = millis();

    // Non-blocking periodic sampling of both sensors
    if (now - lastSamplingTimestamp >= SAMPLING_INTERVAL_IN_MILLISECONDS) {
        lastSamplingTimestamp = now;
        weightSensor.on(Sensor::MEASURE_DATA_REQUESTED_EVENT);
        heartRateSensor.on(Sensor::MEASURE_DATA_REQUESTED_EVENT);
        updateDisplay();
    }

    // Periodic telemetry on the serial console
    if (now - lastReportTimestamp >= REPORT_INTERVAL_IN_MILLISECONDS) {
        lastReportTimestamp = now;
        printTelemetry();
    }
}

void SmartLifeWatchingDevice::on(Event event) {
    switch (event.identifier) {
        case WeightSensor::WEIGHT_MEASURED_EVENT_IDENTIFIER:
            if (operationMode != OperationMode::Monitoring) {
                Serial.println(F("[INFO] Scale responding again."));
                changeOperationMode(OperationMode::Monitoring);
            }
            updateWeight(weightSensor.getWeightInKilograms());
            break;

        case WeightSensor::SCALE_FAULT_EVENT_IDENTIFIER:
            if (operationMode != OperationMode::ScaleFault) {
                Serial.println(F("[ERROR] Scale (HX711) not responding."));
                changeOperationMode(OperationMode::ScaleFault);
            }
            overweightLed.setState(false); // No valid weight: indicator off
            break;

        case HeartRateSensor::HEART_RATE_MEASURED_EVENT_IDENTIFIER: {
            int measuredBeatsPerMinute = heartRateSensor.getBeatsPerMinute();
            updateHeartRate(HeartRateClassifier::classify(measuredBeatsPerMinute), measuredBeatsPerMinute);
            break;
        }

        case HeartRateSensor::NO_SIGNAL_EVENT_IDENTIFIER:
            updateHeartRate(HeartRateStatus::NoSignal, 0);
            break;

        default:
            break; // Unknown event: ignored
    }
}

void SmartLifeWatchingDevice::handle(Command command) {
    // Monitoring hook: LED actuators report every executed state change
    if (command == Led::TURN_ON_COMMAND) {
        Serial.println(F("  [ACTUATOR] LED turned ON"));
    } else if (command == Led::TURN_OFF_COMMAND) {
        Serial.println(F("  [ACTUATOR] LED turned OFF"));
    }
}

void SmartLifeWatchingDevice::printCompanyInformation() const {
    Serial.println(F("=================================================="));
    Serial.println(F(" Smart LifeMonitor, inc. (founded in 2015)"));
    Serial.println(F(" Vital signs monitoring for patients at risk"));
    Serial.println(F(" due to chronic diseases, using IoT technologies."));
    Serial.println(F(" Product line: LifeWatching"));
    Serial.println(F(" Device: Smart LifeWatching (weight + heart rate)"));
    Serial.print(F(" Developer: "));
    Serial.print(DEVELOPER_FULL_NAME);
    Serial.print(F(" - "));
    Serial.println(DEVELOPER_STUDENT_CODE);
    Serial.println(F(" Team member of Smart LifeMonitor, inc."));
    Serial.println(F("=================================================="));
}

void SmartLifeWatchingDevice::changeOperationMode(OperationMode newMode) {
    if (newMode == operationMode) {
        return;
    }
    Serial.print(F("[MODE] "));
    Serial.print(toText(operationMode));
    Serial.print(F(" -> "));
    Serial.println(toText(newMode));
    operationMode = newMode;
}

void SmartLifeWatchingDevice::updateWeight(float newWeightInKilograms) {
    bool wasOverweight = isOverweight();
    weightInKilograms = newWeightInKilograms;

    if (isOverweight() != wasOverweight) {
        Serial.print(F("[STATE] Weight "));
        Serial.print(weightInKilograms, 1);
        Serial.println(isOverweight() ? F(" kg > 80 kg: overweight LED ON")
                                      : F(" kg <= 80 kg: overweight LED OFF"));
    }
    overweightLed.setState(isOverweight());
}

void SmartLifeWatchingDevice::updateHeartRate(HeartRateStatus newStatus, int newBeatsPerMinute) {
    beatsPerMinute = newBeatsPerMinute;

    if (newStatus != heartRateStatus) {
        Serial.print(F("[STATE] Heart rate status: "));
        Serial.print(HeartRateClassifier::toText(heartRateStatus));
        Serial.print(F(" -> "));
        Serial.print(HeartRateClassifier::toText(newStatus));
        Serial.print(F(" ("));
        Serial.print(beatsPerMinute);
        Serial.println(F(" BPM)"));
        if (newStatus == HeartRateStatus::NoSignal) {
            Serial.println(F("[WARNING] No valid pulse: check the heart rate sensor."));
        }
        heartRateStatus = newStatus;
    }
    updateHeartRateIndicators();
}

void SmartLifeWatchingDevice::updateHeartRateIndicators() {
    // Exactly one LED on for a valid reading; all off when there is no signal
    lowHeartRateLed.setState(heartRateStatus == HeartRateStatus::Low);
    normalHeartRateLed.setState(heartRateStatus == HeartRateStatus::Normal);
    highHeartRateLed.setState(heartRateStatus == HeartRateStatus::High);
}

void SmartLifeWatchingDevice::updateDisplay() {
    char line[LcdDisplay::COLUMN_COUNT + 1];

    display.setLine(0, "Smart LifeWatching");

    if (operationMode == OperationMode::ScaleFault) {
        display.setLine(1, "Weight: SCALE ERROR");
    } else {
        char weightText[8];
        dtostrf(weightInKilograms, 5, 1, weightText);
        snprintf(line, sizeof(line), "Weight: %s kg%s", weightText, isOverweight() ? " >80" : "");
        display.setLine(1, line);
    }

    if (heartRateStatus == HeartRateStatus::NoSignal) {
        display.setLine(2, "Heart rate: --- BPM");
    } else {
        snprintf(line, sizeof(line), "Heart rate: %3d BPM", beatsPerMinute);
        display.setLine(2, line);
    }

    snprintf(line, sizeof(line), "Status: %s", HeartRateClassifier::toText(heartRateStatus));
    display.setLine(3, line);

    display.handle(LcdDisplay::REFRESH_COMMAND);
}

void SmartLifeWatchingDevice::printTelemetry() const {
    Serial.print(F("[TELEMETRY] Mode: "));
    Serial.print(toText(operationMode));
    Serial.print(F(" | Weight: "));
    if (operationMode == OperationMode::ScaleFault) {
        Serial.print(F("N/A"));
    } else {
        Serial.print(weightInKilograms, 1);
        Serial.print(F(" kg"));
    }
    Serial.print(F(" | Overweight LED: "));
    Serial.print(overweightLed.isOn() ? F("ON") : F("OFF"));
    Serial.print(F(" | Heart rate: "));
    Serial.print(beatsPerMinute);
    Serial.print(F(" BPM | Status: "));
    Serial.println(HeartRateClassifier::toText(heartRateStatus));
}

bool SmartLifeWatchingDevice::isOverweight() const {
    return operationMode != OperationMode::ScaleFault
           && weightInKilograms > OVERWEIGHT_THRESHOLD_IN_KILOGRAMS;
}

const char* SmartLifeWatchingDevice::toText(OperationMode mode) {
    switch (mode) {
        case OperationMode::Initializing: return "INITIALIZING";
        case OperationMode::Monitoring:   return "MONITORING";
        default:                          return "SCALE FAULT";
    }
}
