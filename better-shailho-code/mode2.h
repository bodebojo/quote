#ifndef MODE2_H
#define MODE2_H

// Globals from main.ino:
extern int default_speed;
extern int right_speed;
extern int left_speed;
extern double avg_dis;
extern void setMotor(int motor, int speed);

// Line‐follower mode (two digital pins: 16 = right line, 17 = left line)
void mode2() {
  int right_line_value = digitalRead(16); // 0 = black, 1 = white
  int left_line_value  = digitalRead(17); // 0 = black, 1 = white

  // If obstacle is too close, stop immediately
  if (avg_dis <= 10.0) {
    setMotor(10, 0);
    setMotor(9,  0);
    return;
  }

  // Initialize speeds if zero
  if (right_speed == 0) right_speed = default_speed;
  if (left_speed  == 0) left_speed  = default_speed;

  // Gradually decrease default_speed to slow down smoothly
  if (default_speed > 25) {
    default_speed -= 5;
  }

  // Both sensors see white → straight
  if (right_line_value == 1 && left_line_value == 1) {
    setMotor(10, right_speed);
    setMotor(9,  left_speed);
  }
  // Veer right: right sees white, left sees black
  else if (right_line_value == 1 && left_line_value == 0) {
    right_speed = right_speed / 2;
    left_speed  = default_speed;
    setMotor(10, right_speed);
    setMotor(9,  left_speed);
  }
  // Veer left: left sees white, right sees black
  else if (right_line_value == 0 && left_line_value == 1) {
    left_speed  = left_speed / 2;
    right_speed = default_speed;
    setMotor(10, right_speed);
    setMotor(9,  left_speed);
  }
  // Both see black → very slow straight
  else {
    setMotor(10, default_speed);
    setMotor(9,  default_speed);
  }
}

#endif
