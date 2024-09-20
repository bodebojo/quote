#include <Leaphyoriginal1.h>
#include <SoftwareSerial.h> // For software serial communication if needed
SoftwareSerial espSerial(13, 15); // Adjust pins if necessary
int speed = 500;
bool shift_pressed = false;


void setup() {
  Serial.begin(9600); // Initialize serial communication with the computer
  espSerial.begin(9600); // Initialize serial communication with the ESP`
  setLed(255, 0, 0);
}

void loop() {
  // Check if there is incoming data on the serial
  if (Serial.available()) {
    String incomingData = Serial.readStringUntil('\n'); // Read the incoming data as a string until newline
    incomingData.trim(); // Remove any trailing whitespace
    Serial.println(speed); // Print the speed variable for debug purposes
    Serial.println(incomingData); // Print the received data
    // Set speed based on shift input
    if (incomingData == "Shift key pressed") {
      if (shift_pressed == false) {
        speed = 1000;
        shift_pressed = true;
        setLed(0, 255, 0);
      }
      else if (shift_pressed == true) {
        speed = 500;
        shift_pressed = false;
        setLed(255, 0, 0);
      }
    }
    // Compare the incoming data
    if (incomingData == "W key pressed") {
      setMotor(10, speed);
      setMotor(9, speed);
      Serial.println("Received command1");
    }
    else if (incomingData == "Y key pressed") {
      setMotor(10, -speed);
      setMotor(9, -speed);
    }
    else if (incomingData == "X key pressed") {
      setMotor(10, -speed/2.5);
      setMotor(9, speed/2.5);
    } 
    else if (incomingData == "Z key pressed") {
      setMotor(10, speed/2.5);
      setMotor(9, -speed/2.5);
    } 
    else{ 
      setMotor(9, 0);
      setMotor(10, 0);    
    }
  }
}
