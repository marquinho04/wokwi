/*
  SmartLifeWatchingDevice.h  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  Versión mínima del caso Smart Lifemonitor (PC2 2025-2), en Arduino Uno.
  Reglas del caso:
  * Peso mayor a 80 kg -> LED azul de peso encendido.
  * Pulso < 60 bajo (LED azul), 60 a 100 normal (verde), > 100 alto (rojo).
  * LCD 2004 con peso, pulso (BPM) y estado.
  * Consola: empresa y developer al iniciar, y cada cambio de estado.
  Excepciones: balanza sin respuesta -> "SCALE ERROR"; pulso menor a 30
  -> "NO SIGNAL". El pulso se simula con un potenciómetro (0-200 BPM)
  porque Wokwi no tiene sensor de pulso.

  Conexiones (Arduino Uno):
  * HX711 DT -> D3, SCK -> D2
  * Potenciómetro (pulso) SIG -> A0
  * LED de peso -> D8; LEDs bajo / normal / alto -> D9 / D10 / D11
  * LCD 2004 I2C (0x27): SDA -> A4, SCL -> A5
*/

#ifndef SMART_LIFE_WATCHING_DEVICE_H
#define SMART_LIFE_WATCHING_DEVICE_H

#include "ModestIoT.h"        // Núcleo: Device, Event, Command, temporizadores, LevelClassifier.
#include "AnalogSensor.h"     // Sensor de pulso (potenciómetro).
#include "WeightSensor.h"     // Balanza: celda de carga + HX711.
#include "DigitalActuator.h"  // LEDs.
#include "LcdDisplay.h"       // Pantalla LCD 2004 por I2C.

class SmartLifeWatchingDevice : public Device {
private:
    // ----- Pines (Arduino Uno)
    static const int SCALE_DATA_PIN = 3, SCALE_CLOCK_PIN = 2, HEART_RATE_PIN = A0;
    static const int OVERWEIGHT_LED_PIN = 8, LOW_LED_PIN = 9, NORMAL_LED_PIN = 10, HIGH_LED_PIN = 11;

    // ----- Identificadores: on() los usa para saber qué sensor avisó.
    static const int WEIGHT_SENSOR_ID = 1, HEART_RATE_SENSOR_ID = 2;

    // ----- Reglas del caso
    static const int OVERWEIGHT_KILOGRAMS = 80;   // LED de peso por encima de 80 kg.
    static const int MINIMUM_VALID_BPM = 30;      // Debajo de esto: sin señal de pulso.
    static const int MAXIMUM_BPM = 200;           // Recorrido completo del potenciómetro.
    static const unsigned long SAMPLING_MS = 500; // Medir cada 0,5 s.

    // ----- Componentes (se crean aquí con sus pines). "this" = a quién avisan los sensores.
    // 105 = cuentas por kg: simula una plataforma de 200 kg con el HX711 "50kg" de Wokwi.
    WeightSensor weightSensor{WEIGHT_SENSOR_ID, SCALE_DATA_PIN, SCALE_CLOCK_PIN, 105.0f, this};
    // El potenciómetro se convierte a 0..200 BPM.
    AnalogSensor heartRateSensor{HEART_RATE_SENSOR_ID, HEART_RATE_PIN, 0, MAXIMUM_BPM, this};
    // Cuatro LEDs declarados en una sola línea, separados por comas.
    Led overweightLed{OVERWEIGHT_LED_PIN}, lowLed{LOW_LED_PIN}, normalLed{NORMAL_LED_PIN}, highLed{HIGH_LED_PIN};
    LcdDisplay display{0x27, 20, 4};                  // Dirección I2C 0x27, 20 columnas x 4 filas.
    PeriodicTimer samplingTimer{SAMPLING_MS};         // Cuándo medir.
    LevelClassifier heartRateClassifier{60, 100};     // < 60 LOW, 60-100 NORMAL, > 100 HIGH.

    // Función auxiliar: muestra el estado del pulso en la LCD y lo imprime en consola.
    // "const char*" = texto (cadena de caracteres de C).
    void showHeartRateStatus(const char* status) {
        char line[21];                                      // Una fila de la LCD: 20 letras + fin de texto.
        snprintf(line, sizeof(line), "Status: %s", status); // Arma el texto sin pasarse del tamaño.
        display.setLine(2, line);                           // Fila 3 de la LCD (se cuenta desde 0).
        Serial.print(F("[STATE] Heart rate: "));
        Serial.println(status);
    }

public:
    // setup(): una vez al encender.
    void setup() override {
        Serial.begin(9600); // Consola del Uno.
        Serial.println(F("Smart LifeMonitor, inc. | Developer: <Developer Full Name>"));
        overweightLed.begin(); lowLed.begin(); normalLed.begin(); highLed.begin(); // LEDs como salida, apagados.
        display.begin();                                  // Enciende la LCD y su luz.
        display.setLine(3, "Smart LifeWatching");         // Nombre del dispositivo en la última fila.
        heartRateSensor.setValidRange(MINIMUM_VALID_BPM, MAXIMUM_BPM); // < 30 BPM -> el sensor avisa FAULT.
        // begin() inicia y pone en cero la balanza; da false si el HX711 no responde.
        if (!weightSensor.begin()) Serial.println(F("[ERROR] Scale (HX711) not responding"));
    }

    // loop(): se repite sin parar; cada 0,5 s mide y refresca la pantalla.
    void loop() override {
        if (samplingTimer.isDue()) {
            weightSensor.measure();                    // Mide el peso -> llama a on().
            heartRateSensor.measure();                 // Mide el pulso -> llama a on().
            display.handle(Command(Command::REFRESH)); // Escribe en la LCD las filas que cambiaron.
        }
    }

    // on(): llegan los avisos de los sensores. Aquí están las reglas del caso.
    void on(Event event) override {
        char line[21]; // Espacio para armar el texto de una fila de la LCD.

        // ----- Aviso de la balanza -----
        if (event.sourceId == WEIGHT_SENSOR_ID) {
            if (event.type == Event::FAULT) {               // Excepción: la balanza no responde.
                display.setLine(0, "Weight: SCALE ERROR");
                overweightLed.setState(false);              // Estado seguro: sin peso válido.
                return;
            }
            char kilograms[8];
            // dtostrf(valor, ancho, decimales, destino): convierte el decimal a texto.
            // Se usa porque en el Uno snprintf no sabe imprimir decimales (%f).
            dtostrf(weightSensor.getValue(), 5, 1, kilograms);
            snprintf(line, sizeof(line), "Weight: %s kg", kilograms);
            display.setLine(0, line);                       // Fila 1 de la LCD.
            // Regla: "mayor a 80 kg" -> ">" (80,0 exacto NO enciende el LED).
            overweightLed.setState(weightSensor.getValue() > OVERWEIGHT_KILOGRAMS);

        // ----- Aviso del sensor de pulso -----
        } else if (event.sourceId == HEART_RATE_SENSOR_ID) {
            if (event.type == Event::FAULT) {               // Excepción: sin señal de pulso.
                display.setLine(1, "Heart rate: --- BPM");
                // Imprime el cambio solo si antes había un estado (algún LED encendido).
                if (lowLed.isOn() || normalLed.isOn() || highLed.isOn()) showHeartRateStatus("NO SIGNAL");
                lowLed.setState(false); normalLed.setState(false); highLed.setState(false); // Todos apagados.
                return;
            }
            // (int)(valor + 0.5) redondea al entero más cercano.
            int bpm = (int) (heartRateSensor.getValue() + 0.5f);
            Level level = heartRateClassifier.classify(bpm); // LOW, NORMAL o HIGH (60 y 100 son NORMAL).
            snprintf(line, sizeof(line), "Heart rate: %d BPM", bpm); // %d = número entero.
            display.setLine(1, line);                         // Fila 2 de la LCD.

            // ¿Cambió el estado? Sí, si el LED del nuevo nivel no es el que está encendido.
            bool changed = (level == Level::Low) != lowLed.isOn()
                        || (level == Level::Normal) != normalLed.isOn()
                        || (level == Level::High) != highLed.isOn();

            // Un solo LED encendido: el del nivel actual.
            lowLed.setState(level == Level::Low);
            normalLed.setState(level == Level::Normal);
            highLed.setState(level == Level::High);
            if (changed) showHeartRateStatus(LevelClassifier::toText(level)); // Solo si cambió.
        }
    }
};

#endif // SMART_LIFE_WATCHING_DEVICE_H
