#include "grlib.h"
#include "BSP.h"
#include <fpu.h>

// Graphic library context
Graphics_Context g_sContext;

int main(){

    BSP_InitBoard();
    FPU_enableModule();

    int16_t ulIdx;
    Graphics_Rectangle myRectangle1 = { 10, 50, 155, 120};
    Graphics_Rectangle myRectangle2 = { 150, 100, 300, 200};
    Graphics_Rectangle myRectangle3 = { 0, 0, 319, 239};

    Graphics_initContext(&g_sContext, &g_sKitronix320x240x16_SSD2119);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    Graphics_clearDisplay(&g_sContext);

    Graphics_clearDisplay(&g_sContext);
    Graphics_drawPixel(&g_sContext, 45, 45);
    Graphics_drawPixel(&g_sContext, 45, 50);
    Graphics_drawPixel(&g_sContext, 50, 50);
    Graphics_drawPixel(&g_sContext, 50, 45);
    Graphics_drawLine(&g_sContext, 60, 60, 200, 200);
    Graphics_drawLine(&g_sContext, 30, 200, 200, 60);
    Graphics_drawLine(&g_sContext, 0, Graphics_getDisplayHeight(
                          &g_sContext) - 1,
                      Graphics_getDisplayWidth(&g_sContext) - 1,
                      Graphics_getDisplayHeight(&g_sContext) - 1);
}
