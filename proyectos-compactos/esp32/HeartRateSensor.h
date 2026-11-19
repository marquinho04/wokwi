/*
  HeartRateSensor.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Heart rate sensor in beats per minute (BPM). Wokwi has no pulse sensor,
  so it is simulated with a potentiometer: full range -> 0..200 BPM.
  Below 30 BPM (physiological minimum) it reports "no signal".
  Also defines the heart rate status rules: < 60 LOW, 60-100 NORMAL, > 100 HIGH.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef HEART_RATE_SENSOR_H
#define HEART_RATE_SENSOR_H

#include <Arduino.h>
#include "ModestIoT.h"

enum class HeartRateStatus { NoSignal, Low, Normal, High };

class HeartRateSensor : public Sensor {
private:
    static const int MAXIMUM_ANALOG_VALUE = 4095;  // 12-bit ADC (ESP32)
    static const int MAXIMUM_BEATS_PER_MINUTE = 200;
    static const int MINIMUM_VALID_BEATS_PER_MINUTE = 30;
    static const int NORMAL_MINIMUM_BEATS_PER_MINUTE = 60;
    static const int NORMAL_MAXIMUM_BEATS_PER_MINUTE = 100;
    int beatsPerMinute = 0;

protected:
    int read() override {
        int measured = map(analogRead(pin), 0, MAXIMUM_ANALOG_VALUE, 0, MAXIMUM_BEATS_PER_MINUTE);
        if (measured < MINIMUM_VALID_BEATS_PER_MINUTE) {
            beatsPerMinute = 0;
            return NO_SIGNAL_EVENT; // Exception: sensor not placed or disconnected
        }
        beatsPerMinute = measured;
        return HEART_RATE_MEASURED_EVENT;
    }

public:
    static const int HEART_RATE_MEASURED_EVENT = 40;
    static const int NO_SIGNAL_EVENT = 41;

    HeartRateSensor(int analogPin, EventHandler* handler) : Sensor(analogPin, handler) {}

    int getBeatsPerMinute() const { return beatsPerMinute; }

    /** Classifies a valid reading (limits 60 and 100 are NORMAL). */
    static HeartRateStatus classify(int beats) {
        if (beats < NORMAL_MINIMUM_BEATS_PER_MINUTE) return HeartRateStatus::Low;
        if (beats > NORMAL_MAXIMUM_BEATS_PER_MINUTE) return HeartRateStatus::High;
        return HeartRateStatus::Normal;
    }

    static const char* toText(HeartRateStatus status) {
        switch (status) {
            case HeartRateStatus::Low:    return "LOW";
            case HeartRateStatus::Normal: return "NORMAL";
            case HeartRateStatus::High:   return "HIGH";
            default:                      return "NO SIGNAL";
        }
    }
};

#endif // HEART_RATE_SENSOR_H
