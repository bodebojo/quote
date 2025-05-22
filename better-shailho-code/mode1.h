#ifndef MODE1_H
#define MODE1_H

#include <Arduino.h>

// These globals are defined in main.ino:
extern String incomingData;
extern int speed;
extern int left_speed;
extern int right_speed;
extern void setMotor(int motor, int speed);

// “Manual” drive via W/A/S/D keys
void mode1() {
  // Key states persist between calls
  static bool wPressed = false;
  static bool aPressed = false;
  static bool sPressed = false;
  static bool dPressed = false;
  static bool shiftPressed = false;

  // Parse “Key Down:” / “Key Up:” from incomingData
  if (incomingData.startsWith("Key Down:")) {
    String key = incomingData.substring(9);
    key.trim();
    key.toLowerCase();
    if (key == "w")        wPressed = true;
    else if (key == "s")   sPressed = true;
    else if (key == "a")   aPressed = true;
    else if (key == "d")   dPressed = true;
    else if (key == "shift") shiftPressed = true;
  }
  else if (incomingData.startsWith("Key Up:")) {
    String key = incomingData.substring(7);
    key.trim();
    key.toLowerCase();
    if (key == "w")        wPressed = false;
    else if (key == "s")   sPressed = false;
    else if (key == "a")   aPressed = false;
    else if (key == "d")   dPressed = false;
    else if (key == "shift") shiftPressed = false;
  }

  // Clear incomingData to avoid re-processing
  incomingData = "";

  // Compute base speed (Shift = half speed)
  speed = shiftPressed ? 400 : 500;
  int turnOffset = speed / 3;
  int turnOffsettank = speed / 3.3;
  // Start with motors stopped
  left_speed  = 0;
  right_speed = 0;

  // Forward/back commands
  if (wPressed) {
    left_speed  = speed;
    right_speed = speed;
    if (aPressed) {
      left_speed  -= turnOffset;
      right_speed += turnOffset;
    } else if (dPressed) {
      left_speed  += turnOffset;
      right_speed -= turnOffset;
    }
  }
  else if (sPressed) {
    left_speed  = -speed;
    right_speed = -speed;
    if (aPressed) {
      left_speed  -= turnOffset;
      right_speed += turnOffset;
    } else if (dPressed) {
      left_speed  += turnOffset;
      right_speed -= turnOffset;
    }
  }
  // Pivot in place if only A/D (no W/S)
  else {
    if (aPressed) {
      left_speed  = -turnOffsettank;
      right_speed =  turnOffsettank;
    }
    else if (dPressed) {
      left_speed  =  turnOffsettank;
      right_speed = -turnOffsettank;
    }
  }

  // Constrain and write to motors
  left_speed  = constrain(left_speed,  -1023, 1023);
  right_speed = constrain(right_speed, -1023, 1023);
  setMotor(10, left_speed);
  setMotor(9,  right_speed);
}

#endif
