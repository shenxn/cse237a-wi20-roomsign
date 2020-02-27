#include <SPI.h>
#include "epd2in9.h"
#include "epaper.h"
#include "epdpaint.h"
#include "status.h"

#define COLORED     0
#define UNCOLORED   1

Epd epd;
unsigned char image[480];
Paint paint(image, 0, 0);


void epaperDrawTemplate(bool available) {
    // clear memory
    epd.ClearFrameMemory(0xFF);

    // draw room name
    paint.SetRotate(ROTATE_90);
    paint.SetWidth(ROOM_NAME_HEIGHT);
    paint.SetHeight(WIDTH / 2);
    paint.Clear(COLORED);
    paint.DrawStringAt(PADDING, (ROOM_NAME_HEIGHT - ROOM_NAME_FONT.Height) / 2, ROOM_NAME, &ROOM_NAME_FONT, UNCOLORED);
    epd.SetFrameMemory(
        paint.GetImage(),
        HEIGHT - ROOM_NAME_HEIGHT,
        0,
        paint.GetWidth(),
        paint.GetHeight()
    );
    paint.Clear(COLORED);
    epd.SetFrameMemory(
        paint.GetImage(),
        HEIGHT - ROOM_NAME_HEIGHT,
        WIDTH / 2,
        paint.GetWidth(),
        paint.GetHeight()
    );

    if (available) {
        paint.SetWidth(STATUS_FONT.Height);
        paint.SetHeight(9 * STATUS_FONT.Width);
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 0, "AVAILABLE", &STATUS_FONT, COLORED);
        epd.SetFrameMemory(
            paint.GetImage(),
            (HEIGHT - ROOM_NAME_HEIGHT - STATUS_FONT.Height) / 2,
            PADDING,
            paint.GetWidth(),
            paint.GetHeight()
        );
    } else {
        paint.SetWidth(STATUS_FONT.Height);
        paint.SetHeight(STATUS_WIDTH);
        paint.Clear(COLORED);
        for (int y = 0; y < HEIGHT - ROOM_NAME_HEIGHT; y += STATUS_FONT.Height) {
            if (y + STATUS_FONT.Height > HEIGHT - ROOM_NAME_HEIGHT) {
                y = HEIGHT - ROOM_NAME_HEIGHT - STATUS_FONT.Height;
            }
            epd.SetFrameMemory(
                paint.GetImage(),
                y,
                0,
                paint.GetWidth(),
                paint.GetHeight()
            );
        }
        paint.DrawStringAt(PADDING, 0, "IN USE", &STATUS_FONT, UNCOLORED);
        epd.SetFrameMemory(
            paint.GetImage(),
            (HEIGHT - ROOM_NAME_HEIGHT - STATUS_FONT.Height) / 2,
            0,
            paint.GetWidth(),
            paint.GetHeight()
        );

        paint.SetWidth(DETAIL_LINE_HEIGHT);
        paint.SetHeight(2 * DETAIL_FONT.Width);
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, (DETAIL_LINE_HEIGHT - DETAIL_FONT.Height) / 2, "BY", &DETAIL_FONT, COLORED);
        epd.SetFrameMemory(
            paint.GetImage(),
            HEIGHT - ROOM_NAME_HEIGHT - DETAIL_PADDING - 3 * DETAIL_LINE_HEIGHT,
            STATUS_WIDTH + DETAIL_PADDING,
            paint.GetWidth(),
            paint.GetHeight()
        );
    }
}


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

    // if (epd.Init(lut_full_update) != 0) {
    //     Serial.print("e-Paper init failed");
    //     return;
    // }
    epd.Reset();

    if (status.event.available) {
        epaperDrawTemplate(true);
        epd.DisplayFrame();
    } else {
        epaperDrawTemplate(false);

        paint.SetRotate(ROTATE_90);
        paint.SetWidth(DETAIL_LINE_HEIGHT);
        paint.SetHeight(DETAIL_LINE_WIDTH);

        int x = HEIGHT - ROOM_NAME_HEIGHT - DETAIL_PADDING - DETAIL_LINE_HEIGHT;
        int y = STATUS_WIDTH + DETAIL_PADDING;
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 0, status.event.summary, &SUMMARY_FONT, COLORED);
        epd.SetFrameMemory(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());

        x -= DETAIL_LINE_HEIGHT;
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, (DETAIL_LINE_HEIGHT - DETAIL_FONT.Height) / 2, status.event.time, &DETAIL_FONT, COLORED);
        epd.SetFrameMemory(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());

        x -= DETAIL_LINE_HEIGHT;
        y += 3 * DETAIL_FONT.Width;
        paint.SetHeight(DETAIL_LINE_WIDTH - 3 * DETAIL_FONT.Width);
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, (DETAIL_LINE_HEIGHT - DETAIL_FONT.Height) / 2, status.event.creator, &DETAIL_FONT, COLORED);
        epd.SetFrameMemory(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());

        epd.DisplayFrame();
    }

    epd.Sleep();
}
