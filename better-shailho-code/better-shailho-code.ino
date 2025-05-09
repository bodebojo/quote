#include <Leaphyoriginal1.h>
#include <SoftwareSerial.h> // For software serial communication if needed
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "boot.h"
#include "mode1.h"
#include "mode2.h"
#include "mode3.h"
#include "mode4.h"
#include "sensors.h"
SoftwareSerial espSerial(13, 15); // Adjust pins if necessary
LiquidCrystal_I2C lcd(0x3F,20,4);

//config
bool do_boot = false;
//varibalesa
String mode;
int right_speed = 0;
int left_speed = 0;
int default_speed = 100;
int speed = 500;
double avg_dis;  // Declaration of variable 'avg_dis' to store the average
double light_left;
double light_right;
String incomingData;
int modeInt;

void setup(){
  Serial.begin(115200); // Initialize serial communication with the computer
  espSerial.begin(115200); // Initialize serial communication with the ESP
  randomSeed(time(0));
  boot();
}

void loop(){
readSerial();
get_distance();
get_light_info();
handlemode();

avg_dis = 0;
}

void readSerial() {
  if (Serial.available()) {
    incomingData = Serial.readStringUntil('\n');
    incomingData.trim();
    Serial.println("IncomingData: " + String(incomingData));
  }
}

void handlemode() {
  if (incomingData.startsWith("Mode:")) {
    mode = incomingData.substring(String("Mode:").length());
    mode.trim();  // Removes any extra whitespace

    modeInt = mode.toInt();  // Convert string to integer
   } if (modeInt == 1) { 
      mode1();
      // Serial.println("mode1 selected");
    } else if (modeInt == 2) {
      mode2();
    } else if (modeInt == 3) {
      mode3();
    } else if (modeInt == 4) {
      mode4();
    } else {
      Serial.println("invalid driving mode detected");
  }
}


void print_to_lcd(String text, String value, float delay_time, bool clear_screen) {
  float pause_ms = delay_time * 1000;

  lcd.setCursor(0, 0);
  lcd.print(text);

  lcd.setCursor(0, 1);
  lcd.print(value);

  delay(pause_ms);

  if (clear_screen) {
    lcd.clear();
  }
}
