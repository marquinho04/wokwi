/*
  ModestIoT.h
  Smart LifeWatching - Aplicación embebida (basada en Modest IoT)

  Es el "corazón" del kit. Define las piezas que usan todos los demás
  archivos. La idea de ModestIoT es separar responsabilidades:

    Sensor  --(Event)-->  Device  --(Command)-->  Actuator
    (lee)                 (decide)                (actúa)

  - Un Sensor solo sabe leer su hardware y avisar ("pasó algo").
  - Un Actuator solo sabe obedecer órdenes ("prende", "apaga").
  - El Device es el "mediador": recibe los avisos, aplica las reglas del
    caso y da las órdenes. Los sensores y actuadores no se conocen entre sí.

  Creado el 19 de noviembre de 2025
  Por <Developer Full Name> - Smart LifeMonitor, inc.
*/

// "Include guard": evita que este archivo se copie dos veces si varios .h lo
// incluyen. Sin esto el compilador diría "redefinition of class ...".
#ifndef MODEST_IOT_H
#define MODEST_IOT_H

#include <Arduino.h>   // Funciones de Arduino: pinMode, digitalWrite, millis, Serial...

// ---------------------------------------------------------------------------
// Resolución analógica según la placa.
// analogRead() devuelve 0..1023 en el Uno (10 bits) y 0..4095 en el ESP32
// (12 bits). Wokwi define ESP32 automáticamente cuando la placa es un ESP32,
// así que el mismo código sirve en las dos sin tocar nada.
// ---------------------------------------------------------------------------
#if defined(ESP32)
static const int ANALOG_MAXIMUM_VALUE = 4095;
#else
static const int ANALOG_MAXIMUM_VALUE = 1023;
#endif

// ---------------------------------------------------------------------------
// Event: el "aviso" que un sensor le manda al Device.
// Lleva dos datos: QUIÉN avisó (sourceId) y QUÉ pasó (type).
// "struct" es igual que "class", pero todo es público por defecto.
// ---------------------------------------------------------------------------
struct Event {
    // "static const" = constante que pertenece al tipo, no a cada objeto.
    // Se usa así: Event::MEASURED
    static const int NONE = 0;     // No pasó nada nuevo: no se envía aviso.
    static const int MEASURED = 1; // Hay una lectura nueva y válida.
    static const int CHANGED = 2;  // Un sensor on/off cambió de estado (botón, PIR).
    static const int FAULT = 3;    // Error: sensor desconectado o lectura imposible.

    int sourceId; // Identificador del sensor que avisa (lo eliges tú en el Device).
    int type;     // Uno de los valores de arriba.

    // Constructor. Lo de después de ":" es la "lista de inicialización":
    // copia los parámetros a los atributos antes de entrar al cuerpo { }.
    Event(int source, int eventType) : sourceId(source), type(eventType) {}
};

// ---------------------------------------------------------------------------
// Command: la "orden" que el Device le da a un actuador.
// ---------------------------------------------------------------------------
struct Command {
    static const int TURN_ON = 1;  // Encender (LED, relé, buzzer).
    static const int TURN_OFF = 2; // Apagar.
    static const int TOGGLE = 3;   // Invertir: si está prendido lo apaga y viceversa.
    static const int REFRESH = 4;  // LCD: escribir en pantalla el texto preparado.
    static const int CLEAR = 5;    // LCD: borrar la pantalla.

    int type;

    // "explicit" evita conversiones automáticas raras (un int no se vuelve
    // Command sin que lo escribas). Se usa así: Command(Command::TURN_ON)
    explicit Command(int commandType) : type(commandType) {}
};

// ---------------------------------------------------------------------------
// EventHandler: "interfaz" = contrato. Quien herede de aquí PROMETE tener
// un método on(Event). El Device hereda de aquí para poder recibir avisos.
// "virtual ... = 0" significa "método puro": no tiene código aquí, cada
// clase hija está obligada a escribirlo.
// ---------------------------------------------------------------------------
class EventHandler {
public:
    virtual void on(Event event) = 0;
    virtual ~EventHandler() {} // Destructor virtual: buena práctica en clases base.
};

// Lo mismo, pero para quien recibe órdenes (los actuadores).
class CommandHandler {
public:
    virtual void handle(Command command) = 0;
    virtual ~CommandHandler() {}
};

// ---------------------------------------------------------------------------
// Sensor: clase base de TODOS los sensores del kit.
// Implementa la parte común (guardar el pin, avisar al Device) y deja a
// cada sensor concreto solo la parte distinta: cómo se lee (read()).
// Esto se llama patrón "Template Method": measure() es la plantilla fija,
// read() es el hueco que llena cada hijo.
// ---------------------------------------------------------------------------
class Sensor {
protected: // "protected": lo ven esta clase y sus hijas, pero no el resto.
    int id;                     // Identificador para que el Device sepa quién avisó.
    int pin;                    // Pin principal del sensor.
    EventHandler* eventHandler; // Puntero a quien recibe los avisos (el Device).
    float value = 0.0f;         // Última lectura válida (cm, kg, °C, BPM...).

    // Cada sensor hijo escribe cómo leer su hardware. Debe:
    //  1) actualizar "value" si la lectura es válida, y
    //  2) devolver el tipo de evento: Event::MEASURED, CHANGED, FAULT o NONE.
    virtual int read() = 0;

public:
    Sensor(int sensorId, int signalPin, EventHandler* handler)
        : id(sensorId), pin(signalPin), eventHandler(handler) {}
    virtual ~Sensor() {}

    // Configura el hardware (pinMode, iniciar librería...). Se llama desde
    // Device::setup(). Por defecto no hace nada; los hijos lo "sobrescriben"
    // (override) si lo necesitan. Devuelve false si el sensor no responde.
    virtual bool begin() { return true; }

    // Lo que el Device llama periódicamente: lee y, si hay algo nuevo, avisa.
    void measure() {
        int eventType = read();                    // El hijo lee el hardware.
        if (eventType != Event::NONE && eventHandler != nullptr) {
            eventHandler->on(Event(id, eventType)); // Avisa al Device: on(...)
        }
    }

    // "const" al final = este método no modifica el objeto (solo consulta).
    float getValue() const { return value; }
    int getId() const { return id; }
};

// ---------------------------------------------------------------------------
// Actuator: clase base de todos los actuadores. Hereda de CommandHandler,
// así que cada actuador hijo está obligado a tener handle(Command).
// ---------------------------------------------------------------------------
class Actuator : public CommandHandler {
protected:
    int pin;

public:
    explicit Actuator(int signalPin) : pin(signalPin) {}

    // Configura el pin. Se llama desde Device::setup().
    virtual void begin() {}
};

// ---------------------------------------------------------------------------
// Device: clase base del dispositivo. Tu Device del caso hereda de aquí y
// escribe tres métodos:
//   setup() -> se llama una vez al arrancar (desde setup() de sketch.ino)
//   loop()  -> se llama sin parar (desde loop() de sketch.ino)
//   on()    -> se llama cada vez que un sensor avisa (heredado de EventHandler)
// ---------------------------------------------------------------------------
class Device : public EventHandler {
public:
    virtual void setup() = 0;
    virtual void loop() = 0;
};

// ---------------------------------------------------------------------------
// PeriodicTimer: para reglas tipo "cada 2,5 segundos".
// ¿Por qué no delay()? Porque delay() congela TODO el programa: mientras
// espera, no lee sensores ni responde. millis() solo mira el reloj y deja
// que loop() siga corriendo ("código no bloqueante").
// ---------------------------------------------------------------------------
class PeriodicTimer {
private:
    unsigned long intervalInMilliseconds; // Cada cuánto debe cumplirse.
    unsigned long lastTimestamp = 0;      // Última vez que se cumplió.

public:
    explicit PeriodicTimer(unsigned long interval) : intervalInMilliseconds(interval) {}

    // Devuelve true UNA vez cada vez que pasa el intervalo. Uso:
    //   if (reportTimer.isDue()) { imprimir... }
    bool isDue() {
        unsigned long now = millis(); // Milisegundos desde que arrancó la placa.
        // La resta funciona bien incluso cuando millis() se reinicia (~49 días).
        if (now - lastTimestamp >= intervalInMilliseconds) {
            lastTimestamp = now;
            return true;
        }
        return false;
    }
};

// ---------------------------------------------------------------------------
// OneShotTimer: para reglas tipo "abrir la válvula DURANTE 5 segundos".
// start() lo arranca; hasExpired() devuelve true una sola vez al terminar.
// ---------------------------------------------------------------------------
class OneShotTimer {
private:
    unsigned long durationInMilliseconds;
    unsigned long startTimestamp = 0;
    bool running = false; // ¿Está contando ahora mismo?

public:
    explicit OneShotTimer(unsigned long duration) : durationInMilliseconds(duration) {}

    void start() {
        startTimestamp = millis();
        running = true;
    }

    void stop() { running = false; } // Cancela la cuenta.

    bool isRunning() const { return running; }

    // Se consulta en cada loop(). Solo da true justo cuando se cumple el tiempo.
    bool hasExpired() {
        if (running && millis() - startTimestamp >= durationInMilliseconds) {
            running = false; // Para que no vuelva a dar true.
            return true;
        }
        return false;
    }
};

// ---------------------------------------------------------------------------
// Level + LevelClassifier: para reglas tipo "menor a 60 bajo, 60 a 100
// normal, mayor a 100 alto". "enum class" es una lista cerrada de valores
// con nombre; se usa así: Level::Low. (No se llaman LOW/HIGH porque esos
// nombres ya los usa Arduino para los pines.)
// ---------------------------------------------------------------------------
enum class Level { Low, Normal, High };

class LevelClassifier {
private:
    float lowLimit;  // Por debajo de esto: Low.
    float highLimit; // Por encima de esto: High.

public:
    LevelClassifier(float lowerLimit, float upperLimit) : lowLimit(lowerLimit), highLimit(upperLimit) {}

    // Los límites cuentan como Normal: con (60, 100), 60 y 100 son Normal.
    Level classify(float measuredValue) const {
        if (measuredValue < lowLimit) return Level::Low;
        if (measuredValue > highLimit) return Level::High;
        return Level::Normal;
    }

    // Texto para mostrar en consola o LCD. "static" = se llama sin objeto:
    // LevelClassifier::toText(level)
    static const char* toText(Level level) {
        switch (level) {
            case Level::Low:  return "LOW";
            case Level::High: return "HIGH";
            default:          return "NORMAL";
        }
    }
};

#endif // MODEST_IOT_H  (fin del include guard)
