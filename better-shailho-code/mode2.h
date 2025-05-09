#ifndef MODE2_H
#define MODE2_H

extern int speed;
extern int right_speed;
extern int left_speed;
extern int default_speed;
extern double avg_dis;

void mode2() {

  int right_line_value = digitalRead(16); // Read the value of the right line follower sensor.
  int left_line_value = digitalRead(17);  // Read the value of the left line follower sensor.
  // 0 is detecting black, 1 is detecting white

  if (avg_dis <= 10) {
    setMotor(10, 0);
    setMotor(9, 0);
    return; // Stop and exit early
  }

  if (right_speed == 0) {
    right_speed = default_speed;
  }
  if (left_speed == 0) {
    left_speed = default_speed;
  }

  if (default_speed > 25) {
    default_speed = default_speed - 5;
  }

  if (right_line_value == 1 && left_line_value == 1) {
    setMotor(10, right_speed);
    setMotor(9, left_speed);
  } else if (right_line_value == 1 && left_line_value == 0) {
    right_speed = right_speed / 2;
    left_speed = default_speed;
    setMotor(10, right_speed);
    setMotor(9, left_speed);
  } else if (right_line_value == 0 && left_line_value == 1) {
    left_speed = left_speed / 2;
    right_speed = default_speed;
    setMotor(10, right_speed);
    setMotor(9, left_speed);
  } else {
    setMotor(10, default_speed);
    setMotor(9, default_speed);
  }
} 

#endif