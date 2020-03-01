#include <SPI.h>
#include "epd4in2b.h"
#include "epdpaint.h"
#include "epaper.h"
#include "status.h"
#include "macro.h"

#define COLORED     0
#define UNCOLORED   1

Epd epd;
unsigned char image[512];
Paint paint(image, 0, 0);


void fillArea(int x, int y, int w, int l, bool is_red=false) {
    epd.SendCommand(PARTIAL_IN);
    epd.SendCommand(PARTIAL_WINDOW);
    epd.SendData(x >> 8);
    epd.SendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    epd.SendData(((x & 0xfff8) + w  - 1) >> 8);
    epd.SendData(((x & 0xf8) + w  - 1) | 0x07);
    epd.SendData(y >> 8);        
    epd.SendData(y & 0xff);
    epd.SendData((y + l - 1) >> 8);        
    epd.SendData((y + l - 1) & 0xff);
    epd.SendData(0x01);         // Gates scan both inside and outside of the partial window. (default) 
    EpdIf::DelayMs(2);
    epd.SendCommand(is_red ? DATA_START_TRANSMISSION_2 : DATA_START_TRANSMISSION_1);
    for(int i = 0; i < w  / 8 * l; i++) {
        epd.SendData(0x00);  
    }
    EpdIf::DelayMs(2);
    epd.SendCommand(PARTIAL_OUT);
}


void drawString(int x, int y, const char *str, sFONT *font, int colored, bool isRed=false) {
    int len = strlen(str);
    for (int i = 0; i < len; i += 8) {
        paint.SetWidth(font->Width * min(8, len - i));
        paint.SetHeight(font->Height);
        paint.Clear(!colored);
        for (int j = 0; j < 8; ++j) {
            if (i + j == len) {
                break;
            }
            paint.DrawCharAt(j * font->Width, 0, str[i + j], font, colored);
        }
        unsigned char *bufferBlack = NULL;
        unsigned char *bufferRed = NULL;
        if (isRed) {
            bufferRed = paint.GetImage();
        } else {
            bufferBlack = paint.GetImage();
        }
        epd.SetPartialWindow(
            bufferBlack,
            bufferRed,
            x + i * font->Width,
            y,
            paint.GetWidth(),
            paint.GetHeight()
        );
    }
}


void epaperDrawTemplate(bool available) {
    // clear memory
    epd.ClearFrame();

    // draw room name
    fillArea(0, 0, WIDTH, ROOM_NAME_HEIGHT);
    drawString(
        PADDING, 
        (ROOM_NAME_HEIGHT - ROOM_NAME_FONT.Height) / 2,
        ROOM_NAME,
        &ROOM_NAME_FONT,
        UNCOLORED
    );

    if (available) {
        drawString(
            PADDING, 
            ROOM_NAME_HEIGHT + (HEIGHT - ROOM_NAME_HEIGHT - STATUS_FONT.Height) / 2,
            "AVAILABLE",
            &STATUS_FONT,
            COLORED
        );
    } else {
        fillArea(0, ROOM_NAME_HEIGHT + DETAIL_HEIGHT, WIDTH, STATUS_HEIGHT, true);
        drawString(
            PADDING, 
            ROOM_NAME_HEIGHT + DETAIL_HEIGHT + (STATUS_HEIGHT - STATUS_FONT.Height) / 2,
            "IN USE",
            &STATUS_FONT,
            UNCOLORED,
            true
        );

        drawString(
            PADDING,
            ROOM_NAME_HEIGHT + PADDING + 2 * DETAIL_LINE_HEIGHT,
            "BY",
            &DETAIL_FONT,
            COLORED
        );
    }
}


void epaperSetup() {
    if (epd.Init() != 0) {
        SERIAL_PRINT(F("e-Paper init failed"));
        return;
    }

    epd.ClearFrame();
    epd.DisplayFrame();

    epd.Sleep();
}


void epaperDisplay() {
    if (!status.updated) {
        return;
    }
    status.updated = false;

    SERIAL_PRINT(F("display available: "));
    SERIAL_PRINTLN((int)status.event.available);

    if (epd.Init() != 0) {
        SERIAL_PRINT(F("e-Paper init failed"));
        return;
    }

    if (status.event.available) {
        epaperDrawTemplate(true);
        epd.DisplayFrame();
    } else {
        epaperDrawTemplate(false);

        int x = PADDING;
        int y = ROOM_NAME_HEIGHT + PADDING;
        drawString(
            x,
            y,
            status.event.summary,
            &SUMMARY_FONT,
            COLORED
        );

        y += DETAIL_LINE_HEIGHT;
        drawString(x, y, status.event.time, &DETAIL_FONT, COLORED);

        x += 3 * DETAIL_FONT.Width;
        y += DETAIL_LINE_HEIGHT;
        drawString(x, y, status.event.creator, &DETAIL_FONT, COLORED);

        epd.DisplayFrame();
    }

    epd.Sleep();
}
