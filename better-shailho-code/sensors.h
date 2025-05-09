#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

extern double avg_dis;
extern double light_left;
extern double light_right;

void print_to_lcd(String text, String value, float delay_time, bool clear_screen);

unsigned long previousMillis_distance = 0;
unsigned long previousMillis_light = 0;
const long interval = 1000;  // Interval in ms for printing

static int loop_count = 0;

void get_distance() {
  unsigned long currentMillis = millis();
  int loop_amount = 10;
  int i, n, prev_n = 0, sum = 0;

  for (i = 1; i <= loop_amount; i++) {
    n = getDistance();
    if (prev_n == 0) prev_n = n;
    n = constrain(n, prev_n - 50, prev_n + 50);
    sum += n;
    prev_n = n;
    delay(100 / loop_amount);
  }

  avg_dis = sum / loop_amount;
  print_to_lcd("AVG Distance:", String(avg_dis, 3), 0.1, false);

  if (currentMillis - previousMillis_distance >= interval) {
    previousMillis_distance = currentMillis;
    Serial.println("Distance: " + String(avg_dis, 3));
  }
}

void get_light_info() {
  unsigned long currentMillis = millis();
  light_left = analogRead(A0);
  light_right = analogRead(A1);

  if (currentMillis - previousMillis_light >= interval) {
    previousMillis_light = currentMillis;
    Serial.println("Light Left: " + String(light_left, 3));
    Serial.println("Light Right: " + String(light_right, 3));
  }
}

#endif