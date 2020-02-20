#include <SPI.h>
#include "radio.h"
#include "epaper.h"

void setup() {
    Serial.begin(115200);
    Serial.println(F("starting"));

    radioConfigure();
    radioFetch();
    epaperSetup();
}

void loop() {
    delay(500);
}
