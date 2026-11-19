/*
  SmartLifeWatchingDevice.h  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  Caso Smart Lifemonitor (PC2 2025-2) en Arduino Uno.
  Reglas del enunciado y dónde se cumplen:
    1. Peso mayor a 80 kg -> LED azul de peso encendido.      -> on(), rama WEIGHT
    2. Pulso < 60 bajo (azul), 60-100 normal (verde),
       > 100 alto (rojo).                                     -> on(), rama HEART_RATE
    3. LCD 2004: peso, pulso en BPM y estado.                 -> updateDisplay()
    4. Consola: empresa y developer al iniciar; cambios de
       estado y modo de operación.                            -> setup(), on()
  Excepciones manejadas: HX711 sin respuesta ("SCALE ERROR") y pulso
  menor a 30 BPM ("NO SIGNAL": sensor mal puesto o desconectado).
*/

#ifndef SMART_LIFE_WATCHING_DEVICE_H
#define SMART_LIFE_WATCHING_DEVICE_H

#include "ModestIoT.h"
#include "AnalogSensor.h"     // Pulso (simulado con potenciómetro).
#include "WeightSensor.h"     // Balanza HX711.
#include "DigitalActuator.h"  // LEDs.
#include "LcdDisplay.h"       // LCD 2004.

class SmartLifeWatchingDevice : public Device {
private:
    // ---- 1. CONSTANTES (pines del Uno, identificadores y reglas del caso)
    static const int WEIGHT_DATA_PIN = 3;     // HX711 DT -> D3
    static const int WEIGHT_CLOCK_PIN = 2;    // HX711 SCK -> D2
    static const int HEART_RATE_PIN = A0;     // Potenciómetro -> A0
    static const int OVERWEIGHT_LED_PIN = 8;  // LED azul de peso
    static const int LOW_LED_PIN = 9;         // LED azul: pulso bajo
    static const int NORMAL_LED_PIN = 10;     // LED verde: pulso normal
    static const int HIGH_LED_PIN = 11;       // LED rojo: pulso alto
    static const unsigned char LCD_ADDRESS = 0x27; // Dirección I2C de la LCD en Wokwi.
    static const int LCD_COLUMNS = 20;
    static const int LCD_ROWS = 4;

    static const int WEIGHT_SENSOR_ID = 1;    // Para distinguir quién avisa en on().
    static const int HEART_RATE_SENSOR_ID = 2;

    static const unsigned long SERIAL_BAUD_RATE = 9600;               // Consola del Uno.
    static const unsigned long SAMPLING_INTERVAL_IN_MILLISECONDS = 500; // Medir cada 0,5 s.
    static const int OVERWEIGHT_THRESHOLD_IN_KILOGRAMS = 80;  // Regla 1.
    static const int NORMAL_MINIMUM_BEATS_PER_MINUTE = 60;    // Regla 2 (límite bajo).
    static const int NORMAL_MAXIMUM_BEATS_PER_MINUTE = 100;   // Regla 2 (límite alto).
    static const int MINIMUM_VALID_BEATS_PER_MINUTE = 30;     // Debajo: "sin señal".
    static const int MAXIMUM_BEATS_PER_MINUTE = 200;          // Tope del potenciómetro.
    static constexpr float WEIGHT_CALIBRATION_FACTOR = 105.0f; // Ver WeightSensor.h.

    // ---- 2. COMPONENTES
    WeightSensor weightSensor;
    AnalogSensor heartRateSensor;
    Led overweightLed;
    Led lowLed;
    Led normalLed;
    Led highLed;
    LcdDisplay display;
    PeriodicTimer samplingTimer;
    LevelClassifier heartRateClassifier; // Hace la regla 2 (bajo/normal/alto).

    // ---- 3. ESTADO
    bool scaleFault = false;              // ¿La balanza está fallando?
    bool heartRateSignal = false;         // ¿Hay pulso válido?
    Level heartRateLevel = Level::Normal; // Último nivel de pulso.

    void printCompanyInformation() {
        Serial.println(F("=============================================="));
        Serial.println(F(" Smart LifeMonitor, inc. (founded in 2015)"));
        Serial.println(F(" Vital signs monitoring for chronic-disease"));
        Serial.println(F(" risk patients with IoT. Line: LifeWatching"));
        Serial.println(F(" Developer: <Developer Full Name> - <u20XXXXXXX>"));
        Serial.println(F("=============================================="));
    }

    // Redondea el pulso a un entero. Se usa el MISMO valor para la regla y
    // para la pantalla, así nunca se ve "100 BPM" con estado HIGH.
    int beatsPerMinute() const {
        return (int) (heartRateSensor.getValue() + 0.5f);
    }

    const char* heartRateStatusText() const {
        return heartRateSignal ? LevelClassifier::toText(heartRateLevel) : "NO SIGNAL";
    }

    // Regla 1: "mayor a 80" -> se usa ">" (80 exacto NO enciende el LED).
    bool isOverweight() const {
        return !scaleFault && weightSensor.getValue() > OVERWEIGHT_THRESHOLD_IN_KILOGRAMS;
    }

    // Regla 2: actualiza el estado del pulso y los 3 LEDs.
    void updateHeartRate(bool hasSignal, Level newLevel) {
        // Solo imprime cuando el estado CAMBIA ("actualización de estado").
        if (hasSignal != heartRateSignal || newLevel != heartRateLevel) {
            heartRateSignal = hasSignal;
            heartRateLevel = newLevel;
            Serial.print(F("[STATE] Heart rate status: "));
            Serial.println(heartRateStatusText());
        }
        // Un solo LED encendido según el nivel; todos apagados si no hay señal.
        lowLed.setState(heartRateSignal && heartRateLevel == Level::Low);
        normalLed.setState(heartRateSignal && heartRateLevel == Level::Normal);
        highLed.setState(heartRateSignal && heartRateLevel == Level::High);
    }

    // Regla 3: arma las 4 líneas de la LCD 2004 y las escribe.
    void updateDisplay() {
        char line[LcdDisplay::MAXIMUM_COLUMNS + 1]; // Texto de una fila (+1 por '\0').
        display.setLine(0, "Smart LifeWatching");

        if (scaleFault) {
            display.setLine(1, "Weight: SCALE ERROR"); // Excepción visible en pantalla.
        } else {
            // En el Uno, snprintf no imprime decimales (%f): por eso se usa
            // dtostrf(valor, ancho, decimales, destino) para convertir el float.
            char weightText[8];
            dtostrf(weightSensor.getValue(), 5, 1, weightText);
            // snprintf arma el texto con formato sin pasarse del tamaño de "line".
            snprintf(line, sizeof(line), "Weight: %s kg%s", weightText, isOverweight() ? " >80" : "");
            display.setLine(1, line);
        }

        if (heartRateSignal) {
            snprintf(line, sizeof(line), "Heart rate: %3d BPM", beatsPerMinute()); // %3d = entero en 3 espacios.
            display.setLine(2, line);
        } else {
            display.setLine(2, "Heart rate: --- BPM");
        }

        snprintf(line, sizeof(line), "Status: %s", heartRateStatusText());
        display.setLine(3, line);
        display.handle(Command(Command::REFRESH)); // Escribe solo las filas cambiadas.
    }

public:
    // ---- 4. CONSTRUCTOR: crea cada componente. "this" = a quién avisan los sensores.
    // AnalogSensor(..., 0, MAXIMUM_BEATS_PER_MINUTE, ...) = el potenciómetro va de 0 a 200 BPM.
    SmartLifeWatchingDevice()
        : weightSensor(WEIGHT_SENSOR_ID, WEIGHT_DATA_PIN, WEIGHT_CLOCK_PIN, WEIGHT_CALIBRATION_FACTOR, this),
          heartRateSensor(HEART_RATE_SENSOR_ID, HEART_RATE_PIN, 0, MAXIMUM_BEATS_PER_MINUTE, this),
          overweightLed(OVERWEIGHT_LED_PIN),
          lowLed(LOW_LED_PIN),
          normalLed(NORMAL_LED_PIN),
          highLed(HIGH_LED_PIN),
          display(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS),
          samplingTimer(SAMPLING_INTERVAL_IN_MILLISECONDS),
          heartRateClassifier(NORMAL_MINIMUM_BEATS_PER_MINUTE, NORMAL_MAXIMUM_BEATS_PER_MINUTE) {}

    // ---- 5. setup()
    void setup() override {
        Serial.begin(SERIAL_BAUD_RATE);
        printCompanyInformation(); // Regla 4.

        overweightLed.begin();
        lowLed.begin();
        normalLed.begin();
        highLed.begin();
        display.begin();
        heartRateSensor.begin();
        // Menos de 30 BPM = sin señal -> el sensor avisará Event::FAULT.
        heartRateSensor.setValidRange(MINIMUM_VALID_BEATS_PER_MINUTE, MAXIMUM_BEATS_PER_MINUTE);

        // begin() devuelve false si el HX711 no responde -> modo falla.
        scaleFault = !weightSensor.begin();
        Serial.println(scaleFault ? F("[MODE] SCALE FAULT: HX711 not responding")
                                  : F("[MODE] MONITORING"));
    }

    // ---- 6. loop(): cada 0,5 s mide los dos sensores y refresca la pantalla.
    void loop() override {
        if (samplingTimer.isDue()) {
            weightSensor.measure();    // -> llamará a on() con el resultado.
            heartRateSensor.measure(); // -> llamará a on() con el resultado.
            updateDisplay();
        }
    }

    // ---- 7. on(): reglas del caso, una rama por sensor.
    void on(Event event) override {
        switch (event.sourceId) {
            case WEIGHT_SENSOR_ID:
                // Mensajes de cambio de modo (solo al pasar de un modo a otro).
                if (event.type == Event::FAULT && !scaleFault) {
                    Serial.println(F("[MODE] SCALE FAULT: HX711 not responding"));
                } else if (event.type == Event::MEASURED && scaleFault) {
                    Serial.println(F("[MODE] MONITORING"));
                }
                scaleFault = (event.type == Event::FAULT);
                overweightLed.setState(isOverweight()); // Regla 1.
                break;

            case HEART_RATE_SENSOR_ID:
                if (event.type == Event::MEASURED) {
                    updateHeartRate(true, heartRateClassifier.classify(beatsPerMinute())); // Regla 2.
                } else {
                    updateHeartRate(false, heartRateLevel); // Excepción: sin señal.
                }
                break;

            default:
                break; // Sensor desconocido: se ignora.
        }
    }
};

#endif // SMART_LIFE_WATCHING_DEVICE_H
