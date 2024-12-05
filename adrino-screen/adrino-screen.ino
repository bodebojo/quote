#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
#include <TimerOne.h>
 
//Default address 0x27, LCD is 16x2
LiquidCrystal_I2C lcd(0x27,16,2);
 
void setup(){
  //Initialize the LCD
  lcd.init();
  //Turn On Backlight
  lcd.backlight();
 
  lcd.print("PCF8574T I2C LCD");
  lcd.setCursor(0,1);
  lcd.print("Arduino Example");
  delay(3000);
  lcd.clear();
 
  lcd.home();
  lcd.print("Powered Up Time:");
 
  //Timer1 duration is 1 second
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(secondCounts);
}
 
int days,hours,minutes,seconds;
 
void loop(){
  lcd.setCursor(0,1);
  //Show the days, hour, minute, and second
  lcd.print(String(days)+" "+String(hours)+":"+String(minutes)+":"+String(seconds)+" ");
}
 
void secondCounts(){
  seconds++;
 
  //Finding the minutes
  if(seconds>=60){
    minutes++;
    seconds=0;
  }
  //Finding the hours
  if(minutes>=60){
    hours++;
    minutes=0;
  }
  //Finding the days
  if(hours>=24){
    days++;
    hours=0;
  }
}