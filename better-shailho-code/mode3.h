#ifndef MODE3_H
#define MODE3_H

void mode3(){
 int SPEED = 1250;
 int loop_amount = 10;
 int loop_count = 0;
 int i = 0;
 int randomIndex = random(-1, 2);
 int stuck = 0; 

  for (i = 1; i <= loop_amount; i++) {     
     if (avg_dis <= 30) {
        stuck += 1;
        setMotor(10, -100);
        setMotor(9, -100);
      if (stuck < 10) {
        delay(250);
    } if (stuck >= 11) {
        delay(500);
    } if (randomIndex == 0) {
        setMotor(10, -100);
        setMotor(9, 100);
    } if (randomIndex == 1) {
        setMotor(10, 100);
        setMotor(9, -100);
    } delay(250);
      if (avg_dis >= 30) {
        setMotor(10, 100);
        setMotor(9, 100);
   }
    } else {
      setMotor(10, SPEED);
      setMotor(9, SPEED);
    }
  }
}

#endif