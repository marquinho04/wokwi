/*
  DhtSensor.h
  Generic IoT kit - Embedded Application (Modest IoT based)

  DHT22 temperature (value, in Celsius) and humidity (%) sensor.
  Requires the "DHT sensor library" in the Wokwi Library Manager.
  Reports Event::FAULT when the reading is not a number.

  Created 19 November 2025
  By <Developer Full Name>
*/

#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <DHT.h>
#include "ModestIoT.h"

class DhtSensor : public Sensor {
private:
    DHT dht;
    float humidity = 0.0f;

protected:
    int read() override {
        float temperature = dht.readTemperature();
        float relativeHumidity = dht.readHumidity();
        if (isnan(temperature) || isnan(relativeHumidity)) {
            return Event::FAULT;
        }
        value = temperature;
        humidity = relativeHumidity;
        return Event::MEASURED;
    }

public:
    DhtSensor(int sensorId, int dataPin, EventHandler* handler)
        : Sensor(sensorId, dataPin, handler), dht(dataPin, DHT22) {}

    bool begin() override {
        dht.begin();
        return true;
    }

    float getTemperatureInCelsius() const { return value; }
    float getHumidity() const { return humidity; }
};

#endif // DHT_SENSOR_H
