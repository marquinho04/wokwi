/*
  CiaSteelFaucetDevice.h
  Cia Steel Faucet - Aplicación embebida (basada en Modest IoT)

  Device (mediador) del grifo Cia Steel Faucet con MotionSense Wave, en ESP32.
  Reglas del caso:
  * Manos a menos de 10 cm -> LED azul encendido; más lejos -> apagado.
  * Al llegar las manos, el relé abre la válvula de agua 5 s y la cierra.
  * Cada 2,5 s la consola muestra la distancia (cm) y la válvula (OPEN/CLOSED).
  * Al iniciar, la consola muestra la empresa y el developer.

  Conexiones (ESP32 DevKit V1):
  * HC-SR04 TRIG -> GPIO 5, ECHO -> GPIO 18
  * LED azul (con resistencia de 220 ohm) -> GPIO 4
  * Módulo relé IN -> GPIO 23

  Creado el 19 de noviembre de 2025
  Por <Developer Full Name> - Moen, inc.
*/

// Include guard: evita que este archivo se compile dos veces.
#ifndef CIA_STEEL_FAUCET_DEVICE_H
#define CIA_STEEL_FAUCET_DEVICE_H

#include "ModestIoT.h"         // Núcleo del kit: Device, Event, Command, temporizadores.
#include "UltrasonicSensor.h"  // Sensor de distancia HC-SR04.
#include "DigitalActuator.h"   // Actuadores Led y Relay.

// "class X : public Device" = X ES un Device: debe tener setup(), loop() y on().
class CiaSteelFaucetDevice : public Device {
private: // Lo de aquí solo lo usa esta clase.
    // ----- Pines (ESP32). Varias constantes en una línea, separadas por comas.
    static const int TRIGGER_PIN = 5, ECHO_PIN = 18, LED_PIN = 4, RELAY_PIN = 23;

    // ----- Identificador del sensor: on() lo usa para saber quién avisó.
    static const int PROXIMITY_SENSOR_ID = 1;

    // ----- Reglas del caso como constantes con nombre (nada de números sueltos).
    static const int THRESHOLD_IN_CENTIMETERS = 10;  // Manos detectadas por debajo de 10 cm.
    static const unsigned long SAMPLING_MS = 200;    // Medir la distancia cada 0,2 s.
    static const unsigned long REPORT_MS = 2500;     // Reporte en consola cada 2,5 s.
    static const unsigned long VALVE_OPEN_MS = 5000; // Válvula abierta durante 5 s.

    // ----- Componentes: cada uno se crea aquí mismo, con sus pines entre llaves { }.
    // "this" = este Device. Se le pasa al sensor para que, al medir, avise a on().
    UltrasonicSensor proximitySensor{PROXIMITY_SENSOR_ID, TRIGGER_PIN, ECHO_PIN, this};
    Led proximityLed{LED_PIN};                // LED azul: manos detectadas.
    Relay valveRelay{RELAY_PIN};              // Relé que abre la válvula de agua.
    PeriodicTimer samplingTimer{SAMPLING_MS}; // Cuándo medir.
    PeriodicTimer reportTimer{REPORT_MS};     // Cuándo imprimir el reporte.
    OneShotTimer valveTimer{VALVE_OPEN_MS};   // Cuánto dura abierta la válvula.

    // ----- Estado: lo que hay que recordar entre una medición y otra.
    bool sensorFault = false; // true mientras el sensor no recibe eco.

public: // Lo de aquí lo puede llamar sketch.ino.
    // setup(): se ejecuta UNA vez al encender. "override" = reemplaza el de Device.
    void setup() override {
        Serial.begin(115200); // Abre la consola (velocidad típica del ESP32).
        // F("...") guarda el texto en la memoria de programa y ahorra RAM.
        Serial.println(F("Moen, inc. - Cia Steel Faucet | Developer: <Developer Full Name>"));
        proximitySensor.begin(); // TRIG como salida, ECHO como entrada.
        proximityLed.begin();    // Pin del LED como salida, arranca apagado.
        valveRelay.begin();      // Pin del relé como salida, válvula cerrada.
    }

    // loop(): se repite sin parar. Solo revisa relojes; nunca usa delay().
    void loop() override {
        // 1. ¿Toca medir? El sensor mide y, con el resultado, llama a on().
        if (samplingTimer.isDue()) proximitySensor.measure();

        // 2. ¿Ya pasaron los 5 s de la válvula? hasExpired() da true una sola vez.
        if (valveTimer.hasExpired()) {
            valveRelay.setState(false); // Cierra la válvula.
            Serial.println(F("[VALVE] CLOSED"));
        }

        // 3. ¿Toca el reporte de cada 2,5 s?
        if (reportTimer.isDue()) {
            Serial.print(F("[REPORT] Distance: "));
            Serial.print(proximitySensor.getValue(), 1); // Distancia con 1 decimal.
            // (condición ? A : B) elige el texto según si el relé está encendido.
            Serial.println(valveRelay.isOn() ? F(" cm | Valve: OPEN") : F(" cm | Valve: CLOSED"));
        }
    }

    // on(): aquí llegan los avisos del sensor. Aquí van las REGLAS del caso.
    void on(Event event) override {
        // Manejo de excepción: el sensor no recibió eco (fuera de rango o desconectado).
        if (event.type == Event::FAULT) {
            // Solo imprime la primera vez, para no llenar la consola.
            if (!sensorFault) Serial.println(F("[ERROR] Proximity sensor without echo"));
            sensorFault = true;
            proximityLed.setState(false); // Estado seguro: sin detección.
            return;                       // Sale de on(): no aplica las reglas.
        }
        sensorFault = false; // Si llegó aquí, la lectura es válida.

        // Regla 1: hay manos si la distancia es MENOR a 10 cm (10 exacto no cuenta).
        bool handsNear = proximitySensor.getValue() < THRESHOLD_IN_CENTIMETERS;

        // Regla 2: abrir la válvula solo cuando las manos ACABAN de llegar.
        // Si el LED todavía está apagado, es que antes no había manos.
        // Así la válvula no se reabre mientras las manos sigan cerca.
        if (handsNear && !proximityLed.isOn()) {
            valveRelay.setState(true); // Abre la válvula.
            valveTimer.start();        // Empieza la cuenta de 5 s.
            Serial.println(F("[VALVE] OPEN for 5 s"));
        }

        // Regla 1: el LED sigue a la detección. setState solo actúa si hay cambio.
        proximityLed.setState(handsNear);
    }
};

#endif // CIA_STEEL_FAUCET_DEVICE_H  (fin del include guard)
