#ifndef MODE3_H
#define MODE3_H

#include <Arduino.h>

// Globals from main.ino:
extern double avg_dis;
extern void setMotor(int motor, int speed);

// Obstacle‐avoidance “wander” mode
void mode3() {
  const int DRIVE_SPEED = 1250;
  static int stuckCounter = 0;
  static int pivotDir     = 0; // 0 = pivot left, 1 = pivot right
  static unsigned long pivotStart = 0;
  const unsigned long PIVOT_DURATION_MS = 250;

  // If obstacle is close:
  if (avg_dis <= 30.0) {
    stuckCounter++;
    // First back up a bit
    setMotor(10, -100);
    setMotor(9,  -100);

    // After backing for ~10 calls, start pivot
    if (stuckCounter >= 10) {
      if (pivotStart == 0) {
        pivotDir   = random(0, 2); // pick left or right
        pivotStart = millis();
      }
      // Pivot for a fixed duration
      if (millis() - pivotStart <= PIVOT_DURATION_MS) {
        if (pivotDir == 0) {
          setMotor(10, -100);
          setMotor(9,  100);
        } else {
          setMotor(10,  100);
          setMotor(9,  -100);
        }
      } else {
        // Done pivoting: reset and on next loop drive forward
        stuckCounter = 0;
        pivotStart   = 0;
      }
    }
  }
  else {
    // No obstacle: drive straight
    setMotor(10, DRIVE_SPEED);
    setMotor(9,  DRIVE_SPEED);
  }
}

#endif
