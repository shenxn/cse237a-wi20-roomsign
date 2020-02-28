#ifndef SERVO_H
#define SERVO_H

#define SERVO_PIN 6
#define SERVO_LOCK 0
#define SERVO_UNLOCK 90
#define SERVO_DELAY 5  // 5 loop cycles (~1s per cycle)

void servoSetup();

void servoUnlock();

#endif
