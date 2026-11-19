/*
  CiaSteelFaucetDevice.h  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  Caso Moen (PC2 2024-2): grifo sin contacto en ESP32.
  Reglas del enunciado y dónde se cumplen en el código:
    1. Manos a menos de 10 cm -> LED azul encendido; si no, apagado.
       -> on() + updateProximity()
    2. Al llegar al umbral, el relé abre la válvula durante 5 segundos.
       -> openValve() + valveTimer (OneShotTimer) + loop()
    3. Cada 2,5 s la consola muestra la distancia (cm) y la válvula (open/closed).
       -> reportTimer (PeriodicTimer) + printReport()
    4. Al iniciar, la consola muestra la empresa y el developer.
       -> printCompanyInformation()

  ESTE es el único archivo que escribes para un caso nuevo: el kit pone
  los sensores y actuadores; aquí van las reglas del caso.
*/

#ifndef CIA_STEEL_FAUCET_DEVICE_H
#define CIA_STEEL_FAUCET_DEVICE_H

// Solo se incluyen las piezas del kit que este caso necesita.
#include "ModestIoT.h"
#include "UltrasonicSensor.h"
#include "DigitalActuator.h"

class CiaSteelFaucetDevice : public Device { // Es un Device: tiene setup, loop y on.
private:
    // ---- 1. CONSTANTES: todo número del enunciado o del circuito tiene nombre.
    //      (La rúbrica penaliza "números mágicos" sueltos en el código.)
    static const int TRIGGER_PIN = 5;        // HC-SR04 TRIG -> GPIO 5
    static const int ECHO_PIN = 18;          // HC-SR04 ECHO -> GPIO 18
    static const int PROXIMITY_LED_PIN = 4;  // LED azul -> GPIO 4
    static const int VALVE_RELAY_PIN = 23;   // Relé (IN) -> GPIO 23

    // Identificador del sensor: con él on() sabe quién avisó.
    static const int PROXIMITY_SENSOR_ID = 1;

    static const unsigned long SERIAL_BAUD_RATE = 115200;                 // Velocidad de consola (ESP32).
    static const unsigned long SAMPLING_INTERVAL_IN_MILLISECONDS = 200;   // Medir cada 0,2 s.
    static const unsigned long REPORT_INTERVAL_IN_MILLISECONDS = 2500;    // Regla 3: cada 2,5 s.
    static const unsigned long VALVE_OPEN_DURATION_IN_MILLISECONDS = 5000; // Regla 2: 5 s.
    static const int PROXIMITY_THRESHOLD_IN_CENTIMETERS = 10;             // Regla 1: 10 cm.

    // ---- 2. COMPONENTES: un objeto por cada pieza del circuito y cada temporizador.
    UltrasonicSensor proximitySensor;
    Led proximityLed;
    Relay valveRelay;
    PeriodicTimer samplingTimer; // Cuándo medir.
    PeriodicTimer reportTimer;   // Cuándo imprimir el reporte.
    OneShotTimer valveTimer;     // Cuánto dura abierta la válvula.

    // ---- 3. ESTADO: lo que el Device necesita recordar entre ciclos.
    bool handsDetected = false;  // ¿Las manos están cerca ahora?
    bool sensorFault = false;    // ¿El sensor está fallando?

    // F("...") guarda el texto en la memoria de programa y ahorra RAM
    // (importante en el Uno, que solo tiene 2 KB).
    void printCompanyInformation() {
        Serial.println(F("=============================================="));
        Serial.println(F(" Moen, inc. - Smart Water Network"));
        Serial.println(F(" Cia Steel Faucet with MotionSense Wave"));
        Serial.println(F(" Developer: <Developer Full Name> - <u20XXXXXXX>"));
        Serial.println(F("=============================================="));
    }

    const char* valveStateText() const {
        return valveRelay.isOn() ? "OPEN" : "CLOSED";
    }

    // Regla 2: abre la válvula y arranca la cuenta de 5 s.
    void openValve() {
        valveRelay.setState(true);
        valveTimer.start();
        Serial.println(F("[VALVE] OPEN for 5 s"));
    }

    void closeValve() {
        valveRelay.setState(false);
        valveTimer.stop();
        Serial.println(F("[VALVE] CLOSED"));
    }

    // Regla 1 (LED) y disparo de la regla 2 (válvula).
    void updateProximity(bool detected) {
        if (detected == handsDetected) {
            return; // Nada cambió: no se repiten mensajes ni órdenes.
        }
        handsDetected = detected;
        proximityLed.setState(handsDetected); // LED sigue a la detección.
        Serial.println(handsDetected ? F("[STATE] Hands detected (< 10 cm)")
                                     : F("[STATE] Hands removed"));
        // Abre solo cuando las manos LLEGAN (no mientras siguen cerca),
        // así no se reabre en bucle ni se desperdicia agua.
        if (handsDetected && !valveRelay.isOn()) {
            openValve();
        }
    }

    // Regla 3: reporte periódico de distancia y válvula.
    void printReport() {
        Serial.print(F("[REPORT] Proximity: "));
        if (sensorFault) {
            Serial.print(F("N/A (sensor fault)"));
        } else {
            Serial.print(proximitySensor.getValue(), 1); // 1 decimal.
            Serial.print(F(" cm"));
        }
        Serial.print(F(" | Valve: "));
        Serial.println(valveStateText());
    }

public:
    // ---- 4. CONSTRUCTOR: crea cada componente con sus pines.
    // "this" = este Device. Se le pasa al sensor para que sepa a quién
    // avisar: cuando mida, llamará a this->on(evento).
    CiaSteelFaucetDevice()
        : proximitySensor(PROXIMITY_SENSOR_ID, TRIGGER_PIN, ECHO_PIN, this),
          proximityLed(PROXIMITY_LED_PIN),
          valveRelay(VALVE_RELAY_PIN),
          samplingTimer(SAMPLING_INTERVAL_IN_MILLISECONDS),
          reportTimer(REPORT_INTERVAL_IN_MILLISECONDS),
          valveTimer(VALVE_OPEN_DURATION_IN_MILLISECONDS) {}

    // ---- 5. setup(): una vez al arrancar.
    void setup() override {
        Serial.begin(SERIAL_BAUD_RATE);
        printCompanyInformation();   // Regla 4.
        proximitySensor.begin();     // Configura los pines de cada componente.
        proximityLed.begin();
        valveRelay.begin();
        Serial.println(F("[MODE] ACTIVE: waiting for hands")); // Modo de operación.
    }

    // ---- 6. loop(): se repite sin parar. Solo revisa relojes; nunca delay().
    void loop() override {
        if (samplingTimer.isDue()) {
            proximitySensor.measure(); // Mide; si hay dato, el sensor llama a on().
        }
        if (valveTimer.hasExpired()) {
            closeValve();              // Regla 2: pasaron los 5 s.
        }
        if (reportTimer.isDue()) {
            printReport();             // Regla 3: cada 2,5 s.
        }
    }

    // ---- 7. on(): aquí llegan los avisos de los sensores. Las reglas del caso.
    void on(Event event) override {
        switch (event.sourceId) {      // ¿Qué sensor avisó?
            case PROXIMITY_SENSOR_ID:
                if (event.type == Event::FAULT) {
                    // Excepción: sensor sin eco. Se avisa una sola vez y se
                    // apaga el LED (estado seguro).
                    if (!sensorFault) {
                        Serial.println(F("[ERROR] Proximity sensor without echo"));
                    }
                    sensorFault = true;
                    updateProximity(false);
                } else {
                    sensorFault = false;
                    // Regla 1: "traspasa 10 cm" = distancia menor a 10.
                    updateProximity(proximitySensor.getValue() < PROXIMITY_THRESHOLD_IN_CENTIMETERS);
                }
                break;

            default:
                break; // Sensor desconocido: se ignora (sin escenarios sin contemplar).
        }
    }
};

#endif // CIA_STEEL_FAUCET_DEVICE_H
