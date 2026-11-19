# Guía Wokwi para la PC2 de SI572 (Desarrollo de Soluciones IoT)

Guía paso a paso para crear un proyecto IoT en Wokwi con OOP y el enfoque ModestIoT, y para responder la Pregunta 1 (Application Service Layer Requirements). Incluye un **kit genérico** de clases para Arduino Uno y ESP32, aplicado a dos casos reales de PC2.

> Material de estudio no oficial, basado en los enunciados de PC2 2024-2 y 2025-2 del curso y en el framework público ModestIoT. Verifica siempre el enunciado y la rúbrica de tu propia evaluación.

## Contenido

| Ruta | Qué tiene |
| --- | --- |
| [docs/01-crear-proyecto-wokwi.md](docs/01-crear-proyecto-wokwi.md) | Uso de Wokwi: crear el proyecto, circuito, librerías, archivos, simular y exportar |
| [docs/02-de-caso-a-codigo.md](docs/02-de-caso-a-codigo.md) | **Receta para cualquier caso:** de la tabla de requisitos al Device, en 7 pasos |
| [docs/03-pregunta-1-service-layer.md](docs/03-pregunta-1-service-layer.md) | Cómo responder la Pregunta 1 con los 12 IoT System Design Steps |
| [docs/04-rubrica-y-checklist.md](docs/04-rubrica-y-checklist.md) | Rúbrica C01–C05 y checklist antes de entregar |
| [docs/05-arduino-uno-vs-esp32.md](docs/05-arduino-uno-vs-esp32.md) | Diferencias entre las dos placas |
| [docs/06-kit-generico.md](docs/06-kit-generico.md) | Referencia del kit: cada clase, su constructor, sus eventos y su librería |
| [kit/](kit) | **El kit genérico:** 8 archivos `.h` reutilizables en casi cualquier caso |
| [estudio/](estudio) | **Mismo código del kit y los ejemplos, con comentarios en español** línea por línea. Para estudiar, no para entregar |
| [ejemplos/](ejemplos) | El kit aplicado a 2 casos: Smart LifeWatching (Uno y ESP32) y Cia Steel Faucet de Moen (ESP32) |
| [caso/README.md](caso/README.md) | Caso Smart Lifemonitor: Pregunta 1 resuelta, decisiones de diseño y pruebas |
| [referencia-extendida/](referencia-extendida) | Primera versión de Smart LifeWatching (22 archivos, `.h` + `.cpp`), para estudiar el patrón con más detalle |

## El kit genérico

| Archivo | Componentes |
| --- | --- |
| `ModestIoT.h` | Núcleo: Event, Command, Sensor, Actuator, Device, PeriodicTimer, OneShotTimer, LevelClassifier |
| `AnalogSensor.h` | Potenciómetro, LDR, gas, humedad de suelo, pulso simulado |
| `DigitalSensor.h` | Botón, PIR, switch |
| `UltrasonicSensor.h` | HC-SR04 |
| `DhtSensor.h` | DHT22 |
| `WeightSensor.h` | HX711 |
| `DigitalActuator.h` | Led, Relay, Buzzer |
| `LcdDisplay.h` | LCD 1602 / 2004 por I2C |

En un caso nuevo solo escribes el **Device** (las reglas del caso), `sketch.ino` y el circuito. El kit funciona igual en Uno y en ESP32.

## Uso rápido

1. Lee las guías [01](docs/01-crear-proyecto-wokwi.md) y [02](docs/02-de-caso-a-codigo.md) y repasa el kit con la [guía 06](docs/06-kit-generico.md).
2. Prueba un ejemplo: crea un proyecto en Wokwi con la placa correspondiente, sube los archivos de la carpeta del ejemplo con **Upload file(s)...**, reemplaza `diagram.json` y agrega las librerías de su `libraries.txt`.
3. Practica con un caso nuevo: copia `ModestIoT.h` y los archivos del kit que necesites, escribe solo el Device y revísalo con el [checklist](docs/04-rubrica-y-checklist.md).

## Antes de usar el código en una entrega

Reemplaza los placeholders en todos los archivos:

- `<Developer Full Name>` por tus nombres y apellidos (title blocks, consola y `diagram.json`).
- `<u20XXXXXXX>` por tu código de estudiante.
- `<projectID>` en `sketch.ino` por el ID de tu proyecto Wokwi.

## Estado de verificación

El kit y los ejemplos se compilaron con stubs de Arduino (C++11 y C++17, en modo Uno y en modo ESP32) y se simularon con los casos de las tablas de prueba: todos dieron los resultados esperados. **No se ejecutaron dentro de Wokwi**, así que pruébalos ahí antes de entregar.
