#include <SPI.h>
#include "radio.h"
#include "epaper.h"
#include "status.h"
#include "rfid.h"
#include "servo.h"
#include "epdif.h"

void setup() {
    Serial.begin(9600);
    Serial.println(F("starting"));

    // set all SS pins to high to prevent SPI conflict
    pinMode(RFID_SS_PIN, OUTPUT);
    digitalWrite(RFID_SS_PIN, HIGH);
    pinMode(CS_PIN, OUTPUT);  // epaper
    digitalWrite(CS_PIN, HIGH);
    pinMode(RADIO_CSN_PIN, OUTPUT);
    digitalWrite(RADIO_CSN_PIN, HIGH);

    Serial.println(F("setup rfid"));
    rfidInit();

    Serial.println(F("setup servo"));
    servoSetup();

    Serial.println(F("setup epaper"));
    epaperSetup();

    // should be after epaper
    Serial.println(F("setup radio"));
    radioConfigure();
}

void loop() {
    rfidRead();
    servoUnlock();
    delay(1000);
}
