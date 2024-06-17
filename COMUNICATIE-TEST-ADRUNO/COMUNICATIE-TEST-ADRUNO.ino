#include "Leaphyoriginal1.h" // voor LED's 
// this sample code provided by www.programmingboss.com
#define TXD2 13
#define RXD2 15

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600); //, SERIAL_8N1, 15, 13);
}

void loop() {
    Serial.println(Serial.readString());
    {
   {
      String s = (Serial.readString());
      
      // replace all newlines in s with spaces
      int nl;
      while ((nl = s.indexOf("\n")) >= 0) {
        s[ nl ] = ' ';
      }
      
      int o = s.indexOf("GPIO 26");
      if (o >= 0) {
        //Serial.println("I found an index of GPIO at position " + o);
        if (s.indexOf("on") >= 0) {
          Serial.println("yes");
        } else if (s.indexOf("off") >= 0) {
          Serial.println("no");
        } else {
          Serial.println("huh?");
        }
      } else {
        Serial.println("Unknown command: '" + s + "'");
      }
      //Serial.println(s);
   }
 
}
}