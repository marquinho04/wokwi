/*
  LcdDisplay.h  —  COPIA DE ESTUDIO (comentarios en español, NO entregar)
  ----------------------------------------------------------------------
  Pantalla LCD de caracteres por I2C (4 cables: GND, VCC, SDA, SCL).
  Sirve para la LCD 1602 (16 columnas x 2 filas) y la 2004 (20 x 4).
  Usa la librería "LiquidCrystal I2C".

  Cómo se usa (dos pasos):
    display.setLine(0, "Hola");                 // 1. Prepara el texto de una fila
    display.handle(Command(Command::REFRESH));  // 2. Lo escribe en la pantalla

  ¿Por qué dos pasos? Guarda el texto en memoria ("buffer") y en REFRESH
  solo reescribe las filas que cambiaron. Si reescribieras todo en cada
  ciclo, la pantalla parpadearía.
*/

#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal_I2C.h> // Librería externa.
#include <string.h>            // memset, strcmp, strcpy (manejo de textos en C).
#include "ModestIoT.h"

class LcdDisplay : public Actuator {
public:
    static const int MAXIMUM_COLUMNS = 20; // Tamaño máximo soportado (LCD 2004).
    static const int MAXIMUM_ROWS = 4;

private:
    static const int NO_GPIO_PIN = -1; // La LCD va por el bus I2C, no usa un pin propio.
    LiquidCrystal_I2C lcd;             // Objeto de la librería.
    int columnCount;                   // Columnas reales de esta pantalla (16 o 20).
    int rowCount;                      // Filas reales (2 o 4).
    // Buffer: una fila de texto por línea. +1 por el '\0' que termina cada texto en C.
    char lines[MAXIMUM_ROWS][MAXIMUM_COLUMNS + 1];
    bool changed[MAXIMUM_ROWS];        // ¿Esta fila tiene texto nuevo por escribir?

    // Llena el buffer con espacios y marca todo como "sin cambios".
    void clearBuffers() {
        for (int row = 0; row < MAXIMUM_ROWS; row++) {
            memset(lines[row], ' ', MAXIMUM_COLUMNS); // Rellena con espacios.
            lines[row][MAXIMUM_COLUMNS] = '\0';       // Marca el fin del texto.
            changed[row] = false;
        }
    }

public:
    // i2cAddress: 0x27 en Wokwi. columns x rows: 16x2 o 20x4.
    // "(condición ? a : b)" limita los valores al máximo del buffer.
    LcdDisplay(unsigned char i2cAddress, int columns, int rows)
        : Actuator(NO_GPIO_PIN), lcd(i2cAddress, columns, rows),
          columnCount(columns > MAXIMUM_COLUMNS ? MAXIMUM_COLUMNS : columns),
          rowCount(rows > MAXIMUM_ROWS ? MAXIMUM_ROWS : rows) {
        clearBuffers();
    }

    // Enciende la pantalla y la luz de fondo. Se llama desde Device::setup().
    void begin() override {
        lcd.init();
        lcd.backlight();
        lcd.clear();
    }

    // Prepara el texto de una fila. Lo rellena con espacios hasta el ancho de
    // la pantalla, para borrar restos de un texto anterior más largo.
    void setLine(int row, const char* text) {
        if (row < 0 || row >= rowCount || text == nullptr) {
            return; // Fila inválida o texto vacío: se ignora (excepción controlada).
        }
        char padded[MAXIMUM_COLUMNS + 1];
        // "%-*s" = texto alineado a la izquierda y rellenado con espacios
        // hasta columnCount caracteres (si es más largo, se recorta).
        snprintf(padded, columnCount + 1, "%-*s", columnCount, text);
        if (strcmp(padded, lines[row]) != 0) { // strcmp != 0: los textos son distintos.
            strcpy(lines[row], padded);         // Copia al buffer.
            changed[row] = true;                // Pendiente de escribir.
        }
    }

    // Ejecuta órdenes: REFRESH escribe las filas cambiadas; CLEAR borra todo.
    void handle(Command command) override {
        if (command.type == Command::REFRESH) {
            for (int row = 0; row < rowCount; row++) {
                if (changed[row]) {
                    lcd.setCursor(0, row);  // Columna 0 de esa fila.
                    lcd.print(lines[row]);
                    changed[row] = false;
                }
            }
        } else if (command.type == Command::CLEAR) {
            lcd.clear();
            clearBuffers();
        }
    }

    int getColumnCount() const { return columnCount; }
};

#endif // LCD_DISPLAY_H
