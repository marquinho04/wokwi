/*
  LcdDisplay.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  LCD 2004 (20x4, I2C) actuator. setLine() stages the text of a row and
  the REFRESH command writes only the rows that changed (no flickering).

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include <string.h>
#include "ModestIoT.h"

class LcdDisplay : public Actuator {
public:
    static const int COLUMN_COUNT = 20;
    static const int ROW_COUNT = 4;
    static const int REFRESH_COMMAND = 200;
    static const int CLEAR_COMMAND = 201;

private:
    static const int NO_GPIO_PIN = -1; // Uses the I2C bus
    LiquidCrystal_I2C lcd;
    char lines[ROW_COUNT][COLUMN_COUNT + 1];
    bool changed[ROW_COUNT];

    void clearBuffers() {
        for (int row = 0; row < ROW_COUNT; row++) {
            memset(lines[row], ' ', COLUMN_COUNT);
            lines[row][COLUMN_COUNT] = '\0';
            changed[row] = false;
        }
    }

public:
    explicit LcdDisplay(unsigned char i2cAddress)
        : Actuator(NO_GPIO_PIN), lcd(i2cAddress, COLUMN_COUNT, ROW_COUNT) {
        clearBuffers();
    }

    void begin() {
        lcd.init();
        lcd.backlight();
        lcd.clear();
    }

    /** Stages a row, padded with spaces to erase old text. */
    void setLine(int row, const char* text) {
        if (row < 0 || row >= ROW_COUNT || text == nullptr) {
            return;
        }
        char padded[COLUMN_COUNT + 1];
        snprintf(padded, sizeof(padded), "%-*s", COLUMN_COUNT, text);
        if (strcmp(padded, lines[row]) != 0) {
            strcpy(lines[row], padded);
            changed[row] = true;
        }
    }

    void handle(Command command) override {
        if (command.identifier == REFRESH_COMMAND) {
            for (int row = 0; row < ROW_COUNT; row++) {
                if (changed[row]) {
                    lcd.setCursor(0, row);
                    lcd.print(lines[row]);
                    changed[row] = false;
                }
            }
        } else if (command.identifier == CLEAR_COMMAND) {
            lcd.clear();
            clearBuffers();
        }
    }
};

#endif // LCD_DISPLAY_H
