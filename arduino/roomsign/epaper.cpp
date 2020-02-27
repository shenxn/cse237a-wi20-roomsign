#include <SPI.h>
#include "epd2in9.h"
#include "epdpaint.h"
#include "imagedata.h"
#include "status.h"

#define COLORED     0
#define UNCOLORED   1

Epd epd;
unsigned char image[480];
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
    Serial.println((int)status.event.available);

    if (epd.Init(lut_full_update) != 0) {
        Serial.print("e-Paper init failed");
        return;
    }

    if (status.event.available) {
        epd.SetFrameMemory(IMAGE_DATA_AVAILABLE);
        epd.DisplayFrame();
    } else {
        epd.SetFrameMemory(IMAGE_DATA_INUSE);

        paint.SetRotate(ROTATE_90);
        paint.SetWidth(24);
        paint.SetHeight(160);

        int x = HEIGHT - ROOM_NAME_HEIGHT - DETAIL_PADDING - DETAIL_LINE_HEIGHT;
        int y = STATUS_WIDTH + DETAIL_PADDING;
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 4, status.event.summary, &Font20, COLORED);
        epd.SetFrameMemory(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());

        x -= DETAIL_LINE_HEIGHT;
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 4, status.event.time, &Font16, COLORED);
        epd.SetFrameMemory(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());

        x -= DETAIL_LINE_HEIGHT;
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 4, status.event.creator, &Font16, COLORED);
        epd.SetFrameMemory(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());

        epd.DisplayFrame();
    }

    epd.Sleep();
}
