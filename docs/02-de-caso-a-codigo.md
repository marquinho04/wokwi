# 2. Del caso al código (OOP + ModestIoT)

La Pregunta 2 de la PC2 pide un prototipo en Wokwi con programación orientada a objetos "basado en ModestIoT". En la práctica significa:

- Un **Device** que actúa como mediador: tiene los sensores y actuadores, recibe eventos y decide.
- **Sensors** que leen el hardware y emiten **Events** (`on(Event)`).
- **Actuators** que ejecutan **Commands** (`handle(Command)`), al estilo CQRS: los eventos informan, los comandos ordenan.
- Un `sketch.ino` que solo crea el Device y llama `setup()` y `loop()`.

```
Device.loop()  ──MEASURE_DATA_REQUESTED──▶  Sensor.on()  ──(lee hardware)──┐
      ▲                                                                   │
      └──────────────── Device.on(WEIGHT_MEASURED / FAULT ...) ◀───────────┘
Device.on()  ──(regla de negocio)──▶  led.handle(TURN_ON_COMMAND) / display.handle(REFRESH_COMMAND)
```

## 2.1 Paso a paso

### Paso 1. Extrae los requisitos del caso en una tabla (5 min)

Subraya cada número y cada componente del enunciado. Ejemplo con Smart LifeWatching:

| Entrada (sensor) | Regla | Salida (actuador) |
| --- | --- | --- |
| Peso (HX711) | > 80 kg | LED azul de peso encendido |
| Ritmo cardiaco | < 60 / 60–100 / > 100 PPM | LED azul / verde / rojo |
| Peso y ritmo | Siempre | LCD 2004: peso, PPM y estado |
| Arranque | Una vez | Consola: datos de la empresa y tu nombre |

Cada fila de esta tabla será un `case` en `Device::on()`. Si una fila no queda en el código, pierdes puntos en C03.

### Paso 2. Arma y prueba el circuito (10 min)

Ver [01-crear-proyecto-wokwi.md](01-crear-proyecto-wokwi.md), secciones 1.2 y 1.3.

### Paso 3. Copia la plantilla (2 min)

La carpeta [`plantilla/`](../plantilla) tiene las 13 clases que no cambian entre casos: `Event`, `EventHandler`, `Command`, `CommandHandler`, `Sensor`, `Actuator`, `Device`, `Led` y `LcdDisplay`. Súbelas con **Upload file(s)...**. Llévala preparada al examen y entiende cada archivo antes.

### Paso 4. Una clase por sensor nuevo (15 min)

Hereda de `Sensor` y escribe la lectura en `processEvent`. Copia `HeartRateSensor` o `WeightSensor` como modelo:

```cpp
void TemperatureSensor::processEvent(Event& event) {
    if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;
    float reading = dht.readTemperature();
    if (isnan(reading)) {                       // exception: sensor fault
        event.identifier = SENSOR_FAULT_EVENT_IDENTIFIER;
        return;
    }
    temperatureInCelsius = reading;
    event.identifier = TEMPERATURE_MEASURED_EVENT_IDENTIFIER;
}
```

Reglas:
- Cada sensor define sus propios identificadores de evento (`static const int ..._EVENT_IDENTIFIER`), distintos entre sí.
- La lectura válida se guarda en un atributo con getter (`getTemperatureInCelsius()`).
- Si el hardware puede fallar o dar un valor imposible, emite un evento de falla. Eso es lo que la rúbrica llama "responder ante excepciones".

### Paso 5. Las reglas de negocio en su propia clase (5 min)

Si hay rangos (bajo/normal/alto), crea un `enum class` y un clasificador, como [`HeartRateStatus.h`](../proyectos/arduino-uno/HeartRateStatus.h). Así el Device no queda lleno de `if` y los límites están en constantes con nombre.

Revisa los límites con el texto exacto: "60 a 100" incluye 60 y 100; "mayor a 80" no incluye 80.

Ojo: no llames `LOW` o `HIGH` a un valor del enum, porque Arduino ya los define como macros. Usa `Low` y `High`.

### Paso 6. El Device mediador (20 min)

Copia la estructura de [`SmartLifeWatchingDevice`](../proyectos/arduino-uno/SmartLifeWatchingDevice.cpp):

1. **Atributos:** un objeto por sensor y actuador. Pasa `this` como manejador para que te envíen sus eventos.
2. **`setup()`:** abre `Serial`, imprime la empresa y tu nombre, y llama `begin()` de cada componente.
3. **`loop()`:** sin `delay()`. Usa `millis()` para pedir lecturas cada cierto intervalo:
   ```cpp
   if (now - lastSamplingTimestamp >= SAMPLING_INTERVAL_IN_MILLISECONDS) {
       lastSamplingTimestamp = now;
       weightSensor.on(Sensor::MEASURE_DATA_REQUESTED_EVENT);
   }
   ```
4. **`on(Event)`:** un `switch` por identificador de evento, con una fila de la tabla del Paso 1 por `case`.
5. **Consola:** imprime cada cambio de estado o modo (`[STATE] NORMAL -> HIGH`), no cada lectura. Es lo que pide "demostrar los modos de operación y actualización de estado".
6. **Actuadores:** `led.setState(condición)` solo envía el comando si el estado cambia, así evitas comandos repetidos.

### Paso 7. sketch.ino (1 min)

```cpp
#include "SmartLifeWatchingDevice.h"

SmartLifeWatchingDevice device;

void setup() {
    device.setup();
}

void loop() {
    device.loop();
}
```

### Paso 8. Probar los casos límite (10 min)

Haz una tabla de prueba (valor exacto del límite, un poco menos, un poco más, falla del sensor) y verifica cada fila en el simulador. Ejemplo: [caso/README.md](../caso/README.md#4-pruebas).

### Paso 9. Pulir para C04 y C05 (10 min)

- Pon un title block al inicio de **cada** archivo: nombre del archivo, descripción, conexiones, fecha y autor. Formato del Arduino Style Guide:
  ```cpp
  /*
    Led.h
    Smart LifeWatching - Embedded Application (Modest IoT based)

    LED actuator controlled through TURN_ON / TURN_OFF commands.

    Created 19 November 2025
    By Juan Perez - Smart LifeMonitor, inc.
  */
  ```
- Comenta en inglés cada clase y método público (estilo Doxygen `/** @brief ... */`).
- Nombres en inglés: `PascalCase` para clases, `camelCase` para métodos y variables, `UPPER_SNAKE_CASE` para constantes. Nada de `pinLed1` ni `sensorPeso`.
- Ningún número suelto en el código: cada pin, umbral e intervalo va como constante con nombre.
- Borra el código comentado, las variables sin uso y los `delay()` en `loop()`.

## 2.2 Por qué esta estructura suma puntos

| Lo que hace el código | Criterio de la rúbrica |
| --- | --- |
| Mediador (Device), template method (`processEvent`/`executeCommand`), CQRS (Event/Command) | C04: patrones y buenas prácticas |
| Clasificador separado y constantes con nombre | C04: sin redundancia ni ineficiencias |
| Eventos de falla con mensaje en LCD y consola | C03: respuesta ante excepciones |
| `millis()` en vez de `delay()` | C03/C04: el device sigue respondiendo mientras espera |
| Nombres en inglés y convenciones consistentes | C05 |
