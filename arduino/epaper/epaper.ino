#include <SPI.h>
#include "radio.h"
#include "epaper.h"

void setup() {
    Serial.begin(115200);
    Serial.println(F("starting"));

    Serial.println(F("setup epaper"));
    epaperSetup();

    Serial.println(F("setup radio"));
    radioConfigure();

    Serial.println(F("initial status fetch"));
    unsigned int available = radioFetch();
    epaperDisplay(available);
}

void loop() {
    delay(500);
}
