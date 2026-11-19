/*
  LcdDisplay.cpp
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Implements the LcdDisplay actuator (LCD 2004 I2C).

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#include <string.h>
#include "LcdDisplay.h"

const Command LcdDisplay::REFRESH_COMMAND(LcdDisplay::REFRESH_COMMAND_IDENTIFIER);
const Command LcdDisplay::CLEAR_COMMAND(LcdDisplay::CLEAR_COMMAND_IDENTIFIER);

LcdDisplay::LcdDisplay(unsigned char i2cAddress, CommandHandler* parentHandler)
    : Actuator(NO_GPIO_PIN, parentHandler), lcd(i2cAddress, COLUMN_COUNT, ROW_COUNT) {
    clearBuffers();
}

void LcdDisplay::begin() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    clearBuffers();
}

void LcdDisplay::setLine(unsigned char rowIndex, const char* text) {
    if (rowIndex >= ROW_COUNT || text == nullptr) {
        return; // Invalid row or text: ignored
    }

    // Build the padded row, so leftovers of longer texts are erased
    char paddedText[COLUMN_COUNT + 1];
    unsigned char column = 0;
    for (; column < COLUMN_COUNT && text[column] != '\0'; column++) {
        paddedText[column] = text[column];
    }
    for (; column < COLUMN_COUNT; column++) {
        paddedText[column] = ' ';
    }
    paddedText[COLUMN_COUNT] = '\0';

    if (strcmp(paddedText, lineBuffer[rowIndex]) != 0) {
        strcpy(lineBuffer[rowIndex], paddedText);
        lineChanged[rowIndex] = true;
    }
}

void LcdDisplay::executeCommand(Command command) {
    if (command == REFRESH_COMMAND) {
        for (unsigned char row = 0; row < ROW_COUNT; row++) {
            if (lineChanged[row]) {
                lcd.setCursor(0, row);
                lcd.print(lineBuffer[row]);
                lineChanged[row] = false;
            }
        }
    } else if (command == CLEAR_COMMAND) {
        lcd.clear();
        clearBuffers();
    }
}

void LcdDisplay::clearBuffers() {
    for (unsigned char row = 0; row < ROW_COUNT; row++) {
        memset(lineBuffer[row], ' ', COLUMN_COUNT);
        lineBuffer[row][COLUMN_COUNT] = '\0';
        lineChanged[row] = false;
    }
}
