/*
  UltrasonicSensor.h
  Generic IoT kit - Embedded Application (Modest IoT based)

  HC-SR04 distance sensor. Value in centimeters. Reports Event::FAULT
  when no echo is received.

  Created 19 November 2025
  By <Developer Full Name>
*/

#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include "ModestIoT.h"

class UltrasonicSensor : public Sensor {
private:
    static const unsigned long ECHO_TIMEOUT_IN_MICROSECONDS = 30000; // about 5 m
    static constexpr float MICROSECONDS_PER_CENTIMETER = 58.0f;       // round trip
    int echoPin;

protected:
    int read() override {
        digitalWrite(pin, LOW);
        delayMicroseconds(2);
        digitalWrite(pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(pin, LOW);
        unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_IN_MICROSECONDS);
        if (duration == 0) {
            return Event::FAULT; // No echo: out of range or disconnected
        }
        value = duration / MICROSECONDS_PER_CENTIMETER;
        return Event::MEASURED;
    }

public:
    UltrasonicSensor(int sensorId, int triggerPin, int echoSignalPin, EventHandler* handler)
        : Sensor(sensorId, triggerPin, handler), echoPin(echoSignalPin) {}

    bool begin() override {
        pinMode(pin, OUTPUT);
        pinMode(echoPin, INPUT);
        return true;
    }
};

#endif // ULTRASONIC_SENSOR_H
