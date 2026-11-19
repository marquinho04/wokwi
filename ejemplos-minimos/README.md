# Ejemplos mínimos (versión para el examen)

Los mismos dos casos de `ejemplos/`, con el Device reducido a lo esencial. Usan los mismos archivos del kit y cumplen las mismas reglas, excepciones y restricciones (OOP, ModestIoT, nombres y comentarios en inglés, title blocks).

| Caso | Placa | Líneas de código que escribes (Device, sin comentarios) |
| --- | --- | --- |
| `cia-steel-faucet-esp32` (Moen) | ESP32 | ~50 |
| `smart-lifewatching-uno` (Smart Lifemonitor) | Arduino Uno | ~70 |

Qué se simplificó respecto de `ejemplos/`:
- Los componentes se crean en la misma línea donde se declaran (`Led proximityLed{LED_PIN};`), así no hace falta escribir un constructor.
- La consola imprime la empresa y tu nombre en una sola línea, y solo los cambios de estado.
- Menos funciones auxiliares: casi toda la lógica está en `on()`.

En el examen, sube los archivos del kit tal cual y escribe solo el Device y `sketch.ino`.
