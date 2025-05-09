#include "HardwareSerial.h"
#ifndef MODE1_H
#define MODE1_H

extern String incomingData;
extern int speed;
extern int left_speed;
extern int right_speed;

void mode1(){
// Set speed based on shift input
    if (incomingData == "Key Down: Shift") {
      speed = 500;
      setLed(0, 255, 0);
    } 
    if (incomingData == "Key Up: Shift") {
      speed = 250;
      setLed(0, 0, 255);
    } 
    if (incomingData == "Key Down: w") {
    left_speed =+ speed;
    right_speed =+ speed;
      Serial.println("W Key Down");
    } 
    if (incomingData == "Key Down: s") {
    left_speed =- speed;
    right_speed =- speed;
      Serial.println("S Key Down");
    } 
    if (incomingData == "Key Down: a") {
    left_speed =- speed;
    right_speed =+ speed;
      Serial.println("A Key Down");
    } 
    if (incomingData == "Key Down: d") {
    left_speed =+ speed;
    right_speed =- speed;
      Serial.println("D Key Down Detected");
    }
    if (incomingData == "Key Up: w") {
    left_speed =- speed;
    right_speed =- speed;
      Serial.println("W Key Up");
    } 
    if (incomingData == "Key Up: s") {
    left_speed =+ speed;
    right_speed =+ speed;
      Serial.println("S Key Up");
    } 
    if (incomingData == "Key Up: a") {
    left_speed =+ speed;
    right_speed =- speed;
      Serial.println("A Key Up");
    } 
    if (incomingData == "Key Up: d") {
    left_speed =- speed;
    right_speed =+ speed;
      Serial.println("D Key Up");
    }
    setMotor(9, left_speed);
    setMotor(10, right_speed);
    //else{ 
    //    setMotor(9, 0);
    //    setMotor(10, 0);    
    //}
}

#endif