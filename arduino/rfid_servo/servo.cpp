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
            Serial.print(F("servo unlocking at "));
            Serial.println(status.unlock_time);
            status.unlocked = true;
            servo.write(SERVO_UNLOCK);
        }
    } else if (status.unlocked) {
        unsigned long now = millis();
        if (now - status.unlock_time > SERVO_DELAY) {
            Serial.print(F("servo locking at "));
            Serial.println(now);
            status.unlocked = false;
            servo.write(SERVO_LOCK);
        }
    }
}
