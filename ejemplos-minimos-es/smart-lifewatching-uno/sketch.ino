/*
  Smart LifeWatching

  Mide el peso del paciente (HX711) y su ritmo cardiaco (BPM), indica con
  LEDs el peso mayor a 80 kg y el estado del ritmo cardiaco, y muestra peso,
  BPM y estado en una LCD 2004. Programación orientada a objetos basada en
  Modest IoT: el Device es el mediador entre los sensores (eventos) y los
  actuadores (comandos).

  El circuito (Arduino Uno):
  * HX711 DT -> D3, SCK -> D2
  * Sensor de ritmo cardiaco (potenciómetro) SIG -> A0
  * LED de peso -> D8; LEDs bajo / normal / alto -> D9 / D10 / D11
  * LCD 2004 I2C (0x27): SDA -> A4, SCL -> A5

  Creado el 19 de noviembre de 2025
  Por <Developer Full Name> - Smart LifeMonitor, inc.

  https://wokwi.com/projects/<projectID>
*/

// El enunciado pide que sketch.ino solo cree el Device y llame a sus métodos.
#include "SmartLifeWatchingDevice.h"

// Objeto global: se crea antes de setup(). Su constructor arma los
// sensores y actuadores (todavía sin tocar el hardware).
SmartLifeWatchingDevice device;

// Arduino llama a setup() una vez al encender la placa.
void setup() {
    device.setup();  // Consola, empresa, begin() de cada componente.
}

// Arduino llama a loop() sin parar después de setup().
void loop() {
    device.loop();   // Temporizadores -> medir -> on() -> reglas -> actuadores.
}
