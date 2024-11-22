#include <Leaphyoriginal1.h>
#include <SoftwareSerial.h> // For software serial communication if needed
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <string.h>
#include <stdlib.h>

SoftwareSerial espSerial(13, 15); // Adjust pins if necessary
LiquidCrystal_I2C lcd(0x3F,20,4);

int speed = 500;
int i, n, sum = 0;  // Declaration of variables 'i' for iteration, 'n' to store each number, and 'sum' to store their total sum
  double avg;  // Declaration of variable 'avg' to store the average

void boot() {
  int boot_time = 5000;
  int boot_delay = boot_time / 16;
  int randomIndex = random(0, 5);

  String faces[] = {">:3", ":0", ":p", ":D", ">:D"};
  String randomFace = faces[randomIndex];

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  delay(boot_delay);

  for (int i = 0; i < 16; i++) {
    String progress = "";
    for (int j = 0; j <= i; j++) {
      progress += "";
    }
    lcd.print(progress); // Print progress bar
    lcd.setCursor(0,1); // Move cursor to second row
    lcd.print(progress); // Print progress bar
    lcd.setCursor(0,0); // Move cursor back to the beginning
    delay(boot_delay); // Wait
  }
  delay(boot_delay * 2);
  lcd.clear();

  lcd.setCursor(6,0);
  lcd.print(randomFace);
  delay(boot_delay * 4);
  lcd.clear();
}

void setup() {
  Serial.begin(9600); // Initialize serial communication with the computer
  espSerial.begin(9600); // Initialize serial communication with the ESP`
  randomSeed(analogRead(0));
  boot();
  
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

  // Loop to read loop_amount numbers
  int loop_amount = 500;
  for (i = 1; i <= 500; i++)  {
    n = getDistance();
    if (n >= 256) {n = 256;}
    sum += n;
    avg = sum / 500;
  }
  // avg = avg.mod;
  if (i = 500) {
    sum = 0;
    i = 0;
  }

  Serial.println("Average distance: ");
  Serial.println(avg);

  lcd.setCursor(0,0);
  lcd.print("AVG distance: ");
  lcd.setCursor(0,1);
  lcd.print(avg);

  avg = 0;

}
