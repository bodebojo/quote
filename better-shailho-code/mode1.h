#include "HardwareSerial.h"
#ifndef MODE1_H
#define MODE1_H

extern String incomingData;
extern int speed;


void mode1(){
// Set speed based on shift input
    if (incomingData == "Key Down: Shift") {
      speed = 500;
      setLed(0, 255, 0);
  } if (incomingData == "Key Up: Shift") {
      speed = 250;
      setLed(0, 0, 255);
  } if (incomingData == "Key Down: w") {
      setMotor(10, speed);
      setMotor(9, speed);
      Serial.println("w ket detecd");
  } if (incomingData == "Key Down: s") {
      setMotor(10, -speed);
      setMotor(9, -speed);
  } if (incomingData == "Key Down: a") {
      setMotor(10, -speed/2.5);
      setMotor(9, speed/2.5);
  } if (incomingData == "Key Down: d") {
      setMotor(10, speed/2.5);
      setMotor(9, -speed/2.5);
  } if (incomingData == "Key Down: Shift") {
      speed = 500;
      setLed(0, 255, 0);      
  }//else{ 
//    setMotor(9, 0);
//    setMotor(10, 0);    
//}
}

#endif