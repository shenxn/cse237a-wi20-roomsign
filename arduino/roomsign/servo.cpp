#include <Arduino.h>
#include <Servo.h>
#include "servo.h"
#include "status.h"
#include "macro.h"

Servo servo;


void servoSetup() {
    servo.attach(SERVO_PIN);
    servo.write(SERVO_LOCK);
    status.unlocked = false;
}

void servoUnlock() {
    if (status.authorized) {
        status.authorized = false;
        status.unlock_time = millis();
        if (!status.unlocked) {
            SERIAL_PRINT(F("servo unlocking at "));
            SERIAL_PRINTLN(status.unlock_time);
            status.unlocked = true;
            servo.write(SERVO_UNLOCK);
        }
    } else if (status.unlocked) {
        unsigned long now = millis();
        if (now - status.unlock_time > SERVO_DELAY) {
            SERIAL_PRINT(F("servo locking at "));
            SERIAL_PRINTLN(now);
            status.unlocked = false;
            servo.write(SERVO_LOCK);
        }
    }
}
