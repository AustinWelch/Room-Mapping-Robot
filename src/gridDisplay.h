#include "BSP.h"
#include "grid.h"
#include "grlib.h"
#include "kitronix320x240x16_ssd2119_spi.h"
#include "touch_P401R.h"

#define TEXT_CENTER_POSITION 270

void gridDisplay_init();
uint32_t gridDisplay_blockSizeSelection();
coord gridDisplay_chooseGridLocation();
void gridDisplay_createMainDisplay();
void gridDisplay_updateInfo();
void gridDisplay_drawMarker(coord pos);
bool gridDisplay_isTouched();
void gridDisplay_addToTotalDistance(uint32_t addition);

void getLineLength();
