/*
  AnalogSensor.h  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  Sirve para CUALQUIER sensor analógico: potenciómetro, fotorresistencia
  (LDR), gas, humedad de suelo, o un sensor que Wokwi no tiene y simulas
  con un potenciómetro (por ejemplo, el de pulso).

  Convierte la lectura cruda (0..1023 en Uno, 0..4095 en ESP32) a una
  unidad física. Ejemplo: AnalogSensor(ID, A0, 0, 200, this) convierte el
  recorrido completo del potenciómetro a 0..200 BPM.
*/

#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H

#include "ModestIoT.h"

// ": public Sensor" = hereda de Sensor: ya tiene id, pin, value, measure()...
class AnalogSensor : public Sensor {
private:
    float minimumValue;      // Valor físico cuando la lectura cruda es 0.
    float maximumValue;      // Valor físico cuando la lectura cruda es el máximo.
    float minimumValidValue; // Menos que esto = falla (ej.: < 30 BPM = sin señal).
    float maximumValidValue; // Más que esto = falla.

protected:
    // "override" = estoy escribiendo el read() que Sensor dejó pendiente.
    // Si me equivoco en el nombre o los parámetros, el compilador avisa.
    int read() override {
        int rawValue = analogRead(pin); // Lectura cruda del pin analógico.

        // Regla de tres: lleva 0..ANALOG_MAXIMUM_VALUE a minimum..maximum.
        float measuredValue = minimumValue
            + (maximumValue - minimumValue) * rawValue / ANALOG_MAXIMUM_VALUE;

        // Manejo de excepción: valor fuera de rango -> aviso de falla.
        if (measuredValue < minimumValidValue || measuredValue > maximumValidValue) {
            return Event::FAULT; // "value" conserva la última lectura buena.
        }
        value = measuredValue;   // Guarda la lectura para getValue().
        return Event::MEASURED;  // Avisa que hay dato nuevo.
    }

public:
    // Al inicio, el rango válido es todo el rango del sensor.
    AnalogSensor(int sensorId, int analogPin, float minimum, float maximum, EventHandler* handler)
        : Sensor(sensorId, analogPin, handler),           // Llama al constructor del padre.
          minimumValue(minimum), maximumValue(maximum),
          minimumValidValue(minimum), maximumValidValue(maximum) {}

    // Opcional: restringe qué valores se aceptan. Ej.: setValidRange(30, 200).
    void setValidRange(float minimumValid, float maximumValid) {
        minimumValidValue = minimumValid;
        maximumValidValue = maximumValid;
    }
};

#endif // ANALOG_SENSOR_H
