#include <SoftwareSerial.h>
#include "serial.h"
#include "rfid.h"
#include "servo.h"

void setup() {
    Serial.begin(115200);    // Initialize serial communications with the PC

    Serial.println(F("initializing softSerial"));
    serialBegin();

    Serial.println(F("initializing RFID reader"));
    rfidInit();

    Serial.println(F("initializing servo"));
    servoSetup();

    serialRequest();
}

void loop() {
    serialRead();
    rfidRead();
    servoUnlock();
    delay(100);
}
