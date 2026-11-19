# 6. Kit genérico: referencia

El [kit](../kit) son 8 archivos `.h` que cubren los componentes más comunes de los casos del curso. **El mismo código funciona en Arduino Uno y en ESP32**: `ModestIoT.h` detecta la placa y ajusta la resolución analógica (1023 en el Uno, 4095 en el ESP32). Para un caso nuevo solo cambian el Device, los pines y `diagram.json`.

## Catálogo

### Núcleo: `ModestIoT.h` (siempre se sube)

| Clase | Para qué | Uso |
| --- | --- | --- |
| `Event` | Aviso de un sensor al Device | `event.sourceId` (qué sensor) y `event.type`: `Event::MEASURED`, `Event::CHANGED`, `Event::FAULT` |
| `Command` | Orden a un actuador | `Command(Command::TURN_ON)`, `TURN_OFF`, `TOGGLE`, `REFRESH`, `CLEAR` |
| `Sensor` | Base de los sensores | `begin()`, `measure()`, `getValue()` |
| `Actuator` | Base de los actuadores | `begin()`, `handle(Command)` |
| `Device` | Base del dispositivo | Implementas `setup()`, `loop()` y `on(Event)` |
| `PeriodicTimer` | "Cada X ms" | `PeriodicTimer timer(2500);` … `if (timer.isDue()) {...}` |
| `OneShotTimer` | "Durante X ms" | `start()`, `hasExpired()` (true una sola vez), `stop()`, `isRunning()` |
| `LevelClassifier` | Bajo / normal / alto | `LevelClassifier c(60, 100);` → `c.classify(v)` da `Level::Low`, `Normal` o `High` (límites incluidos en Normal); `LevelClassifier::toText(level)` |

### Sensores

| Clase | Constructor | `getValue()` | Eventos | Librería Wokwi |
| --- | --- | --- | --- | --- |
| `AnalogSensor` | `(id, pin, minimum, maximum, this)` | Valor físico entre `minimum` y `maximum` | `MEASURED`; `FAULT` fuera de `setValidRange(min, max)` | — |
| `DigitalSensor` | `(id, pin, activeLow, this)` | 1 activo / 0 inactivo; también `isActive()` | `CHANGED`, solo cuando cambia | — |
| `UltrasonicSensor` | `(id, triggerPin, echoPin, this)` | Distancia en cm | `MEASURED`; `FAULT` si no hay eco | — |
| `DhtSensor` | `(id, pin, this)` | Temperatura °C; también `getHumidity()` | `MEASURED`; `FAULT` si la lectura es NaN | `DHT sensor library` |
| `WeightSensor` | `(id, dataPin, clockPin, countsPerKilogram, this)` | Peso en kg (negativos → 0) | `MEASURED` cuando hay dato listo; `FAULT` si no llegan datos durante 2 s | `HX711 Arduino Library` |

Notas:
- `AnalogSensor` sirve para todo lo que Wokwi simula con un valor analógico: potenciómetro, fotoresistencia (LDR), sensor de gas, humedad de suelo, o un sensor que Wokwi no tiene (como el de pulso), simulado con un potenciómetro.
- `DigitalSensor` con `activeLow = true` para un botón conectado entre el pin y GND (usa la resistencia interna de pull-up). Con `false` para un PIR, que entrega HIGH al detectar.
- `WeightSensor` con el HX711 "50kg" de Wokwi: 420 cuentas por kg es la escala real; 105 simula una plataforma de 200 kg (control en 20 kg = 80 kg). No espera al HX711 ni hace tara al arrancar: en Wokwi la balanza ya marca 0 con 0 kg, así que el peso siempre corresponde al control.
- Si `DhtSensor` da error de compilación por `Adafruit_Sensor.h`, agrega también `Adafruit Unified Sensor` en el Library Manager.

### Actuadores

| Clase | Constructor | Uso |
| --- | --- | --- |
| `Led` | `(pin)` | `begin()`, `setState(bool)`, `isOn()`, `handle(Command(Command::TOGGLE))` |
| `Relay` | `(pin)` | Igual que `Led`: válvula, bomba, cerradura |
| `Buzzer` | `(pin, frequencyInHertz)` | Igual que `Led`; suena con `tone()` mientras está encendido |
| `LcdDisplay` | `(i2cAddress, columns, rows)` | `begin()`, `setLine(row, text)`, `handle(Command(Command::REFRESH))`; 16x2 o 20x4, dirección 0x27 en Wokwi; librería `LiquidCrystal I2C` |

`setState()` solo envía el comando cuando el estado cambia. `LcdDisplay` solo reescribe las filas que cambiaron, así que puedes llamar a `REFRESH` en cada ciclo sin parpadeo.

## Ejemplos incluidos

### Ejemplo 1: Smart LifeWatching (Smart Lifemonitor, Uno y ESP32)

[`ejemplos/smart-lifewatching-uno`](../ejemplos/smart-lifewatching-uno) y [`ejemplos/smart-lifewatching-esp32`](../ejemplos/smart-lifewatching-esp32). Usa `WeightSensor`, `AnalogSensor` (pulso de 0 a 200 PPM, válido desde 30), 4 `Led`, `LcdDisplay` 20x4, `PeriodicTimer` y `LevelClassifier(60, 100)`. Las dos carpetas tienen los mismos archivos del kit: solo cambian los pines, el baud rate y `diagram.json`. Pruebas en [caso/README.md](../caso/README.md#4-pruebas).

### Ejemplo 2: Cia Steel Faucet (Moen, ESP32)

[`ejemplos/cia-steel-faucet-esp32`](../ejemplos/cia-steel-faucet-esp32), caso de la PC2 2024-2 del recopilatorio. Usa `UltrasonicSensor`, `Led`, `Relay`, dos `PeriodicTimer` (muestreo cada 200 ms y reporte cada 2,5 s) y un `OneShotTimer` de 5 s para la válvula. No necesita librerías externas.

Interpretación de las reglas del enunciado:
- "Traspasa el threshold de 10 cm" se toma como distancia **menor** a 10 cm. Exactamente 10 cm no activa.
- La válvula se abre una vez cada vez que las manos llegan al umbral, y se cierra a los 5 s aunque las manos sigan cerca (para no desperdiciar agua). Para volver a abrirla, hay que alejar las manos y acercarlas de nuevo.
- El caso menciona conexión WiFi, pero ninguna regla depende de ella, así que no se implementó.

Pruebas (simuladas con stubs; mueve la distancia del HC-SR04 en Wokwi):

| Acción | LED | Válvula (relé) |
| --- | --- | --- |
| 50 cm | Apagado | Cerrada |
| Exactamente 10 cm | Apagado | Cerrada |
| 8 cm | Encendido | Abierta (`[VALVE] OPEN for 5 s`) |
| Manos cerca 5 s después | Encendido | Cerrada (`[VALVE] CLOSED`), no se reabre |
| Alejar a 40 cm y volver a 5 cm | Apagado → encendido | Se abre de nuevo |
| Sensor sin eco | Apagado | Termina sus 5 s y se cierra |
| Siempre | — | `[REPORT] Proximity: … cm \| Valve: OPEN/CLOSED` cada 2,5 s |

## Agregar un componente que no está en el kit

Si el caso trae algo nuevo (servo, LED RGB, sensor de llama...), crea una clase igual que las del kit:

```cpp
// Sensor nuevo: hereda de Sensor e implementa read()
class FlameSensor : public Sensor {
protected:
    int read() override {
        value = analogRead(pin);
        return Event::MEASURED;   // o Event::FAULT si la lectura no es válida
    }
public:
    FlameSensor(int sensorId, int signalPin, EventHandler* handler)
        : Sensor(sensorId, signalPin, handler) {}
};

// Actuador nuevo: hereda de Actuator e implementa handle()
class RgbLed : public Actuator { /* begin() con pinMode y handle(Command) */ };
```

Ojo con el servo: en el Uno se usa la librería `Servo` y en el ESP32 `ESP32Servo`. Por eso no está en el kit.

## Límites del kit

- Está pensado para un solo archivo compilado (`sketch.ino` incluye todos los `.h`), que es como funciona Wokwi.
- No incluye WiFi ni MQTT. Si el caso exige enviar datos a la nube, eso va aparte.
- Los ejemplos se compilaron con stubs de Arduino (C++11 y C++17, Uno y ESP32) y se simularon caso por caso. **No se ejecutaron dentro de Wokwi:** pruébalos ahí antes de usarlos en una entrega.
