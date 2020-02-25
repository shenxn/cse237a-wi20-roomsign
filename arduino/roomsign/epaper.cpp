#include <SPI.h>
#include "epd2in9.h"
#include "epdpaint.h"
#include "imagedata.h"
#include "status.h"

#define COLORED     0
#define UNCOLORED   1

Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);

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


void epaperDisplay() {
    if (!status.updated) {
        return;
    }
    status.updated = false;

    Serial.print("display available: ");
    Serial.println(status.available);

    if (epd.Init(lut_full_update) != 0) {
        Serial.print("e-Paper init failed");
        return;
    }

    if (status.available) {
        epd.SetFrameMemory(IMAGE_DATA_AVAILABLE);
        epd.DisplayFrame();
        // epd.SetFrameMemory(IMAGE_DATA_AVAILABLE);
        // epd.DisplayFrame();
    } else {
        epd.SetFrameMemory(IMAGE_DATA_INUSE);
        // epd.DisplayFrame();
        // epd.SetFrameMemory(IMAGE_DATA_INUSE);
        // epd.DisplayFrame();

        paint.SetRotate(ROTATE_90);
        paint.SetWidth(24);
        paint.SetHeight(200);
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 4, status.name, &Font16, COLORED);
        epd.SetFrameMemory(paint.GetImage(), 128-24-16-24, 104+16+8*5, paint.GetWidth(), paint.GetHeight());
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 4, status.time, &Font16, COLORED);
        epd.SetFrameMemory(paint.GetImage(), 128-24-16-24*2, 104+16+8*5, paint.GetWidth(), paint.GetHeight());
        epd.DisplayFrame();
    }

    epd.Sleep();
}
