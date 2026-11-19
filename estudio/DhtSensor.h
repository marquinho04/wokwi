/*
  DhtSensor.h  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  Sensor DHT22: temperatura (°C) y humedad relativa (%).
  Usa la librería "DHT sensor library" de Adafruit (agrégala en el
  Library Manager de Wokwi). getValue() devuelve la temperatura;
  getHumidity() devuelve la humedad.
  En Wokwi cambias los valores haciendo clic en el DHT22 durante la simulación.
*/

#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <DHT.h>       // Librería externa: por eso este archivo necesita libraries.txt.
#include "ModestIoT.h"

class DhtSensor : public Sensor {
private:
    DHT dht;               // Objeto de la librería que habla con el sensor.
    float humidity = 0.0f; // Última humedad válida.

protected:
    int read() override {
        float temperature = dht.readTemperature();
        float relativeHumidity = dht.readHumidity();

        // Si la lectura falla, la librería devuelve NaN ("not a number").
        // isnan() lo detecta -> excepción.
        if (isnan(temperature) || isnan(relativeHumidity)) {
            return Event::FAULT;
        }
        value = temperature;
        humidity = relativeHumidity;
        return Event::MEASURED;
    }

public:
    // dht(dataPin, DHT22): crea el objeto de la librería con el pin y el modelo.
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
