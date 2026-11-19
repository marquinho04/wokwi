/*
  WeightSensor.h
  Smart LifeWatching - Aplicación embebida (basada en Modest IoT)

  Balanza: celda de carga + amplificador HX711. Peso en kg.
  Usa la librería "HX711 Arduino Library" (de Bogdan Necula).

  "No bloqueante": no se queda esperando al HX711. En cada medición
  pregunta si hay un dato listo (is_ready()):
    - Si hay dato: lo lee y avisa Event::MEASURED.
    - Si no hay dato todavía: no avisa nada (Event::NONE) y se conserva
      el último peso.
    - Si pasan 2 segundos sin ningún dato: avisa Event::FAULT
      (HX711 desconectado o sin respuesta).

  ¿Qué es calibrationFactor? El HX711 entrega "cuentas" crudas; el factor
  dice cuántas cuentas equivalen a 1 kg.
  - El HX711 "50kg" de Wokwi da 0..21000 cuentas para 0..50 kg = 420/kg.
  - Con 105 se simula una plataforma de 200 kg (4 celdas de 50 kg):
    el control en 20 kg muestra 80 kg.
  No se hace "tara" (poner en cero): en Wokwi la celda ya marca 0 con 0 kg.

  Creado el 19 de noviembre de 2025
  Por <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include "HX711.h"
#include "ModestIoT.h"

class WeightSensor : public Sensor {
private:
    static const unsigned long FAULT_TIMEOUT_IN_MILLISECONDS = 2000; // 2 s sin datos = falla.
    HX711 scale;                            // Objeto de la librería.
    int clockPin;                           // Pin SCK (el pin principal "pin" es DT).
    float calibrationFactor;                // Cuentas por kg (ver arriba).
    unsigned long lastReadingTimestamp = 0; // Cuándo llegó el último dato válido.

protected:
    int read() override {
        // ¿El HX711 tiene un dato listo? (DT en LOW = listo)
        if (!scale.is_ready()) {
            // Todavía no. ¿Ya pasó demasiado tiempo sin datos?
            if (millis() - lastReadingTimestamp > FAULT_TIMEOUT_IN_MILLISECONDS) {
                return Event::FAULT; // Excepción: el HX711 no responde.
            }
            return Event::NONE;      // Esperar a la próxima medición, sin avisar.
        }
        float reading = scale.get_units(1);        // Lectura dividida por el factor = kg.
        value = (reading < 0.0f) ? 0.0f : reading; // Un -0.1 por ruido se muestra como 0.
        lastReadingTimestamp = millis();           // Recuerda cuándo llegó este dato.
        return Event::MEASURED;
    }

public:
    WeightSensor(int sensorId, int dataPin, int sckPin, float countsPerKilogram, EventHandler* handler)
        : Sensor(sensorId, dataPin, handler), clockPin(sckPin), calibrationFactor(countsPerKilogram) {}

    // Configura los pines y el factor de calibración. Nunca se queda esperando.
    bool begin() override {
        scale.begin(pin, clockPin);         // DT y SCK.
        scale.set_scale(calibrationFactor); // Cuentas por kg.
        lastReadingTimestamp = millis();    // Empieza a contar los 2 s desde ahora.
        return true;
    }
};

#endif // WEIGHT_SENSOR_H
