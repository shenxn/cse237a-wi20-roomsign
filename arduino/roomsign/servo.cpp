#include <Arduino.h>
#include <Servo.h>
#include "servo.h"
#include "status.h"
#include "macro.h"

Servo servo;


void servoWrite(int value) {
    servo.write(value);
    delay(500);  // wait for rotation finish
}


void servoSetup() {
    servo.attach(SERVO_PIN);
    servoWrite(SERVO_LOCK);
    status.unlocked = false;
}


void servoUnlock() {
    if (status.authorized) {
        status.authorized = false;
        status.unlock_time = status.clock;
        if (!status.unlocked) {
            SERIAL_PRINT(F("servo unlocking at "));
            SERIAL_PRINTLN(status.unlock_time);
            status.unlocked = true;
            servoWrite(SERVO_UNLOCK);
        }
    } else if (status.unlocked) {
        unsigned long now = status.clock;
        if (now - status.unlock_time > SERVO_DELAY) {
            SERIAL_PRINT(F("servo locking at "));
            SERIAL_PRINTLN(now);
            status.unlocked = false;
            servoWrite(SERVO_LOCK);
        }
    }
}
