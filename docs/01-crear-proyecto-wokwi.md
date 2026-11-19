# 1. Crear bien un proyecto en Wokwi

Wokwi es un simulador en el navegador: armas el circuito, escribes el código C++ (Arduino sketch) y lo ejecutas sin hardware. Esta guía cubre el uso del simulador. La forma de convertir un caso en código está en [02-de-caso-a-codigo.md](02-de-caso-a-codigo.md).

## 1.1 Crear el proyecto

1. Entra a [wokwi.com](https://wokwi.com) e inicia sesión. Sin sesión no puedes guardar ni obtener la URL del proyecto, y la URL se pide en la entrega.
2. Crea un proyecto nuevo con la placa que pida el enunciado: **Arduino Uno** o **ESP32**. Usar otra placa te cuesta puntos en C02.
3. Guarda de inmediato (botón **Save**) y ponle un nombre claro, por ejemplo `smart-lifewatching-pc2`. La URL queda como `https://wokwi.com/projects/<id>`.

Un proyecto nuevo trae tres archivos:

| Archivo | Para qué sirve |
| --- | --- |
| `sketch.ino` | Punto de entrada: `setup()` y `loop()` |
| `diagram.json` | El circuito: componentes (`parts`) y cables (`connections`) |
| `libraries.txt` | Librerías externas; se crea al agregar la primera desde el Library Manager |

## 1.2 Armar el circuito primero

Arma y prueba el circuito antes de escribir las clases. Un cable mal puesto parece un error de código y te puede hacer perder 20 minutos.

1. En el panel de simulación, pulsa el botón **+** y agrega cada componente que pide el caso (sensor, display, LEDs, relé...).
2. Para cablear, haz clic en un pin y luego en el pin destino. Usa colores con significado: rojo para alimentación, negro para GND y otros colores para las señales.
3. Pon una **resistencia de 220 Ω** en serie con cada LED. Wokwi simula sin ella, pero el profesor evalúa la configuración del dispositivo.
4. Para los displays LCD por I2C, pon el atributo `"pins": "i2c"`. Así solo usan 4 cables: GND, VCC, SDA y SCL.
5. Si te es más cómodo, edita `diagram.json` directamente. Cada cable es una línea `["origen:pin", "destino:pin", "color", []]`.

Prueba rápida antes de seguir: un `digitalWrite(pin, HIGH)` por cada LED y un `Serial.println(analogRead(pin))` para los sensores analógicos.

## 1.3 Agregar librerías

1. En el desplegable de las pestañas de archivos, abre **Library Manager**.
2. Busca la librería por nombre y agrégala con **+**. Wokwi la escribe en `libraries.txt`.
3. Las librerías que usan los proyectos de este repositorio:

| Componente | Librería en Wokwi | Include |
| --- | --- | --- |
| HX711 (celda de carga) | `HX711 Arduino Library` (Bogdan Necula) | `#include "HX711.h"` |
| LCD 1602/2004 por I2C | `LiquidCrystal I2C` | `#include <LiquidCrystal_I2C.h>` |

El enunciado 2025-2 dice "hx711 Sensor Library". Busca "hx711" y elige la de Bogdan Necula, porque el código usa su API (`wait_ready_timeout`, `get_units`, `tare`).

## 1.4 Agregar archivos `.h` y `.cpp`

Wokwi admite varios archivos. Cada clase va en su propio `.h` (declaración) y `.cpp` (implementación).

- **Un archivo:** en el desplegable de las pestañas, **New file...**, y escribe el nombre con extensión (`Led.h`).
- **Varios a la vez:** en el mismo desplegable, **Upload file(s)...**, y selecciona todos los `.h` y `.cpp` de una carpeta de este repositorio.
- Todos los archivos van en la raíz del proyecto, sin subcarpetas, y se incluyen con comillas: `#include "Led.h"`.

## 1.5 Ejecutar y probar

1. Pulsa **Play** (▶). Si no compila, el error aparece abajo con el archivo y la línea.
2. La consola (Serial Monitor) se abre sola en el Uno. En el ESP32, `diagram.json` debe tener los cables `esp:TX0 → $serialMonitor:RX` y `esp:RX0 → $serialMonitor:TX` (ya están en el proyecto ESP32 de este repositorio).
3. Durante la simulación puedes interactuar con los componentes:
   - **HX711:** haz clic sobre él y mueve el valor de carga.
   - **Potenciómetro:** arrastra la perilla.
   - **DHT22, HC-SR04 y otros sensores:** haz clic y mueve los valores.
4. Prueba los valores límite del caso. Si la regla dice "mayor a 80", prueba 80 (no debe activar) y un poco más de 80 (sí debe activar).

## 1.6 Guardar y exportar para la entrega

1. **Save** guarda el proyecto. Copia su URL para el `.pptx`.
2. Exporta el ZIP: presiona **F1** en el editor de código y elige **Download project zip** (la opción también está en el desplegable del botón **Save**).
3. Renombra el ZIP con el formato exacto del enunciado, por ejemplo `upc-pre-202520-1asi0572-3355-pc2-u202012345.zip`.
4. Descomprímelo y revisa que tenga todos tus `.h`, `.cpp`, `sketch.ino`, `diagram.json` y `libraries.txt`.

## Errores típicos

| Síntoma | Causa probable |
| --- | --- |
| `HX711.h: No such file or directory` | Falta la librería en el Library Manager |
| El LCD prende pero no muestra texto | Dirección I2C distinta de 0x27, o SDA y SCL cruzados |
| El LCD no prende | VCC o GND sin conectar; en el ESP32, alimenta el LCD desde `VIN` |
| El ESP32 no imprime nada | Faltan los cables a `$serialMonitor`, o el baud rate no es 115200 |
| `multiple definition of ...` | Definiste una constante `static const Command` en el `.h` en vez del `.cpp` |
| El LED nunca prende | LED al revés: el ánodo (A) va a la resistencia y el pin; el cátodo (C) a GND |
