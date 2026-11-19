/*
  WeightSensor.h  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  Balanza: celda de carga + amplificador HX711. Peso en kg.
  Usa la librería "HX711 Arduino Library" (de Bogdan Necula).

  ¿Qué es calibrationFactor? El HX711 entrega "cuentas" crudas; el factor
  dice cuántas cuentas equivalen a 1 kg.
  - El HX711 "50kg" de Wokwi da 0..21000 cuentas para 0..50 kg = 420/kg.
  - Con 105 se simula una plataforma de 200 kg (4 celdas de 50 kg):
    el control en 20 kg muestra 80 kg. Útil si el caso pide pesos > 50 kg.
*/

#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include "HX711.h"
#include "ModestIoT.h"

class WeightSensor : public Sensor {
private:
    static const unsigned long READ_TIMEOUT_IN_MILLISECONDS = 200;     // Espera máx. por lectura.
    static const unsigned long STARTUP_TIMEOUT_IN_MILLISECONDS = 1000; // Espera máx. al arrancar.
    HX711 scale;             // Objeto de la librería.
    int clockPin;            // Pin SCK (el pin principal "pin" es DT).
    float calibrationFactor; // Cuentas por kg (ver arriba).

protected:
    int read() override {
        // Excepción: si el HX711 no responde a tiempo (desconectado), falla.
        // Se usa wait_ready_timeout para NO quedarse colgado esperando.
        if (!scale.wait_ready_timeout(READ_TIMEOUT_IN_MILLISECONDS)) {
            return Event::FAULT;
        }
        float reading = scale.get_units(1); // Lectura ya dividida por el factor = kg.
        // Tras la tara puede salir -0.1 por ruido: se muestra como 0.
        value = (reading < 0.0f) ? 0.0f : reading;
        return Event::MEASURED;
    }

public:
    WeightSensor(int sensorId, int dataPin, int sckPin, float countsPerKilogram, EventHandler* handler)
        : Sensor(sensorId, dataPin, handler), clockPin(sckPin), calibrationFactor(countsPerKilogram) {}

    // Inicia la balanza y hace la "tara" (pone en cero el peso actual).
    // Devuelve false si el HX711 no contesta: el Device lo trata como falla.
    bool begin() override {
        scale.begin(pin, clockPin);
        scale.set_scale(calibrationFactor);
        if (!scale.wait_ready_timeout(STARTUP_TIMEOUT_IN_MILLISECONDS)) {
            return false;
        }
        scale.tare();
        return true;
    }
};

#endif // WEIGHT_SENSOR_H
