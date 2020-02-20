#include <SPI.h>
#include "radio.h"

void setup() {
    Serial.begin(115200);
    Serial.println(F("starting"));

    radioConfigure();
    radioFetch();
}

void loop() {
    delay(500);
}
