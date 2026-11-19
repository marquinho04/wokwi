# Copia de estudio (comentarios en español)

Estos archivos son **idénticos en código** a los de [`kit/`](../kit) y [`ejemplos/`](../ejemplos), pero con comentarios en español que explican qué hace cada parte, por qué, y la sintaxis de C++ que aparece.

> **No entregues estos archivos.** El enunciado exige comentarios en inglés ("Documente con comentarios en inglés sus archivos de código fuente") y C04 lo evalúa. Estudia aquí y entrega las versiones de `kit/` y `ejemplos/`.

## Orden de lectura sugerido

| # | Archivo | Qué aprendes |
| --- | --- | --- |
| 1 | `sketch.ino` | Cómo arranca todo |
| 2 | `ModestIoT.h` | Event, Command, Sensor, Actuator, Device, temporizadores, clasificador; herencia, `virtual`, `override` |
| 3 | `DigitalActuator.h` | Cómo un actuador obedece comandos; polimorfismo (Buzzer) |
| 4 | `UltrasonicSensor.h` | Cómo un sensor lee hardware y avisa, con manejo de falla |
| 5 | `CiaSteelFaucetDevice.h` | Un Device completo y simple: reglas, temporizadores, consola |
| 6 | `AnalogSensor.h`, `LcdDisplay.h`, `WeightSensor.h` | Los componentes del caso Smart Lifemonitor |
| 7 | `SmartLifeWatchingDevice.h` | Un Device con LCD, varios LEDs y niveles bajo/normal/alto |
| 8 | `DigitalSensor.h`, `DhtSensor.h` | Botones/PIR y temperatura, para otros casos |
