#include "gridDisplay.h"

Graphics_Context g_sContext;
touch_context g_sTouchContext;

uint32_t totalDistance = 0;
uint32_t timeRunning = 0;
uint32_t prevDistance = 0;
uint32_t prevTime = 0;
uint32_t prevAngle;
coord prevCoords = { .x = 0, .y = 0 };

void gridDisplay_init()
{
    Kitronix320x240x16_SSD2119Init();
    Graphics_initContext(&g_sContext, &g_sKitronix320x240x16_SSD2119);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    Graphics_clearDisplay(&g_sContext);

    touch_initInterface();
}

uint32_t gridDisplay_blockSizeSelection() {
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    //Box 1: 5 cm
    Graphics_drawLine(&g_sContext, 53, 79, 53, 160);
    Graphics_drawLine(&g_sContext, 53, 160, 134, 160);
    Graphics_drawLine(&g_sContext, 134, 160, 134, 79);
    Graphics_drawLine(&g_sContext, 134, 79, 53, 79);

    //Box 2: 10 cm
    Graphics_drawLine(&g_sContext, 187, 79, 187, 160);
    Graphics_drawLine(&g_sContext, 187, 160, 268, 160);
    Graphics_drawLine(&g_sContext, 268, 160, 268, 79);
    Graphics_drawLine(&g_sContext, 268, 79, 187, 79);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_drawStringCentered(&g_sContext, "5 cm", AUTO_STRING_LENGTH, 93, 115, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "10 cm", AUTO_STRING_LENGTH, 227, 115, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Choose size of each pixel", AUTO_STRING_LENGTH, 160, 35, TRANSPARENT_TEXT);

    while (true) {
        touch_updateCurrentTouch(&g_sTouchContext);

        if(g_sTouchContext.touch) {
            if (g_sTouchContext.x > 53 && g_sTouchContext.x  < 134 && g_sTouchContext.y > 79 && g_sTouchContext.y < 160) {
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
                Graphics_drawStringCentered(&g_sContext, "Choose size of each pixel", AUTO_STRING_LENGTH, 160, 35, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "5 cm", AUTO_STRING_LENGTH, 93, 115, TRANSPARENT_TEXT);
                Graphics_drawLine(&g_sContext, 53, 79, 53, 160);
                Graphics_drawLine(&g_sContext, 53, 160, 134, 160);
                Graphics_drawLine(&g_sContext, 134, 160, 134, 79);
                Graphics_drawLine(&g_sContext, 134, 79, 53, 79);

                Graphics_drawStringCentered(&g_sContext, "10 cm", AUTO_STRING_LENGTH, 227, 115, TRANSPARENT_TEXT);
                Graphics_drawLine(&g_sContext, 187, 79, 187, 160);
                Graphics_drawLine(&g_sContext, 187, 160, 268, 160);
                Graphics_drawLine(&g_sContext, 268, 160, 268, 79);
                Graphics_drawLine(&g_sContext, 268, 79, 187, 79);
                return 5;
            }
            else if (g_sTouchContext.x > 187 && g_sTouchContext.x  < 268 && g_sTouchContext.y > 79 && g_sTouchContext.y < 160) {
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
                Graphics_drawStringCentered(&g_sContext, "Choose size of each pixel", AUTO_STRING_LENGTH, 160, 35, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "5 cm", AUTO_STRING_LENGTH, 93, 115, TRANSPARENT_TEXT);
                Graphics_drawLine(&g_sContext, 53, 79, 53, 160);
                Graphics_drawLine(&g_sContext, 53, 160, 134, 160);
                Graphics_drawLine(&g_sContext, 134, 160, 134, 79);
                Graphics_drawLine(&g_sContext, 134, 79, 53, 79);

                Graphics_drawStringCentered(&g_sContext, "10 cm", AUTO_STRING_LENGTH, 227, 115, TRANSPARENT_TEXT);
                Graphics_drawLine(&g_sContext, 187, 79, 187, 160);
                Graphics_drawLine(&g_sContext, 187, 160, 268, 160);
                Graphics_drawLine(&g_sContext, 268, 160, 268, 79);
                Graphics_drawLine(&g_sContext, 268, 79, 187, 79);
                return 10;
            }
        }
    }
}

coord gridDisplay_chooseGridLocation() {
    coord prevTouch;
    coord curTouch;
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawLine(&g_sContext, 19, 19, 19, 220);
    Graphics_drawLine(&g_sContext, 19, 220, 220, 220);
    Graphics_drawLine(&g_sContext, 220, 220, 220, 19);
    Graphics_drawLine(&g_sContext, 220, 19, 19, 19);

    Graphics_drawLine(&g_sContext, 229, 79, 229, 160);
    Graphics_drawLine(&g_sContext, 229, 160, 310, 160);
    Graphics_drawLine(&g_sContext, 310, 160, 310, 79);
    Graphics_drawLine(&g_sContext, 310, 79, 229, 79);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_drawStringCentered(&g_sContext, "Submit", AUTO_STRING_LENGTH, 270, 115, TRANSPARENT_TEXT);

    while (true) {
        touch_updateCurrentTouch(&g_sTouchContext);

        if(g_sTouchContext.touch) {
            curTouch.x = g_sTouchContext.x;
            curTouch.y = g_sTouchContext.y;

            if (curTouch.x > 229 && curTouch.x  < 310 && curTouch.y > 79 && curTouch.y < 160) {
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
                Graphics_drawPixel(&g_sContext, prevTouch.x, prevTouch.y);
                Graphics_drawPixel(&g_sContext, prevTouch.x, prevTouch.y - 1);
                Graphics_drawPixel(&g_sContext, prevTouch.x + 1, prevTouch.y);
                Graphics_drawPixel(&g_sContext, prevTouch.x + 1, prevTouch.y - 1);
                Graphics_drawLine(&g_sContext, 19, 19, 19, 220);
                Graphics_drawLine(&g_sContext, 19, 220, 220, 220);
                Graphics_drawLine(&g_sContext, 220, 220, 220, 19);
                Graphics_drawLine(&g_sContext, 220, 19, 19, 19);

                Graphics_drawStringCentered(&g_sContext, "Submit", AUTO_STRING_LENGTH, 270, 115, TRANSPARENT_TEXT);
                Graphics_drawLine(&g_sContext, 229, 79, 229, 160);
                Graphics_drawLine(&g_sContext, 229, 160, 310, 160);
                Graphics_drawLine(&g_sContext, 310, 160, 310, 79);
                Graphics_drawLine(&g_sContext, 310, 79, 229, 79);

                coord retVal;
                retVal.x = prevTouch.x - 19;
                retVal.y = 220 - prevTouch.y;
                return retVal;
            }

            if (curTouch.x > 19 && curTouch.x  < 220 && curTouch.y > 19 && curTouch.y < 220) {
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
                Graphics_drawPixel(&g_sContext, prevTouch.x, prevTouch.y);
                Graphics_drawPixel(&g_sContext, prevTouch.x, prevTouch.y - 1);
                Graphics_drawPixel(&g_sContext, prevTouch.x + 1, prevTouch.y);
                Graphics_drawPixel(&g_sContext, prevTouch.x + 1, prevTouch.y - 1);

                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
                Graphics_drawPixel(&g_sContext, curTouch.x, curTouch.y);
                Graphics_drawPixel(&g_sContext, curTouch.x, curTouch.y - 1);
                Graphics_drawPixel(&g_sContext, curTouch.x + 1, curTouch.y);
                Graphics_drawPixel(&g_sContext, curTouch.x + 1, curTouch.y - 1);

                prevTouch = curTouch;
            }
        }
    }
}

void gridDisplay_createMainDisplay() {
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawLine(&g_sContext, 19, 19, 19, 220);
    Graphics_drawLine(&g_sContext, 19, 220, 220, 220);
    Graphics_drawLine(&g_sContext, 220, 220, 220, 19);
    Graphics_drawLine(&g_sContext, 220, 19, 19, 19);

    Graphics_drawStringCentered(&g_sContext, "Dist", AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 24, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Coords", AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 74, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Angle", AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 124, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Size", AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 174, TRANSPARENT_TEXT);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    char* buffer[10];
    sprintf(buffer, "%i cm", Grid_getBlockSize());
    Graphics_drawStringCentered(&g_sContext, buffer, AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 194, TRANSPARENT_TEXT);
}


void gridDisplay_updateInfo()
{
    char* buffer[10];

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    sprintf(buffer, "%i.%i cm", prevDistance/10000, (prevDistance/1000) % 10);
    Graphics_drawStringCentered(&g_sContext, buffer, AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 44, TRANSPARENT_TEXT);

    sprintf(buffer, "(%i,%i)", prevCoords.x / (Grid_getBlockSize() * 10000), prevCoords.y / (Grid_getBlockSize() * 10000));
    Graphics_drawStringCentered(&g_sContext, buffer, AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 94, TRANSPARENT_TEXT);

    sprintf(buffer, "%i", prevAngle);
    Graphics_drawStringCentered(&g_sContext, buffer, AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 144, TRANSPARENT_TEXT);


    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    sprintf(buffer, "%i.%i cm", totalDistance/10000, (totalDistance/1000) % 10);
    Graphics_drawStringCentered(&g_sContext, buffer, AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 44, TRANSPARENT_TEXT);

    robotPos curPos = Grid_getRobotPos();
    coord curCoords = curPos.coordinates;
    sprintf(buffer, "(%i,%i)", curCoords.x / (Grid_getBlockSize() * 10000), curCoords.y / (Grid_getBlockSize() * 10000));
    Graphics_drawStringCentered(&g_sContext, buffer, AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 94, TRANSPARENT_TEXT);

    uint32_t angle = curPos.direction;
    sprintf(buffer, "%i", angle);
    Graphics_drawStringCentered(&g_sContext, buffer, AUTO_STRING_LENGTH, TEXT_CENTER_POSITION, 144, TRANSPARENT_TEXT);


    prevDistance = totalDistance;
    prevTime = timeRunning;
    prevCoords = curCoords;
    prevAngle = angle;
}

bool gridDisplay_isTouched()
{
    touch_updateCurrentTouch(&g_sTouchContext);
    return g_sTouchContext.touch;
}

void gridDisplay_drawMarker(coord pos){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    int32_t adjustedX = (pos.x + 19);
    int32_t adjustedY = 220 - (pos.y);
    Graphics_drawPixel(&g_sContext, adjustedX, adjustedY);
    Graphics_drawPixel(&g_sContext, adjustedX, adjustedY - 1);
    Graphics_drawPixel(&g_sContext, adjustedX + 1, adjustedY);
    Graphics_drawPixel(&g_sContext, adjustedX + 1, adjustedY - 1);
}

void gridDisplay_addToTotalDistance(uint32_t addition){
    prevDistance = totalDistance;
    totalDistance += addition;
}
