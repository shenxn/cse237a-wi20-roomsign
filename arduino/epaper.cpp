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

    // epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
    // epd.DisplayFrame();
    // epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
    // epd.DisplayFrame();

    /** 
     *  there are 2 memory areas embedded in the e-paper display
     *  and once the display is refreshed, the memory area will be auto-toggled,
     *  i.e. the next action of SetFrameMemory will set the other memory area
     *  therefore you have to clear the frame memory twice.
     */
    epd.SetFrameMemory(IMAGE_DATA);
    epd.DisplayFrame();
    epd.SetFrameMemory(IMAGE_DATA);
    epd.DisplayFrame();

    epd.Sleep();
}
