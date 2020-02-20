/* Includes ------------------------------------------------------------------*/
#include "fonts.h"
#include <avr/pgmspace.h>

const uint8_t Font${fontSize}_Table[] PROGMEM = 
{
	${fontData}
};

sFONT Font${fontSize} = {
  Font${fontSize}_Table,
  ${width}, /* Width */
  ${fontSize}, /* Height */
};