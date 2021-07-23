#include "BSP.h"
#include "grid.h"
#include "grlib.h"
#include "kitronix320x240x16_ssd2119_spi.h"

#define TEXT_CENTER_POSITION 270

void gridDisplay_init();
void gridDisplay_updateInfo();
void gridDisplay_drawMarker(coord pos);
void gridDisplay_addToTotalDistance(uint32_t addition);

void getLineLength();
