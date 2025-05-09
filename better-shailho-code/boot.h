#ifndef BOOT_H
#define BOOT_H

extern LiquidCrystal_I2C lcd;
extern bool do_boot;

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

void bootsetup(float boot_time_s) {
  int boot_time_half_s = boot_time_s /2;
  int boot_time_half_ms = boot_time_half_s * 1000;
  int boot_delay = boot_time_half_ms/ 17;
  
  lcd.clear();
  lcd.setCursor(0,0);

  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 16; col++) {
      lcd.setCursor(col, row);
      lcd.write(byte(0));
      delay(boot_delay);
    }
  }
  delay(boot_delay);
  lcd.clear();

  lcd.setCursor(7,0);
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.setCursor(7,1);
  lcd.write(byte(3));
  lcd.write(byte(4));

  delay(boot_time_half_ms);
  lcd.clear();
}

void boot(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, block);
  lcd.createChar(1, heart0);
  lcd.createChar(2, heart1);
  lcd.createChar(3, heart2);
  lcd.createChar(4, heart3);
  lcd.backlight();
  if (do_boot == true) {
    bootsetup(2.5);
  }
 }

#endif