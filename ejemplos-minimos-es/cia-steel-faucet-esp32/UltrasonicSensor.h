/*
  UltrasonicSensor.h
  Cia Steel Faucet - Aplicación embebida (basada en Modest IoT)

  Sensor de distancia HC-SR04. Funciona como un murciélago:
    1. Se manda un pulso corto por TRIG ("grita").
    2. El sensor pone ECHO en HIGH mientras espera el rebote ("escucha").
    3. Cuánto dura ECHO en HIGH = tiempo de ida y vuelta del sonido.
  El sonido tarda unos 58 microsegundos en ir y volver 1 cm, así que
  distancia (cm) = duración / 58.

  Creado el 19 de noviembre de 2025
  Por <Developer Full Name> - Moen, inc.
*/

#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include "ModestIoT.h"

class UltrasonicSensor : public Sensor {
private:
    // Si no llega eco en 30 ms (~5 m), se da por perdido.
    static const unsigned long ECHO_TIMEOUT_IN_MICROSECONDS = 30000;
    // "constexpr" = constante conocida al compilar. Se usa para float porque
    // "static const float" dentro de una clase no está permitido en C++11.
    static constexpr float MICROSECONDS_PER_CENTIMETER = 58.0f;
    int echoPin; // El pin principal (pin) es TRIG; este es ECHO.

protected:
    int read() override {
        // Pulso de disparo: LOW 2 µs (limpia), HIGH 10 µs (dispara), LOW.
        digitalWrite(pin, LOW);
        delayMicroseconds(2);
        digitalWrite(pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(pin, LOW);

        // pulseIn mide cuántos µs está ECHO en HIGH (0 si se pasa el timeout).
        unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_IN_MICROSECONDS);
        if (duration == 0) {
            return Event::FAULT; // Excepción: sin eco (fuera de rango o desconectado).
        }
        value = duration / MICROSECONDS_PER_CENTIMETER; // Distancia en cm.
        return Event::MEASURED;
    }

public:
    // El pin del padre (Sensor) es TRIG; echoSignalPin se guarda aparte.
    UltrasonicSensor(int sensorId, int triggerPin, int echoSignalPin, EventHandler* handler)
        : Sensor(sensorId, triggerPin, handler), echoPin(echoSignalPin) {}

    bool begin() override {
        pinMode(pin, OUTPUT);    // TRIG: la placa manda el pulso.
        pinMode(echoPin, INPUT); // ECHO: la placa escucha.
        return true;
    }
};

#endif // ULTRASONIC_SENSOR_H
