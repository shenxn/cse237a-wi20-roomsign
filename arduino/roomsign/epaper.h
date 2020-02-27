#ifndef EPAPER_H
#define EPAPER_H

#include "fonts.h"

#define COLORED 0
#define UNCOLORED 1

#define WIDTH 296
#define HEIGHT 128
#define PADDING 16
#define ROOM_NAME "ROOM B210"
#define ROOM_NAME_HEIGHT 24
#define ROOM_NAME_FONT Font20
#define STATUS_WIDTH 104
#define STATUS_FONT Font24
#define SUMMARY_FONT Font20
#define DETAIL_PADDING 16
#define DETAIL_LINE_HEIGHT 24
#define DETAIL_LINE_WIDTH 160
#define DETAIL_FONT Font16

void epaperSetup();

void epaperDisplay();

#endif
