# Guía Wokwi para la PC2 de SI572 (Desarrollo de Soluciones IoT)

Paso a paso para crear bien un proyecto IoT en Wokwi con OOP y el enfoque ModestIoT, cómo responder la Pregunta 1 (Application Service Layer Requirements) y un caso real resuelto en **Arduino Uno** y en **ESP32**.

> Material de estudio no oficial, basado en los enunciados de PC2 2024-2 y 2025-2 del curso y en el framework público ModestIoT. Verifica siempre el enunciado y la rúbrica de tu propia evaluación.

## Contenido

| Ruta | Qué tiene |
| --- | --- |
| [docs/01-crear-proyecto-wokwi.md](docs/01-crear-proyecto-wokwi.md) | Uso de Wokwi: crear el proyecto, circuito, librerías, archivos, simular y exportar |
| [docs/02-de-caso-a-codigo.md](docs/02-de-caso-a-codigo.md) | Método de 9 pasos para convertir el caso en clases (Device, Sensor, Actuator, Event, Command) |
| [docs/03-pregunta-1-service-layer.md](docs/03-pregunta-1-service-layer.md) | Cómo responder la Pregunta 1 con los 12 IoT System Design Steps |
| [docs/04-rubrica-y-checklist.md](docs/04-rubrica-y-checklist.md) | Rúbrica C01–C05 y checklist antes de entregar |
| [docs/05-arduino-uno-vs-esp32.md](docs/05-arduino-uno-vs-esp32.md) | Diferencias entre las dos placas y cómo adaptar el código |
| [plantilla/](plantilla) | 13 archivos base reutilizables en cualquier caso (Event, Command, Sensor, Actuator, Device, Led, LcdDisplay) |
| [caso/README.md](caso/README.md) | Caso Smart LifeWatching: resumen, Pregunta 1 resuelta, decisiones de diseño y pruebas |
| [proyectos/arduino-uno/](proyectos/arduino-uno) | Proyecto Wokwi completo para Arduino Uno |
| [proyectos/esp32/](proyectos/esp32) | El mismo proyecto adaptado a ESP32 |
| [plantilla-compacta/](plantilla-compacta) | **Versión para el examen:** 3 archivos base (`ModestIoT.h`, `Led.h`, `LcdDisplay.h`) |
| [proyectos-compactos/](proyectos-compactos) | **Versión para el examen:** el mismo caso en 6 `.h` + `sketch.ino`, para Uno y ESP32 |

## ¿Versión completa o compacta?

| | Completa (`proyectos/`) | Compacta (`proyectos-compactos/`) |
| --- | --- | --- |
| Archivos de código | 22 (`.h` + `.cpp` por clase) | 7 (todo en `.h` + `sketch.ino`) |
| Líneas | ~1250 | ~575 |
| Mismo comportamiento y mismas excepciones | Sí | Sí |
| Cumple OOP + ModestIoT + title blocks | Sí | Sí |
| Para qué sirve | Estudiar el patrón con detalle | Escribirlo en los 100 minutos del examen |

En la compacta, `ModestIoT.h` junta las abstracciones (Event, Command, EventHandler, CommandHandler, Sensor, Actuator, Device), cada clase va completa en su `.h` y los comandos se identifican con constantes `int`.

## Uso rápido

1. Lee las guías 01 y 02 y estudia [`plantilla/`](plantilla) hasta entender cada clase.
2. Abre un proyecto nuevo en Wokwi con la placa correspondiente, sube los archivos de `proyectos/arduino-uno` o `proyectos/esp32` con **Upload file(s)...** y reemplaza `diagram.json`.
3. Agrega las librerías `HX711 Arduino Library` y `LiquidCrystal I2C` desde el Library Manager.
4. Ejecuta y recorre la tabla de pruebas de [caso/README.md](caso/README.md#4-pruebas).
5. Para practicar, resuelve otro caso desde cero usando solo `plantilla/` y la guía 02, y revísalo con el checklist de la guía 04.

## Antes de usar el código en una entrega

Reemplaza los placeholders en todos los archivos:

- `<Developer Full Name>` por tus nombres y apellidos (title blocks, `diagram.json` y `SmartLifeWatchingDevice.cpp`).
- `<u20XXXXXXX>` por tu código de estudiante.
- `<projectID>` en `sketch.ino` por el ID de tu proyecto Wokwi.

## Estado de verificación

Las dos versiones se compilaron con stubs de Arduino (C++11 y C++17) y se simularon con los casos de la tabla de pruebas: dieron los resultados esperados. No se ejecutaron dentro de Wokwi, así que pruébalas ahí antes de entregar.
