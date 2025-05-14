#ifndef MODE1_H
#define MODE1_H

extern String incomingData;
extern int speed;
extern int left_speed;
extern int right_speed;
extern void setMotor(int motor, int speed);

// Key states
bool wPressed = false;
bool aPressed = false;
bool sPressed = false;
bool dPressed = false;
bool shiftPressed = false;

void mode1() {
    // Update key states based on incomingData
    if (incomingData == "Key Down: Shift") shiftPressed = true;
    else if (incomingData == "Key Up: Shift") shiftPressed = false;
    else if (incomingData == "Key Down: w") wPressed = true;
    else if (incomingData == "Key Up: w") wPressed = false;
    else if (incomingData == "Key Down: s") sPressed = true;
    else if (incomingData == "Key Up: s") sPressed = false;
    else if (incomingData == "Key Down: a") aPressed = true;
    else if (incomingData == "Key Up: a") aPressed = false;
    else if (incomingData == "Key Down: d") dPressed = true;
    else if (incomingData == "Key Up: d") dPressed = false;

    // Set base speed
    speed = shiftPressed ? 250 : 500;
    int turnOffset = speed / 3;

    // Default motor speeds
    left_speed = 0;
    right_speed = 0;

    // Forward/Backward
    if (wPressed) {
        left_speed = speed;
        right_speed = speed;
    } else if (sPressed) {
        left_speed = -speed;
        right_speed = -speed;
    }

    // Turning adjustments (only if moving)
    if (aPressed) {
        left_speed -= turnOffset;
        right_speed += turnOffset;
    } else if (dPressed) {
        left_speed += turnOffset;
        right_speed -= turnOffset;
    }

    // Clamp motor speeds to valid range
    left_speed = constrain(left_speed, -1023, 1023);
    right_speed = constrain(right_speed, -1023, 1023);

    setMotor(10, left_speed);
    setMotor(9, right_speed);

    incomingData = "";
}

#endif
