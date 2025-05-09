#ifndef MODE1_H
#define MODE1_H

extern String incomingData;
extern int speed;


void mode1(){
// Set speed based on shift input
    if (incomingData == "Shift key pressed") {
      speed = 500;
      setLed(0, 255, 0);
  } if (incomingData == "Shift key released") {
      speed = 250;
      setLed(0, 0, 255);
  } if (incomingData == "w key pressed") {
      setMotor(10, speed);
      setMotor(9, speed);
  } if (incomingData == "s key pressed") {
      setMotor(10, -speed);
      setMotor(9, -speed);
  } if (incomingData == "a key pressed") {
      setMotor(10, -speed/2.5);
      setMotor(9, speed/2.5);
  } if (incomingData == "d key pressed") {
      setMotor(10, speed/2.5);
      setMotor(9, -speed/2.5);
  } if (incomingData == "Shift key pressed") {
      speed = 500;
      setLed(0, 255, 0);      
  }//else{ 
//    setMotor(9, 0);
//    setMotor(10, 0);    
//}
}

#endif