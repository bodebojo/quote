#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// Globals from main.ino:
extern double avg_dis;
extern double light_left;
extern double light_right;
extern LiquidCrystal_I2C lcd;

// Perform exactly one ultrasonic ping, filter it, update avg_dis, LCD, and Serial
void get_distance_one_shot() {
  static int prev_distance = 0;
  static unsigned long lastPrint = 0;
  const unsigned long PRINT_INTERVAL = 1000; // ms

  int raw = getDistance(); // from Leaphyoriginal1
  if (prev_distance == 0) prev_distance = raw;
  raw = constrain(raw, prev_distance - 50, prev_distance + 50);
  prev_distance = raw;
  avg_dis = raw;

  // Update LCD (no delay):
  // print_to_lcd("AVG Distance:", String(avg_dis, 1), 0.0, false);

  // Print to Serial once per second
  if (millis() - lastPrint >= PRINT_INTERVAL) {
    lastPrint = millis();
    Serial.println("Distance: " + String(avg_dis, 1));
  }
}

// Read A0/A4 once, update light_left/right, and Serial every second
void get_light_one_shot() {
  static unsigned long lastPrint = 0;
  const unsigned long PRINT_INTERVAL = 1000;

  light_left  = analogRead(A0);
  light_right = analogRead(A4);

  if (millis() - lastPrint >= PRINT_INTERVAL) {
    lastPrint = millis();
    Serial.println("Light Left: " + String(light_left, 1));
    Serial.println("Light Right: " + String(light_right, 1));
  }
}

#endif
