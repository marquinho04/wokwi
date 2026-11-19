# 4. Rúbrica y checklist de entrega

Rúbrica de la PC2 2025-2 (20 puntos). La PC2 2024-2 tuvo la misma estructura: 5 puntos para la Pregunta 1 y 15 para el prototipo.

| Criterio | S | E | M | I | Qué exige el nivel Sobresaliente |
| --- | --- | --- | --- | --- | --- |
| C01 Application service layer | 5,0 | 3,5 | 1,5 | 0 | Requisitos claros y completos, con la interfaz de usuario de **cada** servicio y **todos** alineados con la solución IoT |
| C02 IoT Device Design | 5,0 | 3,5 | 1,5 | 0 | El prototipo cumple **de forma completa** la configuración solicitada |
| C03 Software Features | 5,0 | 3,5 | 1,5 | 0 | Todas las operaciones funcionan, con los valores esperados **en cada caso** y respuesta adecuada **ante las excepciones** |
| C04 Quality Attributes | 3,0 | 2,0 | 1,0 | 0 | C++; propósito y autoría comentados en **todos** los archivos; indentación estándar; lógica consistente; condicionales sin escenarios no contemplados; sin redundancia; patrones de arquitectura y de diseño; **todos** los technical constraints |
| C05 Naming standards | 2,0 | 1,0 | 0,5 | 0 | Inglés y nomenclatura estándar en **todos** los identificadores |

Las palabras "todos", "cada" y "completa" son las que separan S de E. Un solo archivo sin title block o un solo identificador en español puede bajarte de nivel.

## Checklist antes de exportar

### C01 – Pregunta 1
- [ ] Hay un servicio por cada capacidad del caso (monitoreo, alertas, configuración o gestión, según aplique).
- [ ] Cada servicio indica usuario y tipo de interfaz (móvil, tablet, desktop o IoT device).
- [ ] Las descripciones usan los números del caso (umbrales, unidades, colores).

### C02 – Circuito
- [ ] La placa es la que pide el enunciado (Uno o ESP32).
- [ ] Están **todos** los componentes pedidos, incluido el reemplazo que pida el caso (por ejemplo, LCD 2004 en vez de 1602).
- [ ] Cada LED tiene su resistencia; GND y VCC conectados en todos los módulos.

### C03 – Funcionalidad
- [ ] Probaste cada regla en su límite exacto, justo debajo y justo encima.
- [ ] Hay manejo de excepciones: sensor sin respuesta o lectura inválida, con mensaje en el display y la consola.
- [ ] El display muestra todo lo que pide el caso, sin restos de textos anteriores.
- [ ] La consola muestra los datos de la empresa y tu nombre al arrancar, y cada cambio de estado.

### C04 – Calidad
- [ ] Title block en **cada** `.h`, `.cpp` y `sketch.ino`, con tu nombre (sin placeholders).
- [ ] `sketch.ino` solo crea el Device y llama `setup()` y `loop()`.
- [ ] Una clase por sensor o actuador, que hereda de `Sensor` o `Actuator`.
- [ ] Sin `delay()` en `loop()`; usas `millis()`.
- [ ] Sin números sueltos; sin código comentado ni variables sin uso.
- [ ] Todo `switch` e `if` cubre el caso por defecto o el valor inválido.

### C05 – Nombres
- [ ] Clases en `PascalCase`, métodos y variables en `camelCase`, constantes en `UPPER_SNAKE_CASE`.
- [ ] Todo en inglés, incluidos los nombres de archivo.

### Entrega
- [ ] El ZIP exportado de Wokwi tiene el nombre exacto del enunciado.
- [ ] El `.pptx` tiene la tabla de la Pregunta 1 y la URL del proyecto Wokwi.
- [ ] Subiste los dos archivos a la actividad correcta.
