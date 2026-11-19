# 5. Arduino Uno vs ESP32 en Wokwi

Las clases (`Device`, `Sensor`, `Actuator`, `Event`, `Command`) son iguales en las dos placas. Solo cambian los pines, la resolución analógica, la consola y `diagram.json`. Compara [`proyectos/arduino-uno`](../proyectos/arduino-uno) con [`proyectos/esp32`](../proyectos/esp32): solo cambian 6 archivos.

| Aspecto | Arduino Uno | ESP32 DevKit V1 |
| --- | --- | --- |
| Parte en Wokwi | `wokwi-arduino-uno` | `wokwi-esp32-devkit-v1` |
| Nombre de pines en `diagram.json` | `uno:8`, `uno:A0`, `uno:5V`, `uno:GND.1` | `esp:D4`, `esp:D34`, `esp:3V3`, `esp:VIN`, `esp:GND.1` |
| I2C (LCD) | SDA = A4, SCL = A5 | SDA = GPIO 21, SCL = GPIO 22 |
| Lectura analógica | 10 bits: 0–1023 | 12 bits: 0–4095 |
| Pines analógicos recomendados | A0–A5 | GPIO 32, 33, 34, 35 (34 y 35 son solo entrada) |
| Pines a evitar | 0 y 1 (Serial) | 6–11 (memoria flash); 0, 2, 12 y 15 tienen funciones de arranque |
| Nivel lógico | 5 V | 3,3 V (alimenta el LCD desde `VIN`) |
| Consola | Se conecta sola; 9600 baudios | Necesita los cables a `$serialMonitor`; lo usual es 115200 baudios |
| Constantes de pin | `A0` existe como constante | Se usa el número de GPIO: `34` |

## Qué cambia en el código

```cpp
// Arduino Uno
static const int HEART_RATE_SENSOR_PIN = A0;
static const int MAXIMUM_ANALOG_VALUE = 1023;   // 10-bit ADC
static const unsigned long SERIAL_BAUD_RATE = 9600;

// ESP32
static const int HEART_RATE_SENSOR_PIN = 34;    // ADC1, input only
static const int MAXIMUM_ANALOG_VALUE = 4095;   // 12-bit ADC
static const unsigned long SERIAL_BAUD_RATE = 115200;
```

Si te olvidas del cambio a 4095 en el ESP32, el potenciómetro llega al tope a un cuarto de recorrido y todas tus lecturas salen mal: un error típico de C03.

## Consola del ESP32 en `diagram.json`

```json
[ "esp:TX0", "$serialMonitor:RX", "", [] ],
[ "esp:RX0", "$serialMonitor:TX", "", [] ]
```

## Sobre la librería ModestIoT del profesor

El framework [Modest-IoT-Nano-Framework](https://github.com/modest-iot/modest-iot-nano-framework) (versión 1.1.2, en el Arduino Library Manager) **solo funciona en ESP32** (`architectures=esp32`) y depende de `ArduinoJson` y `MQTT`. Trae su propio `Device` con un motor asíncrono (FreeRTOS y timers), y clases listas como `Led`, `LoadCellAmplifier`, `PulseSensor` y `CharacterLcdDisplay`.

Los proyectos de este repositorio **no usan esa librería**: implementan sus mismas ideas (Device mediador, Sensor/Event, Actuator/Command) en clases propias y livianas. Así:

- el mismo código corre en Uno y en ESP32;
- se pudo compilar y probar fuera de Wokwi;
- se ve cómo funciona el patrón por dentro.

Si en tu clase usan la librería, la estructura de esta guía sigue valiendo, pero las firmas cambian (por ejemplo, el constructor de `Device` y la forma de registrar sensores). Revisa los ejemplos de la carpeta `examples/` del framework, como `SmartScaleDevice`.
