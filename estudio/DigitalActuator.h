/*
  DigitalActuator.h  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  Actuadores de dos estados (encendido / apagado):
    Led    -> indicador luminoso
    Relay  -> módulo relé: válvula de agua, bomba, cerradura...
    Buzzer -> zumbador: suena mientras está encendido

  Los tres comparten la misma lógica (DigitalActuator) y solo el Buzzer
  cambia la forma de "encender" (usa tone() en vez de digitalWrite).
  Esto es herencia + polimorfismo: el método apply() es "virtual" y el
  Buzzer lo reemplaza con su propia versión.
*/

#ifndef DIGITAL_ACTUATOR_H
#define DIGITAL_ACTUATOR_H

#include "ModestIoT.h"

class DigitalActuator : public Actuator {
protected:
    bool turnedOn = false; // Estado actual.

    // Escribe el estado en el hardware. "virtual" = las hijas pueden
    // reemplazarlo (el Buzzer lo hace).
    virtual void apply(bool turnOn) {
        turnedOn = turnOn;
        digitalWrite(pin, turnedOn ? HIGH : LOW);
    }

public:
    explicit DigitalActuator(int signalPin) : Actuator(signalPin) {}

    // Configura el pin como salida y arranca apagado.
    void begin() override {
        pinMode(pin, OUTPUT);
        apply(false);
    }

    // Recibe una orden (Command) y la ejecuta. Es la forma "ModestIoT" de
    // controlar un actuador: el Device no toca pines, da órdenes.
    void handle(Command command) override {
        switch (command.type) {
            case Command::TURN_ON:  apply(true); break;
            case Command::TURN_OFF: apply(false); break;
            case Command::TOGGLE:   apply(!turnedOn); break; // "!" invierte el bool.
            default:                break; // Orden desconocida: se ignora.
        }
    }

    // Atajo que más vas a usar: led.setState(condición).
    // Solo envía la orden si el estado realmente cambia; así no se repiten
    // órdenes en cada ciclo y la consola no se llena de mensajes.
    void setState(bool shouldBeOn) {
        if (shouldBeOn == turnedOn) {
            return; // Ya está como se pide.
        }
        if (shouldBeOn) {
            handle(Command(Command::TURN_ON));
        } else {
            handle(Command(Command::TURN_OFF));
        }
    }

    bool isOn() const { return turnedOn; }
};

// Led y Relay se comportan igual que DigitalActuator. Existen como clases
// propias para que el código diga QUÉ es cada cosa (Led ledAlarma; Relay
// valvula;). El enunciado pide clases que representen cada tipo de actuator.
class Led : public DigitalActuator {
public:
    explicit Led(int signalPin) : DigitalActuator(signalPin) {}
};

class Relay : public DigitalActuator {
public:
    explicit Relay(int signalPin) : DigitalActuator(signalPin) {}
};

// El buzzer necesita una frecuencia para sonar: tone(pin, Hz) / noTone(pin).
class Buzzer : public DigitalActuator {
private:
    unsigned int frequencyInHertz; // Ej.: 1000 Hz.

protected:
    // Reemplaza el apply() del padre (polimorfismo).
    void apply(bool turnOn) override {
        turnedOn = turnOn;
        if (turnedOn) {
            tone(pin, frequencyInHertz);
        } else {
            noTone(pin);
        }
    }

public:
    Buzzer(int signalPin, unsigned int frequency) : DigitalActuator(signalPin), frequencyInHertz(frequency) {}
};

#endif // DIGITAL_ACTUATOR_H
