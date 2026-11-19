# Caso aplicado: Smart LifeWatching (PC2 SI572, 2025-2)

Aplicación de la guía a la PC2 2025-2 (NRC 3355). La solución con el kit genérico está en [ejemplos/smart-lifewatching-uno](../ejemplos/smart-lifewatching-uno) (la placa que pide el enunciado) y [ejemplos/smart-lifewatching-esp32](../ejemplos/smart-lifewatching-esp32). La primera versión, más extensa, está en [referencia-extendida/](../referencia-extendida).

## 1. Resumen del caso

Smart LifeMonitor, inc. (fundada en 2015) ofrece monitoreo de signos vitales para pacientes con riesgo por enfermedades crónicas. Su línea LifeWatching empieza con el dispositivo **Smart LifeWatching**, que debe:

| Entrada | Regla | Salida |
| --- | --- | --- |
| Peso (celda de carga + HX711) | Mayor a 80 kg | LED azul de peso encendido |
| Ritmo cardiaco (PPM) | Menor a 60 / 60 a 100 / mayor a 100 | Estado bajo (LED azul) / normal (verde) / alto (rojo) |
| Peso y ritmo | Siempre | LCD 2004 con peso, PPM y estado (reemplaza al LCD 1602 del prototipo) |
| Arranque | Una vez | Consola: datos de la empresa y nombre del developer |

Technical constraints: Wokwi + Arduino sketch (C++), OOP basado en ModestIoT, `sketch.ino` que solo instancia el Device, nombres y comentarios en inglés, title blocks, y consola que muestre los modos de operación y los cambios de estado.

## 2. Pregunta 1 resuelta

| Servicio | Usuario final | Interfaz (tipo) | Especificación de la interfaz de usuario |
| --- | --- | --- | --- |
| S1. Medición y visualización local de signos vitales | Paciente | IoT Device: LCD 2004 + 4 LEDs | Línea 1: nombre del dispositivo. Línea 2: peso en kg, con marca ">80" si supera 80 kg. Línea 3: ritmo cardiaco en PPM. Línea 4: estado (LOW/NORMAL/HIGH). LED azul si el peso supera 80 kg. LED azul (<60), verde (60–100) o rojo (>100) para el ritmo. Mensajes de error si falla un sensor ("SCALE ERROR", "NO SIGNAL"). |
| S2. Monitoreo de peso | Paciente; personal de monitoreo de Smart Lifemonitor | Móvil (paciente); desktop web (personal) | Móvil: último peso, fecha y hora, indicador visual si supera 80 kg y gráfico de evolución. Desktop: tabla de pacientes con su último peso, filtro "peso > 80 kg" e historial por paciente. |
| S3. Monitoreo de ritmo cardiaco | Paciente; personal de monitoreo | Móvil (paciente); desktop web y tablet (personal) | PPM actual con el mismo código de color de los LEDs (azul bajo, verde normal, rojo alto). Gráfico de PPM en el tiempo con bandas <60, 60–100 y >100. Desktop y tablet: panel con todos los pacientes y su estado actual. |
| S4. Alertas de riesgo | Paciente; personal de monitoreo | Móvil (notificación push); desktop web y tablet (panel de alertas) | Alerta cuando el ritmo pasa a bajo (<60) o alto (>100), cuando el peso supera 80 kg y cuando el dispositivo reporta una falla de sensor. Panel con las alertas ordenadas por hora: paciente, valor medido, estado y botón "atendida". |
| S5. Gestión de pacientes y dispositivos Smart LifeWatching | Personal de Smart Lifemonitor | Desktop web | Registro de pacientes, asociación paciente–dispositivo, estado del dispositivo (activo, en falla, sin datos) y consulta de sus umbrales (80 kg; 60 y 100 PPM). |

## 3. Pregunta 2: decisiones de diseño

| Clase (versión con el kit) | Rol | Patrón |
| --- | --- | --- |
| `SmartLifeWatchingDevice` | Recibe eventos, aplica las reglas y envía comandos | Mediator |
| `WeightSensor`, `AnalogSensor` (pulso) | Leen el hardware y emiten eventos de medición o de falla | Herencia de `Sensor` con `read()` sobrescrito |
| `Led`, `LcdDisplay` | Ejecutan comandos | Herencia de `Actuator`, Command |
| `LevelClassifier(60, 100)` | Reglas < 60 / 60–100 / > 100 | Responsabilidad única, reutilizable |
| `Event`, `Command` | Objetos de valor que viajan entre componentes | Separación evento/comando (CQRS) |

Dos supuestos que hay que poder explicar:

- **Sensor de ritmo cardiaco.** Wokwi no tiene sensor de pulso, así que se simula con un potenciómetro: todo el recorrido equivale a 0–200 PPM. Una lectura menor a 30 PPM se trata como "sin señal" (sensor mal puesto o desconectado), igual que el mínimo fisiológico del framework ModestIoT.
- **Calibración del peso: 105 cuentas/kg.** El HX711 "50kg" de Wokwi solo llega a 50 kg (0–21000 cuentas), así que sin escalar nunca se podría probar el caso > 80 kg. Se asume una plataforma de 4 celdas de 50 kg (capacidad de 200 kg), como el ejemplo `SmartScaleDevice` del framework: 21000 / 200 = 105. Con el control del HX711 en 20 kg, el LCD muestra 80,0 kg.

## 4. Pruebas

Probado compilando con stubs de Arduino y simulando cada caso, en las dos versiones. **No se ejecutó dentro de Wokwi:** hazlo antes de entregar.

| Acción en el simulador | Resultado esperado |
| --- | --- |
| Iniciar | Consola: datos de Smart LifeMonitor, inc. y tu nombre; modo INITIALIZING → MONITORING |
| HX711 en 20 kg | `Weight:  80.0 kg`, LED de peso apagado (80 no es mayor a 80) |
| HX711 en 25 kg | `Weight: 100.0 kg >80`, LED azul de peso encendido |
| Potenciómetro al mínimo | `Heart rate: --- BPM`, `Status: NO SIGNAL`, los 3 LEDs de ritmo apagados y aviso en consola |
| Potenciómetro a ~30 % (≈ 58 PPM) | `Status: LOW`, LED azul de ritmo |
| Potenciómetro al centro (≈ 100 PPM) | `Status: NORMAL`, LED verde |
| Potenciómetro al máximo (200 PPM) | `Status: HIGH`, LED rojo |

La consola imprime cada cambio con las etiquetas `[MODE]`, `[STATE]` y `[ACTUATOR]`, y un resumen `[TELEMETRY]` cada 2 s.

## 5. Conexiones

| Componente | Arduino Uno | ESP32 DevKit V1 |
| --- | --- | --- |
| HX711 DT / SCK | D3 / D2 | GPIO 19 / GPIO 18 |
| Sensor de ritmo (potenciómetro) SIG | A0 | GPIO 34 |
| LED azul de peso > 80 kg | D8 | GPIO 4 |
| LED azul bajo / verde normal / rojo alto | D9 / D10 / D11 | GPIO 25 / 26 / 27 |
| LCD 2004 I2C SDA / SCL (0x27) | A4 / A5 | GPIO 21 / GPIO 22 |
| Alimentación | 5V | HX711 y potenciómetro a 3V3; LCD a VIN |

Todos los LEDs llevan una resistencia de 220 Ω en serie.
