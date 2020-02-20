#include <SPI.h>
#include "epd2in9.h"
#include "epdpaint.h"
#include "imagedata.h"

Epd epd;

void epaperSetup() {
    if (epd.Init(lut_full_update) != 0) {
        Serial.print("e-Paper init failed");
        return;
    }

    epd.ClearFrameMemory(0xFF);
    epd.DisplayFrame();
    epd.ClearFrameMemory(0xFF);
    epd.DisplayFrame();

    epd.Sleep();
}


void epaperDisplay(unsigned int available) {
    Serial.print("display available: ");
    Serial.println(available);

    if (epd.Init(lut_full_update) != 0) {
        Serial.print("e-Paper init failed");
        return;
    }

    if (available) {
        epd.SetFrameMemory(IMAGE_DATA_AVAILABLE);
        epd.DisplayFrame();
        epd.SetFrameMemory(IMAGE_DATA_AVAILABLE);
        epd.DisplayFrame();
    } else {
        epd.SetFrameMemory(IMAGE_DATA_INUSE);
        epd.DisplayFrame();
        epd.SetFrameMemory(IMAGE_DATA_INUSE);
        epd.DisplayFrame();
    }

    epd.Sleep();
}
