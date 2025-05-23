#include <Leaphyoriginal1.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "boot.h"
#include "mode1.h"
#include "mode2.h"
#include "mode3.h"
#include "mode4.h"
#include "sensors.h"

// ==================== CONFIG & GLOBALS ====================

// I²C LCD (20×4) at 0x3F
LiquidCrystal_I2C lcd(0x3F, 20, 4);

// Software serial to ESP (RX=13, TX=15)
SoftwareSerial espSerial(13, 15);

// Whether to run the boot animation at startup
bool do_boot = false;

// Current drive mode (1–4). Default to 0 (no mode)
int modeInt = 0;

// Non‐blocking serial‐line parsing buffer
static const uint8_t SERIAL_BUF_LEN = 64;
char serialBuf[SERIAL_BUF_LEN];
uint8_t serialPos = 0;

// Variables shared by modes & sensors
String incomingData = "";
int speed = 500;         // base speed (adjusted in mode1)
int left_speed = 0;
int right_speed = 0;
int default_speed = 100; // used by mode2

double avg_dis = 0;      // updated once per DISTANCE_INTERVAL_MS
double light_left = 0;   // updated once per LIGHT_INTERVAL_MS
double light_right = 0;

// Timing for sensor updates (millis-based)
const unsigned long DISTANCE_INTERVAL_MS = 200;
const unsigned long LIGHT_INTERVAL_MS    = 500;
unsigned long lastDistanceMillis = 0;
unsigned long lastLightMillis    = 0;

// ==================== FORWARD DECLARATIONS ====================

// Serial handling
void handleSerial();
void handleMode();

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);

  // Seed random() from floating analog pin (A5) for mode3
  randomSeed(analogRead(A5));

  boot();  // run boot animation if do_boot == true
}

void loop() {
  handleSerial();

  handleMode();

  // 3) Periodic distance update (one ultrasonic ping)
  if (millis() - lastDistanceMillis >= DISTANCE_INTERVAL_MS) {
    lastDistanceMillis = millis();
    get_distance_one_shot();
  }

  // 4) Periodic light update (one analog read)
  if (millis() - lastLightMillis >= LIGHT_INTERVAL_MS) {
    lastLightMillis = millis();
    get_light_one_shot();
  }
}

// ==================== SERIAL HANDLING (NON-BLOCKING) ====================

void handleSerial() {
  while (Serial.available()) {
    char c = Serial.read();

    // If newline or carriage return: process buffer
    if (c == '\n' || c == '\r') {
      if (serialPos > 0) {
        serialBuf[serialPos] = '\0';          // Null-terminate
        String line = String(serialBuf);
        line.trim();
        Serial.println("IncomingData: " + line);

        // If it's "Mode: X", update modeInt
        if (line.startsWith("Mode:")) {
          String tmp = line.substring(5);
          tmp.trim();
          int m = tmp.toInt();
          if (m >= 1 && m <= 4) {
            modeInt = m;
            Serial.print(">> Set mode to ");
            Serial.println(modeInt);
          } else {
            Serial.println(">> Invalid mode: " + tmp);
          }
        }

        if (line.startsWith("message_to_lcd:")){
        String lcd_message = line.substring(15);
        lcd_message.trim();
        print_to_lcd("", "", 0.0, true);
        print_to_lcd(String(lcd_message), "", 0.0, false);
        }

        if (line.startsWith("clear_lcd")) {
        print_to_lcd("", "", 0.0, true);
        }
        // Otherwise, if in mode1, pass Key commands
        else if (modeInt == 1 &&
                 (line.startsWith("Key Down:") || line.startsWith("Key Up:"))) {
          incomingData = line;
        }
      }
      // Reset buffer for next line
      serialPos = 0;
    }
    else {
      // Append char to buffer if space remains
      if (serialPos < SERIAL_BUF_LEN - 1) {
        serialBuf[serialPos++] = c;
      }
      // Else: overflow, ignore until newline
    }
  }
}

void handleMode() {
  switch (modeInt) {
    case 1:
      mode1();
      break;
    case 2:
      mode2();
      break;
    case 3:
      mode3();
      break;
    case 4:
      mode4();
      break;
    default:
      // No valid mode selected: stop both motors
      setMotor(10, 0);
      setMotor(9,  0);
      break;
  }
}

