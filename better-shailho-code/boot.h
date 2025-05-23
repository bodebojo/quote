#ifndef BOOT_H
#define BOOT_H

#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

extern LiquidCrystal_I2C lcd;
extern bool do_boot;

// Draw a simple “boot” animation using custom characters
void bootsetup(float boot_time_s) {
  int boot_time_half_s  = boot_time_s / 2.0;
  int boot_time_half_ms = boot_time_half_s * 1000;
  int boot_delay        = boot_time_half_ms / 17; // spread across 16×2 chars

  lcd.clear();
  lcd.setCursor(0, 0);
  // Fill two rows of 16×2 in “block” character
  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 16; col++) {
      lcd.setCursor(col, row);
      lcd.write(byte(0));  // custom char index 0 = full block
      delay(boot_delay);
    }
  }
  delay(boot_delay);
  lcd.clear();

  // Draw a simple “heart” shape in the center
  lcd.setCursor(7, 0);
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.setCursor(7, 1);
  lcd.write(byte(3));
  lcd.write(byte(4));

  delay(boot_time_half_ms);
  lcd.clear();
}

void boot() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Define custom characters:
  byte block[8] = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  byte heart0[8] = {
    0b00000,
    0b00000,
    0b00110,
    0b01111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  byte heart1[8] = {
    0b00000,
    0b00000,
    0b01100,
    0b11110,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  byte heart2[8] = {
    0b11111,
    0b11111,
    0b01111,
    0b00111,
    0b00011,
    0b00001,
    0b00000,
    0b00000
  };
  byte heart3[8] = {
    0b11111,
    0b11111,
    0b11110,
    0b11100,
    0b11000,
    0b10000,
    0b00000,
    0b00000
  };

  lcd.createChar(0, block);
  lcd.createChar(1, heart0);
  lcd.createChar(2, heart1);
  lcd.createChar(3, heart2);
  lcd.createChar(4, heart3);

  if (do_boot) {
    bootsetup(2.5);
  }
}

// Print a two‐line message on the LCD, then optionally delay & clear
void print_to_lcd(String text, String value, float delay_time, bool clear_screen) {
  String message = text + " " + value;
  message.trim(); // Remove extra spaces
  float pause_ms = delay_time * 1000.0;

  if (clear_screen) {
    lcd.clear();
  }

  int msgLen = message.length();

  if (msgLen <= 16) {
    // Fits in one line
    lcd.setCursor(0, 0);
    lcd.print(message);
    lcd.setCursor(0, 1);
    lcd.print("                ");
  } else if (msgLen <= 32) {
    // Word-wrap for two lines
    int splitPos = message.lastIndexOf(' ', 16); // Try to split at a space
    if (splitPos == -1) splitPos = 16; // No space found, force split

    String top = message.substring(0, splitPos);
    String bottom = message.substring(splitPos);
    bottom.trim(); // Trim leading space

    lcd.setCursor(0, 0);
    lcd.print(top);
    lcd.setCursor(0, 1);
    lcd.print(bottom);
  } else {
    // Scroll top line only
    String scrollMessage = message + "                "; // Pad with spaces to scroll out
    int scrollLen = scrollMessage.length();

    for (int i = 0; i <= scrollLen - 16; i++) {
      lcd.setCursor(0, 0);
      lcd.print(scrollMessage.substring(i, i + 16));
      lcd.setCursor(0, 1);
      lcd.print("                "); // Keep bottom line empty
      delay(600); // Scroll speed
    }
  }

  if (pause_ms > 0) {
    delay((int)pause_ms);
  }
}





#endif
