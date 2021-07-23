#include "gridDisplay.h"

Graphics_Context g_sContext;

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

void getLineLength()
{
}

void gridDisplay_drawMarker(coord pos){
    uint32_t adjustedX = (pos.x * (Grid_getBlockSize()/5)) + 19;
    uint32_t adjustedY = 220 - (pos.y * 2);
    Graphics_drawPixel(&g_sContext, adjustedX, adjustedY);
    Graphics_drawPixel(&g_sContext, adjustedX, adjustedY - 1);
    Graphics_drawPixel(&g_sContext, adjustedX + 1, adjustedY);
    Graphics_drawPixel(&g_sContext, adjustedX + 1, adjustedY - 1);
}

void gridDisplay_addToTotalDistance(uint32_t addition){
    prevDistance = totalDistance;
    totalDistance += addition;
}
