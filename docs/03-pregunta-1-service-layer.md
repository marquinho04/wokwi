# 3. Cómo responder la Pregunta 1 (Application Service Layer Requirements)

En las PC2 del recopilatorio (2024-2 Cia Steel Faucet y 2025-2 Smart Lifemonitor), la Pregunta 1 vale 5 puntos y pide lo mismo: el **paso 6 de los 12 IoT System Design Steps**, *Definition of application service layer requirements*, con la interfaz de usuario de cada servicio (móvil, tablet, desktop o más de una).

## 3.1 Dónde está en los 12 pasos (Anexo A)

| # | Paso |
| --- | --- |
| 1 | Definition of the system requirements |
| 2 | Selection of the IoT system typology |
| 3 | Definition of physical layer requirements |
| 4 | Definition of exchange layer requirements |
| 5 | Definition of information layer requirements |
| **6** | **Definition of application service layer requirements** |
| 7 | Selection of the architectures of data exchange and information integration layers |
| 8 | Selection of the sensors and the actuators |
| 9 | Selection of the microcontroller and radio transceivers |
| 10 | Definition of the data processing for each node and in Cloud |
| 11 | Analysis of the processing time |
| 12 | Definition of the graphical user interface |

El paso 6 responde a una pregunta: qué servicios ofrece la solución a cada usuario y con qué interfaz los usa.

## 3.2 Método en 5 pasos (15 min en el examen)

1. **Identifica a los usuarios** que el caso menciona o implica: el usuario del dispositivo (paciente, cliente) y quien opera el servicio (personal de la empresa, soporte). No inventes usuarios que el caso no sostiene.
2. **Saca un servicio por cada capacidad del caso.** Cada sensor y cada regla del enunciado suele ser un servicio de monitoreo o de alerta. Agrega los transversales que el caso justifica: configuración (si hay umbrales), gestión de dispositivos y soporte.
3. **Asigna la interfaz por tipo de dispositivo** en cada servicio, con un motivo: móvil para el usuario final, desktop o tablet para quien supervisa a muchos usuarios, y el propio IoT device (display y LEDs) cuando el servicio es local.
4. **Describe qué muestra y qué permite hacer cada interfaz**, con los datos reales del caso: unidades, umbrales, colores, estados. "Muestra el peso en kg y marca si supera 80 kg" vale más que "muestra información".
5. **Revisa que todo cuadre con la Pregunta 2.** Lo que muestra el LCD en tu prototipo debe coincidir con lo que dices del servicio local.

## 3.3 Formato recomendado

La solución modelo de 2024-2 (nota 17,5) usó dos columnas: *Application Service Layer Requirement* y *Especificación de requisitos*. Esta versión agrega las columnas de usuario e interfaz, que son justo lo que mide C01:

| Servicio | Usuario final | Interfaz (tipo) | Especificación de la interfaz de usuario |
| --- | --- | --- | --- |
| S1. ... | ... | Móvil / Tablet / Desktop / IoT Device | Qué datos muestra, en qué unidades, qué acciones permite, qué alertas |

Ejemplo completo para Smart Lifemonitor: [caso/README.md](../caso/README.md#2-pregunta-1-resuelta).

## 3.4 Qué separa un 5 de un 3 (C01)

| Sobresaliente | Esperado o menos |
| --- | --- |
| Cubre **todos** los servicios que el caso implica | Faltan servicios (por ejemplo, alertas o configuración) |
| Cada servicio dice **qué tipo de interfaz** y **por qué** | Dice "aplicación" sin indicar móvil, tablet o desktop |
| Usa los datos del caso (80 kg, 60–100 PPM, colores de LED) | Descripciones genéricas que sirven para cualquier caso |
| Coherente con el prototipo de la Pregunta 2 | Contradice lo que muestra el device |

## 3.5 Si la Pregunta 1 pide otro paso

En otras evaluaciones del recopilatorio (Smart Agro, Eventify) se pidieron otros pasos. Qué incluir según lo que pidieron esos enunciados:

| Paso pedido | Qué pidió el enunciado |
| --- | --- |
| 1. System requirements | Power supply (capacidades de suministro de energía) y restricciones de time-delay |
| 3. Physical layer | Número y tipos de nodos sensores y actuadores; target uncertainty de cada sensor; target accuracy y precision de los actuadores; processing power del Edge node |
| 5. Information layer | Usuarios finales; número y tipos de servicios por usuario; necesidades de información integrada de cada servicio |

Aplica el mismo método: una tabla, datos concretos del caso y nada genérico.
