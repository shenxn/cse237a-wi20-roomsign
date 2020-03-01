#ifndef EPAPER_H
#define EPAPER_H

#include "fonts.h"
#include "epd4in2b.h"

#define COLORED 0
#define UNCOLORED 1

#define WIDTH EPD_WIDTH
#define HEIGHT EPD_HEIGHT
#define PADDING 24
#define ROOM_NAME "ROOM B210"
#define ROOM_NAME_HEIGHT 40
#define ROOM_NAME_FONT Font24
#define SUMMARY_FONT Font32
#define DETAIL_HEIGHT 180
#define DETAIL_LINE_HEIGHT 48
#define DETAIL_FONT Font24
#define STATUS_HEIGHT 80
#define STATUS_FONT Font32

void epaperSetup();

void epaperDisplay();

#endif
