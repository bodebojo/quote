#include <Leaphyoriginal1.h>
#include <SoftwareSerial.h> // For software serial communication if needed
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <string.h>
#include <stdlib.h>

SoftwareSerial espSerial(13, 15); // Adjust pins if necessary
LiquidCrystal_I2C lcd(0x3F,20,4);

int speed = 500;
double avg_dis;  // Declaration of variable 'avg_dis' to store the average

byte block[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
byte heart0[8] = {
  0b00000,
  0b00000,
  0b00110,
  0b01111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
byte heart1[8] = {
  0b00000,
  0b00000,
  0b01100,
  0b11110,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
byte heart2[8] = {
  0b11111,
  0b11111,
  0b01111,
  0b00111,
  0b00011,
  0b00001,
  0b00000,
  0b00000
};
byte heart3[8] = {
  0b11111,
  0b11111,
  0b11110,
  0b11100,
  0b11000,
  0b10000,
  0b00000,
  0b00000
};

void boot(int boot_time_s) {
  int boot_time_half_s = boot_time_s /2;
  int boot_time_half_ms = boot_time_half_s * 1000;
  int boot_delay = boot_time_half_ms/ 17;
  int randomIndex = random(0, 5);

  String faces[] = {">:3", ":0", ":p", ":D", ">:D"};
  String randomFace = faces[randomIndex];

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);

  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 16; col++) {
      lcd.setCursor(col, row);
      lcd.write(byte(0));
      delay(boot_delay);
    }
  }
  delay(boot_delay);
  lcd.clear();

  lcd.setCursor(7,0);
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.setCursor(7,1);
  lcd.write(byte(3));
  lcd.write(byte(4));

  delay(boot_time_half_ms);
  lcd.clear();
}

void setup() {
  Serial.begin(9600); // Initialize serial communication with the computer
  espSerial.begin(9600); // Initialize serial communication with the ESP`
  randomSeed(analogRead(0));

  lcd.init();
  lcd.createChar(0, block);
  lcd.createChar(1, heart0);
  lcd.createChar(2, heart1);
  lcd.createChar(3, heart2);
  lcd.createChar(4, heart3);

  boot(2.5);
  
}

void move() {
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
    if (incomingData == "Shift key released") {
      speed = 250;
      setLed(0, 0, 255);
      Serial.println(speed);
    }
    // Compare the incoming data
    if (incomingData == "W key pressed") {
      setMotor(10, speed);
      setMotor(9, speed);
      Serial.println("Received command1");
    }
    if (incomingData == "Y key pressed") {
      setMotor(10, -speed);
      setMotor(9, -speed);
    }
    if (incomingData == "X key pressed") {
      setMotor(10, -speed/2.5);
      setMotor(9, speed/2.5);
    } 
    if (incomingData == "Z key pressed") {
      setMotor(10, speed/2.5);
      setMotor(9, -speed/2.5);
    }
    if (incomingData == "Shift key pressed") {
      speed = 500;
      Serial.println(speed);
      setLed(0, 255, 0);
      
    }
    if (incomingData == "Shift key released") {
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

void get_distance() {
  // Loop to read loop_amount numbers
  int loop_amount = 10;
  int loop_count = 0;
  int i, n, prev_n, sum = 0; // Declaration of variables 'i' for iteration, 'n' to store each number, and 'sum' to store their total sum
  for (i = 1; i <= loop_amount; i++) {
    n = getDistance(); // Get the distance from the sensor
    if (prev_n == 0) {prev_n = n;}
    n = constrain(n, (prev_n-5), (prev_n+5)); // Limit the distance
    sum += n; // Add the distance to the sum
    prev_n = n; // Record the old distance in prev_n
    delay(25);
  }
  loop_count++;
  avg_dis = sum / loop_amount; // Calculate the average distance
  print_to_lcd("AVG Distance:", String(avg_dis, 3), 0.25);

  if (loop_count = 10) {
    // Serial.println("Average distance: ");
    // Serial.println(avg_dis); // Print the average distance to the serial monitor
    loop_count = 0;
  }
  i = 0;
  sum = 0;
}

void get_line_information() {

  int rightLineValue = digitalRead(16); // Read the value of the right line follower sensor.
  int leftLineValue = digitalRead(17);  // Read the value of the left line follower sensor.

  if (leftLineValue == 0 && rightLineValue == 0)
  {
      // The robot is on the line.
      Serial.println("The robot is on the line.");
  }
  else if (leftLineValue == 0 && rightLineValue == 1)
  {
      // The robot is to the left of the line.
      Serial.println("The robot is to the left of the line.");
  }
  else if (leftLineValue == 1 && rightLineValue == 0)
  {
      // The robot is to the right of the line.
      Serial.println("The robot is to the right of the line.");
  }
  else {
    Serial.println("The robot is flying in space and cant see a line no matter how far it looks.");
  }
}

void print_to_lcd(String first_line, String second_line, int pause) {
  float pause_ms = pause * 1000;

  lcd.setCursor(0,0);
  lcd.print(first_line);
  lcd.setCursor(0,1);
  lcd.print(second_line);

  delay(pause_ms);
}

void loop() {
  move();
  get_distance();

  avg_dis = 0;

  int pin16State = digitalRead(16);
  Serial.println("Pin 16 state: " + String(pin16State));
  int pin17State = digitalRead(17);
  Serial.println("Pin 17 state: " + String(pin17State));  
}
