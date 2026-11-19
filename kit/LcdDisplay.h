/*
  LcdDisplay.h
  Generic IoT kit - Embedded Application (Modest IoT based)

  I2C character LCD: 16x2 (LCD 1602) or 20x4 (LCD 2004).
  Requires the "LiquidCrystal I2C" library. setLine() stages a row and
  Command::REFRESH writes only the rows that changed (no flickering).

  Created 19 November 2025
  By <Developer Full Name>
*/

#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include <string.h>
#include "ModestIoT.h"

class LcdDisplay : public Actuator {
public:
    static const int MAXIMUM_COLUMNS = 20;
    static const int MAXIMUM_ROWS = 4;

private:
    static const int NO_GPIO_PIN = -1; // Uses the I2C bus
    LiquidCrystal_I2C lcd;
    int columnCount;
    int rowCount;
    char lines[MAXIMUM_ROWS][MAXIMUM_COLUMNS + 1];
    bool changed[MAXIMUM_ROWS];

    void clearBuffers() {
        for (int row = 0; row < MAXIMUM_ROWS; row++) {
            memset(lines[row], ' ', MAXIMUM_COLUMNS);
            lines[row][MAXIMUM_COLUMNS] = '\0';
            changed[row] = false;
        }
    }

public:
    /** i2cAddress: 0x27 in Wokwi. columns x rows: 16x2 or 20x4. */
    LcdDisplay(unsigned char i2cAddress, int columns, int rows)
        : Actuator(NO_GPIO_PIN), lcd(i2cAddress, columns, rows),
          columnCount(columns > MAXIMUM_COLUMNS ? MAXIMUM_COLUMNS : columns),
          rowCount(rows > MAXIMUM_ROWS ? MAXIMUM_ROWS : rows) {
        clearBuffers();
    }

    void begin() override {
        lcd.init();
        lcd.backlight();
        lcd.clear();
    }

    /** Stages the text of a row, padded with spaces to erase old text. */
    void setLine(int row, const char* text) {
        if (row < 0 || row >= rowCount || text == nullptr) {
            return;
        }
        // Copy the text and pad with spaces (no printf "%-*s": not supported on Arduino Uno)
        char padded[MAXIMUM_COLUMNS + 1];
        int column = 0;
        for (; column < columnCount && text[column] != '\0'; column++) {
            padded[column] = text[column];
        }
        for (; column < columnCount; column++) {
            padded[column] = ' ';
        }
        padded[columnCount] = '\0';
        if (strcmp(padded, lines[row]) != 0) {
            strcpy(lines[row], padded);
            changed[row] = true;
        }
    }

    void handle(Command command) override {
        if (command.type == Command::REFRESH) {
            for (int row = 0; row < rowCount; row++) {
                if (changed[row]) {
                    lcd.setCursor(0, row);
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
