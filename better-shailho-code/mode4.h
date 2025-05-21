#ifndef MODE4_H
#define MODE4_H

// Simply stop both motors
void mode4() {
  setMotor(10, 0);
  setMotor(9,  0);
}

#endif
