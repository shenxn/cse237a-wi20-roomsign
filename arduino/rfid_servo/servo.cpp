#include <Arduino.h>
#include <Servo.h>
#include "servo.h"
#include "status.h"

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
            Serial.println(F("servo unlocking"));
            status.unlocked = true;
            servo.write(SERVO_UNLOCK);
        }
    } else if (status.unlocked) {
        if (millis() - status.unlock_time > SERVO_DELAY) {
            Serial.println(F("servo locking"));
            status.unlocked = false;
            servo.write(SERVO_LOCK);
        }
    }
}
