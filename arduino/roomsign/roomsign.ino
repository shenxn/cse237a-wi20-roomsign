#include <SPI.h>
#include "radio.h"
#include "epaper.h"
#include "status.h"
#include "serial.h"

void setup() {
    Serial.begin(9600);
    Serial.println(F("starting"));

    Serial.println(F("setup epaper"));
    epaperSetup();

    Serial.println(F("setup radio"));
    radioConfigure();

    Serial.println(F("setup soft serial"));
    serialBegin();

    Serial.println(F("initial status fetch"));
    radioFetch();
    epaperDisplay();
}

void loop() {
    serialRead();
    radioRead();
    epaperDisplay();
    delay(1000);
}
