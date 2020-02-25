#include <SPI.h>
#include "radio.h"
#include "epaper.h"
#include "status.h"
#include "rfid.h"
#include "servo.h"

void setup() {
    Serial.begin(115200);
    Serial.println(F("starting"));

    Serial.println(F("setup RFID"));
    rfidInit();

    Serial.println(F("setup epaper"));
    epaperSetup();

    Serial.println(F("setup radio"));
    radioConfigure();

    Serial.println(F("setup servo"));
    servoSetup();

    Serial.println(F("initial status fetch"));
    radioFetch();
    // status.available = false;
    // status.updated = true;
    epaperDisplay();
}

void loop() {
    radioRead();
    epaperDisplay();
    rfidRead();
    servoUnlock();
    delay(500);
}
