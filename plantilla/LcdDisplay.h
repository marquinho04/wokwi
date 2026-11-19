/*
  LcdDisplay.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  LCD 2004 (20 columns x 4 rows, I2C) actuator. Text is staged per row
  with setLine() and written to the screen with REFRESH_COMMAND; only the
  rows that changed are rewritten, to avoid flickering.

  The circuit:
  * LCD SDA -> board SDA pin (Arduino Uno A4 / ESP32 GPIO 21)
  * LCD SCL -> board SCL pin (Arduino Uno A5 / ESP32 GPIO 22)

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include "Actuator.h"

/**
 * @brief Character LCD actuator with buffered, change-only row updates.
 */
class LcdDisplay : public Actuator {
public:
    static const unsigned char COLUMN_COUNT = 20; ///< LCD 2004 columns.
    static const unsigned char ROW_COUNT = 4;     ///< LCD 2004 rows.

    static const int REFRESH_COMMAND_IDENTIFIER = 200; ///< Write staged rows to the screen.
    static const int CLEAR_COMMAND_IDENTIFIER = 201;   ///< Clear screen and buffers.
    static const Command REFRESH_COMMAND;              ///< Prebuilt refresh command.
    static const Command CLEAR_COMMAND;                ///< Prebuilt clear command.

    /**
     * @brief Constructs the display.
     * @param i2cAddress I2C address of the LCD backpack (Wokwi default 0x27).
     * @param parentHandler Optional parent handler for propagated commands.
     */
    explicit LcdDisplay(unsigned char i2cAddress, CommandHandler* parentHandler = nullptr);

    /**
     * @brief Initializes the LCD and turns the backlight on.
     */
    void begin();

    /**
     * @brief Stages the text of one row (truncated / padded to 20 characters).
     * @param rowIndex Row 0..3.
     * @param text Null-terminated text.
     */
    void setLine(unsigned char rowIndex, const char* text);

protected:
    void executeCommand(Command command) override;

private:
    static const int NO_GPIO_PIN = -1; ///< The LCD uses the I2C bus, not a dedicated GPIO.

    LiquidCrystal_I2C lcd;                          ///< LCD driver (LiquidCrystal I2C library).
    char lineBuffer[ROW_COUNT][COLUMN_COUNT + 1];   ///< Staged text per row.
    bool lineChanged[ROW_COUNT];                    ///< Rows pending to be written.

    void clearBuffers();
};

#endif // LCD_DISPLAY_H
