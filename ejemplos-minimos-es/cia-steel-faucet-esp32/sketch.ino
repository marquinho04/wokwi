/*
  Cia Steel Faucet

  Grifo sin contacto: un sensor ultrasónico detecta las manos del usuario,
  un LED azul muestra la detección y un relé abre la válvula de agua
  durante 5 segundos. Programación orientada a objetos basada en Modest IoT:
  el Device es el mediador entre los sensores (eventos) y los actuadores
  (comandos).

  El circuito (ESP32 DevKit V1):
  * HC-SR04 TRIG -> GPIO 5, ECHO -> GPIO 18
  * LED azul (con resistencia de 220 ohm) -> GPIO 4
  * Módulo relé IN -> GPIO 23

  Creado el 19 de noviembre de 2025
  Por <Developer Full Name> - Moen, inc.

  https://wokwi.com/projects/<projectID>
*/

// El enunciado pide que sketch.ino solo cree el Device y llame a sus métodos.
#include "CiaSteelFaucetDevice.h"

// Objeto global: se crea antes de setup(). Su constructor arma los
// sensores y actuadores (todavía sin tocar el hardware).
CiaSteelFaucetDevice device;

// Arduino llama a setup() una vez al encender la placa.
void setup() {
    device.setup();  // Consola, empresa, begin() de cada componente.
}

// Arduino llama a loop() sin parar después de setup().
void loop() {
    device.loop();   // Temporizadores -> medir -> on() -> reglas -> actuadores.
}
