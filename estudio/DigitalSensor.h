/*
  DigitalSensor.h  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  Sensores de dos estados (activo / inactivo): pulsador, sensor de
  movimiento PIR, switch, sensor de inclinación.

  Solo avisa cuando el estado CAMBIA (Event::CHANGED). Así, si mantienes
  presionado un botón, el Device recibe un solo aviso, no cientos.

  activeLow:
    true  -> botón conectado entre el pin y GND. Se usa la resistencia
             interna de pull-up: en reposo el pin lee HIGH y al presionar LOW.
    false -> sensores que entregan HIGH cuando detectan (como el PIR).
*/

#ifndef DIGITAL_SENSOR_H
#define DIGITAL_SENSOR_H

#include "ModestIoT.h"

class DigitalSensor : public Sensor {
private:
    bool activeLow;      // Ver explicación arriba.
    bool active = false; // Último estado conocido (empieza inactivo).

protected:
    int read() override {
        // ¿El pin está en el nivel que significa "activo"?
        // (condición ? valorSiVerdadero : valorSiFalso) es un "if" en una línea.
        bool currentlyActive = (digitalRead(pin) == (activeLow ? LOW : HIGH));

        if (currentlyActive == active) {
            return Event::NONE; // Sin cambio: no molestar al Device.
        }
        active = currentlyActive;     // Guarda el nuevo estado.
        value = active ? 1.0f : 0.0f; // Para getValue(): 1 activo, 0 inactivo.
        return Event::CHANGED;
    }

public:
    DigitalSensor(int sensorId, int signalPin, bool isActiveLow, EventHandler* handler)
        : Sensor(sensorId, signalPin, handler), activeLow(isActiveLow) {}

    // Configura el pin como entrada (con pull-up interno si es activeLow).
    bool begin() override {
        pinMode(pin, activeLow ? INPUT_PULLUP : INPUT);
        return true;
    }

    bool isActive() const { return active; }
};

#endif // DIGITAL_SENSOR_H
