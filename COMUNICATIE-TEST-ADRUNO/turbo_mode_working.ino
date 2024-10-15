#include <Leaphyoriginal1.h>
#include <SoftwareSerial.h> // For software serial communication if needed
SoftwareSerial espSerial(13, 15); // Adjust pins if necessary
int speed = 500;


void setup() {
  Serial.begin(9600); // Initialize serial communication with the computer
  espSerial.begin(9600); // Initialize serial communication with the ESP`
  
}

void loop() {
  // Check if there is incoming data on the serial
  if (Serial.available()) {
    // Read the incoming data as a string
    String incomingData = Serial.readStringUntil('\n');
    incomingData.trim(); // Remove any trailing whitespace
    // Print the received data
    Serial.println(incomingData);
    // Set speed based on shift input
    
    
    if (incomingData == "Shift key pressed") {
      speed = 500;
      setLed(0, 255, 0);
      Serial.println(speed);
    }
    else if (incomingData == "Shift key released") {
      speed = 250;
      setLed(0, 0, 255);
      Serial.println(speed);
    }
    // Compare the incoming data
    else if (incomingData == "W key pressed") {
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
    else if (incomingData == "Shift key pressed") {
      speed = 500;
      Serial.println(speed);
      setLed(0, 255, 0);
      
    }
    else if (incomingData == "Shift key released") {
      speed = 250;
      Serial.println(speed);
      setLed(0, 0, 255);
      
    }
    else{ 
      setMotor(9, 0);
      setMotor(10, 0);    
    }
  }
}
