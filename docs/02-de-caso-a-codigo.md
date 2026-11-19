# 2. Del caso al código con el kit genérico

La Pregunta 2 de la PC2 pide un prototipo en Wokwi con programación orientada a objetos "basado en ModestIoT". Con el [kit genérico](../kit) casi todo ya está escrito. Para un caso nuevo solo escribes **un archivo**: el Device con las reglas del caso, más `sketch.ino` y el circuito.

```
Device.loop() ──(cada X ms)──▶ sensor.measure() ──(lee hardware)──▶ Device.on(Event{sourceId, type})
Device.on()   ──(regla del caso)──▶ led.setState(...) / relay.setState(...) / display.handle(Command(REFRESH))
```

- **Sensor:** lee el hardware y avisa al Device con un `Event` que dice qué sensor fue (`sourceId`) y qué pasó (`MEASURED`, `CHANGED`, `FAULT`).
- **Actuator:** ejecuta un `Command` (`TURN_ON`, `TURN_OFF`, `TOGGLE`, `REFRESH`).
- **Device:** mediador. Tiene los sensores y actuadores y aplica las reglas en `on(Event)`.

## 2.1 Receta para cualquier caso (70 min)

### Paso 1. Tabla de requisitos (5 min)

Subraya cada número y cada componente del enunciado y arma esta tabla. Cada fila será código en `on(Event)` o en `loop()`.

| Entrada (sensor) | Regla | Salida (actuador) |
| --- | --- | --- |
| Distancia (HC-SR04) | < 10 cm | LED azul encendido |
| Distancia | Al llegar a < 10 cm | Relé abierto 5 s |
| — | Cada 2,5 s | Consola: distancia y estado de la válvula |
| Arranque | Una vez | Consola: empresa y tu nombre |

### Paso 2. Elige las clases del kit (2 min)

| Si el caso tiene... | Usa | Archivo |
| --- | --- | --- |
| Potenciómetro, LDR, gas, humedad de suelo, pulso simulado | `AnalogSensor` | `AnalogSensor.h` |
| Botón, PIR, switch, tilt | `DigitalSensor` | `DigitalSensor.h` |
| HC-SR04 (distancia) | `UltrasonicSensor` | `UltrasonicSensor.h` |
| DHT22 (temperatura y humedad) | `DhtSensor` | `DhtSensor.h` |
| HX711 (peso) | `WeightSensor` | `WeightSensor.h` |
| LED, relé, buzzer | `Led`, `Relay`, `Buzzer` | `DigitalActuator.h` |
| LCD 1602 o 2004 por I2C | `LcdDisplay` | `LcdDisplay.h` |
| "Cada X segundos" | `PeriodicTimer` | `ModestIoT.h` |
| "Durante X segundos" | `OneShotTimer` | `ModestIoT.h` |
| "Bajo / normal / alto" | `LevelClassifier` | `ModestIoT.h` |

Detalle de cada clase: [06-kit-generico.md](06-kit-generico.md).

### Paso 3. Arma y prueba el circuito (10 min)

Ver [01-crear-proyecto-wokwi.md](01-crear-proyecto-wokwi.md). Sube a Wokwi `ModestIoT.h` y solo los archivos del kit que vas a usar: cada uno incluye su librería, y un archivo sin su librería instalada no compila.

### Paso 4. Escribe el Device (30 min)

Copia el ejemplo más parecido y cambia lo que corresponda:
- [`ejemplos/cia-steel-faucet-esp32/CiaSteelFaucetDevice.h`](../ejemplos/cia-steel-faucet-esp32/CiaSteelFaucetDevice.h): ultrasónico, LED, relé con temporizador, reporte periódico.
- [`ejemplos/smart-lifewatching-uno/SmartLifeWatchingDevice.h`](../ejemplos/smart-lifewatching-uno/SmartLifeWatchingDevice.h): HX711, sensor analógico, 4 LEDs, LCD, niveles bajo/normal/alto.

Estructura del Device:

```cpp
class MyDevice : public Device {
private:
    // 1. Pines, identificadores de sensor y reglas como constantes con nombre
    static const int TEMPERATURE_SENSOR_ID = 1;
    static const int ALARM_LED_PIN = 8;
    static const int HIGH_TEMPERATURE_IN_CELSIUS = 8;

    // 2. Un objeto por sensor, actuador y temporizador
    DhtSensor temperatureSensor;
    Led alarmLed;
    PeriodicTimer samplingTimer;

public:
    // 3. Constructor: pasa `this` a los sensores para recibir sus eventos
    MyDevice()
        : temperatureSensor(TEMPERATURE_SENSOR_ID, 15, this),
          alarmLed(ALARM_LED_PIN),
          samplingTimer(2000) {}

    // 4. setup(): Serial, empresa y tu nombre, begin() de cada componente
    void setup() override { /* ... */ }

    // 5. loop(): solo temporizadores, sin delay()
    void loop() override {
        if (samplingTimer.isDue()) {
            temperatureSensor.measure();
        }
    }

    // 6. on(): las reglas del caso, una rama por sensor
    void on(Event event) override {
        switch (event.sourceId) {
            case TEMPERATURE_SENSOR_ID:
                if (event.type == Event::FAULT) { /* mensaje de error */ }
                else alarmLed.setState(temperatureSensor.getValue() > HIGH_TEMPERATURE_IN_CELSIUS);
                break;
            default:
                break;
        }
    }
};
```

(En el código final, el `15` y el `2000` del constructor también van como constantes con nombre.)

Reglas que suman puntos:
- **Excepciones:** maneja siempre `Event::FAULT` con un mensaje en consola y, si hay, en el LCD. Pon los actuadores en un estado seguro.
- **Cambios de estado:** imprime en consola solo cuando algo cambia (`[STATE] NORMAL -> HIGH`), no cada lectura. Es lo que pide "demostrar los modos de operación y actualización de estado".
- **Límites exactos:** "60 a 100" incluye 60 y 100 (`LevelClassifier` ya lo hace así). "Mayor a 80" no incluye 80: usa `>`.
- **Nada de `delay()` en `loop()`:** usa `PeriodicTimer` y `OneShotTimer`.

### Paso 5. sketch.ino (1 min)

```cpp
#include "MyDevice.h"

MyDevice device;

void setup() {
    device.setup();
}

void loop() {
    device.loop();
}
```

### Paso 6. Prueba los casos límite (10 min)

Por cada regla prueba el valor exacto del límite, uno un poco menor, uno un poco mayor y la falla del sensor. Ejemplos de tablas de prueba: [caso/README.md](../caso/README.md#4-pruebas) y [06-kit-generico.md](06-kit-generico.md#ejemplo-2-cia-steel-faucet-moen-esp32).

### Paso 7. Pulido para C04 y C05 (10 min)

- Title block en **cada** archivo, con tu nombre en el campo `By`. Si el caso es de otra empresa, agrégala también:
  ```cpp
  /*
    CiaSteelFaucetDevice.h
    Cia Steel Faucet - Embedded Application (Modest IoT based)

    Touchless faucet: ...

    Created 19 November 2025
    By Juan Perez - Moen, inc.
  */
  ```
- En los archivos del kit, cambia `Generic IoT kit` por el nombre del dispositivo del caso.
- Nombres en inglés: `PascalCase` para clases, `camelCase` para métodos y variables, `UPPER_SNAKE_CASE` para constantes.
- Borra el código comentado y las variables sin uso, y no subas archivos del kit que no uses.

## 2.2 Por qué esta estructura suma puntos

| Lo que hace el código | Criterio de la rúbrica |
| --- | --- |
| Mediador (Device), herencia Sensor/Actuator con método `read()`/`handle()` sobrescrito, Event/Command | C04: patrones de arquitectura y de diseño |
| Una clase por tipo de sensor y actuador | Technical constraint: "clases y objetos para representar... sensors y actuators" |
| Constantes con nombre, `LevelClassifier` y temporizadores reutilizables | C04: sin redundancia |
| `Event::FAULT` manejado en cada sensor | C03: respuesta ante excepciones |
| Nombres en inglés y convenciones consistentes | C05 |
