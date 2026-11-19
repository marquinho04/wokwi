/*
  sketch.ino  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  El enunciado pide que sketch.ino SOLO cree el Device y llame a sus
  métodos. Toda la lógica vive en las clases.

  Arduino llama a setup() una vez al encender y luego a loop() sin parar.
  Aquí solo se "pasa la posta" al Device.

  Para el caso Moen sería igual, cambiando el include y la clase:
    #include "CiaSteelFaucetDevice.h"
    CiaSteelFaucetDevice device;
*/

#include "SmartLifeWatchingDevice.h"

// Objeto global: se crea antes de setup(). Su constructor arma los
// sensores y actuadores (todavía sin tocar el hardware).
SmartLifeWatchingDevice device;

void setup() {
    device.setup();  // Consola, empresa, begin() de cada componente.
}

void loop() {
    device.loop();   // Temporizadores -> medir -> on() -> reglas -> actuadores.
}
