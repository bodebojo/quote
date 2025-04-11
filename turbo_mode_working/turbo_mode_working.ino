#include <Leaphyoriginal1.h>
#include <SoftwareSerial.h> // For software serial communication if needed
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

SoftwareSerial espSerial(13, 15); // Adjust pins if necessary
LiquidCrystal_I2C lcd(0x3F,20,4);

bool do_boot = false;
int speed = 500;
int right_speed = 0;
int left_speed = 0;
int default_speed = 100;
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
  randomSeed(time(0));

  lcd.init();
  lcd.createChar(0, block);
  lcd.createChar(1, heart0);
  lcd.createChar(2, heart1);
  lcd.createChar(3, heart2);
  lcd.createChar(4, heart3);
  lcd.backlight();
  if (do_boot == true) {
  boot(2.5);
  }
}

void move(int mode) {
  Serial.println(mode);
  String incomingData = Serial.readStringUntil('\n');
  incomingData.trim();

  if (incomingData == "CDM1") {
    mode = 1;
  }
  if (incomingData == "CDM2") {
    mode = 2;
  }
  if (incomingData == "CDM3") {
    mode = 3;
  }

  if (mode == 1) {
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
      }
      if (incomingData == "S key pressed") {
        setMotor(10, -speed);
        setMotor(9, -speed);
      }
      if (incomingData == "A key pressed") {
        setMotor(10, -speed/2.5);
        setMotor(9, speed/2.5);
      } 
      if (incomingData == "D key pressed") {
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
      // else{ 
      //   setMotor(9, 0);
      //   setMotor(10, 0);    
      // }
    }
  }
  if (mode == 2) {
    int right_line_value = digitalRead(16); // Read the value of the right line follower sensor.
    int left_line_value = digitalRead(17);  // Read the value of the left line follower sensor.
    // 0 is detecting black, 1 is detecting white
    if (avg_dis <= 10) {
      setMotor(10, 0);
      setMotor(9, 0);
    }
    if (right_speed == 0) {
      right_speed = default_speed;
      Serial.println("Setting default speed for right motor");
    }
    if (left_speed == 0) {
      left_speed = default_speed;
      Serial.println("Setting default speed for left motor");
    }
    if (default_speed > 25) {
      // if (default_speed == 100) {
      //   setMotor(10, 100);
      //   setMotor(9, 100);
      //   default_speed = 50;
      // }
      Serial.println("Default Speed (" + String(default_speed) + ") is too high decreasing by 5");
      default_speed = default_speed - 5;
    }


    if (right_line_value == 1 && left_line_value == 1) {
      setMotor(10, right_speed);
      setMotor(9, left_speed);
    }
    else if (right_line_value == 1 && left_line_value == 0) {
      right_speed = right_speed / 2;
      left_speed = default_speed;
      setMotor(10, right_speed);
      setMotor(9, left_speed); 
    }
    else if (right_line_value == 0 && left_line_value == 1) {
      left_speed = left_speed / 2;
      right_speed = default_speed;
      setMotor(10, right_speed);
      setMotor(9, left_speed); 
    }
    else {
      setMotor(10, default_speed);
      setMotor(9, default_speed); 
    }
  }
  if (mode == 3) {
    int SPEED = 1250;
    int loop_amount = 10;
    int loop_count = 0;
    int i = 0;
    int randomIndex = random(-1, 2);
    int stuck = 0; 

    for (i = 1; i <= loop_amount; i++) {
      get_distance();
      
      if (avg_dis <= 30) {
        stuck += 1;
        Serial.println("Stuck: "+ String(stuck, 3));
        setMotor(10, -100);
        setMotor(9, -100);
        if (stuck < 10) {
          delay(250);
        }
        if (stuck >= 11) {
          delay(500);
        }

        if (randomIndex == 0) {
          Serial.println("Random: 0");
          setMotor(10, -100);
          setMotor(9, 100);
        }
        if (randomIndex == 1) {
          Serial.println("Random: 1");
          setMotor(10, 100);
          setMotor(9, -100);
        }
        delay(250);
        if (avg_dis >= 30) {
          setMotor(10, 100);
          setMotor(9, 100);
        }
      }

      else {
        setMotor(10, SPEED);
        setMotor(9, SPEED);
      }
    }
  }
  if (mode == 4) {
    int SPEED = 500;

    setMotor(10, SPEED);
    setMotor(9, SPEED/4);
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
    n = constrain(n, (prev_n-50), (prev_n+50)); // Limit the distance
    sum += n; // Add the distance to the sum
    prev_n = n; // Record the old distance in prev_n
  }
  loop_count++;
  avg_dis = sum / loop_amount; // Calculate the average distance

  print_to_lcd("AVG Distance:", String(avg_dis, 3), 0.1, false);

  if (loop_count = 10) {
    Serial.println("Average distance: "+String(avg_dis, 3));
    // Serial.println(avg_dis); // Print the average distance to the serial monitor
    loop_count = 0;
  }
  i = 0;
  sum = 0;
}

String pickRandomString(String stringList[], int listSize) {
  int randomIndex = random(listSize);
  return stringList[randomIndex];
}

void show_faces() {
  // lcd.autoscroll();
  // String faces[] = {">:3", ":0", ":p", ":D", ">:D"};
  // int randomIndex0 = random(0, 5);
  // String randomFace0 = faces[randomIndex0];
  // int randomIndex1 = random(0, 5);
  // String randomFace1 = faces[randomIndex1];
  // String information = "The quick brown fox jumps over the lazy"; // 40 chars per line limit

  String empty_string = "                ";

  String eye[] = {"O", "#", "$", "*", "V", "-", "|", "Z"};
  String eyes[] =   {"  O          0  ", "  -          O  ", "  V          V  ", "  |          |  ", "  -          -  ", "  #          #  ", "  $          $  "};
  String mouths[] = {"       ==       ", "       ()       ", "       []       ", "       vv       ", "       ##       "};
  
  int eye_list_size = sizeof(eye) / sizeof(eye[0]);
  int eyes_list_size = sizeof(eyes) / sizeof(eyes[0]);
  int mouths_list_size = sizeof(mouths) / sizeof(mouths[0]);

  String random_eye1 = pickRandomString(eyes, eyes_list_size);
  String random_eye2 = pickRandomString(eyes, eyes_list_size);

  // String random_eyes = pickRandomString(eyes, eyes_list_size);

  String random_mouth = pickRandomString(mouths, mouths_list_size);

  String double_eyes = ""+random_eye1+"          "+random_eye2+" ";

  print_to_lcd(double_eyes, random_mouth, 2, true);

}

void print_to_lcd(String first_line, String second_line, int pause, bool clear) {
  float pause_ms = pause * 1000;

  lcd.setCursor(0,0);
  lcd.print(first_line);
  lcd.setCursor(0,1);
  lcd.print(second_line);

  delay(pause_ms);
  if (clear == true) {
    lcd.clear();
  }
}

void loop() {

  move(3); //(1: Web Based, 2: Line Following, 3: Self Driving, 4: Circles ^^)

  // show_faces();

  avg_dis = 0;
}