#include <Leaphyoriginal1.h>
#include <SoftwareSerial.h> // For software serial communication if needed
#include <LiquidCrystal_I2C.h>

SoftwareSerial espSerial(13, 15); // Adjust pins if necessary
LiquidCrystal_I2C lcd(0x3F, 16, 2); // Set the I2C address, number of columns, and number of rows
int speed = 500;


void setup() {
  Serial.begin(9600); // Initialize serial communication with the computer
  espSerial.begin(9600); // Initialize serial communication with the ESP`

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.clear(); // Clear the LCD
  lcd.setCursor(0, 0); // Set the cursor to the first row, first column
  lcd.print("Hello, world!"); // Print text to the LCD
  
}

void loop() {
  // You can add more code here to update the display or perform other actions
}