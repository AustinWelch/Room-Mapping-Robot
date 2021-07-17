#include "OPT3101.h"
#include "BSP.h"
#include <stdio.h>
#include "grlib.h"
#include "kitronix320x240x16_ssd2119_spi.h"
#include "BSP.h"
#include "driverlib.h"
#include <fpu.h>
#include <stdint.h>

// Graphic library context
Graphics_Context g_sContext;

char* output[100];
uint32_t prevDistances[] = {0, 0, 0};
void displayDistance(uint8_t channel);


int main(){
    BSP_InitBoard();
    FPU_enableModule();
    __enable_interrupt();

    Kitronix320x240x16_SSD2119Init();
    Graphics_initContext(&g_sContext, &g_sKitronix320x240x16_SSD2119);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    Graphics_clearDisplay(&g_sContext);

    OPT3101_init();

    uint8_t channel = 0;

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    sprintf(output, "Right");
    Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 79, 100, TRANSPARENT_TEXT);
    sprintf(output, "Center");
    Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 159, 70, TRANSPARENT_TEXT);
    sprintf(output, "Left");
    Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 239, 100, TRANSPARENT_TEXT);

    while(1){
        OPT3101_takeMeasurement(channel);

        while(!OPT3101_readyToSample());

        OPT3101_readOutputRegs();
        displayDistance(channel);
        channel = (channel + 1) % 3;
        //DelayMs(50);
    }
}

void displayDistance(uint8_t channel){

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    if (channel == 0){
        sprintf(output, "%i.%icm", prevDistances[0]/10, prevDistances[0] % 10);
        Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 239, 120, TRANSPARENT_TEXT);
    } else if (channel == 1) {
        sprintf(output, "%i.%icm", prevDistances[1]/10, prevDistances[1] % 10);
        Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 159, 90, TRANSPARENT_TEXT);
    } else {
        sprintf(output, "%i.%icm", prevDistances[2]/10, prevDistances[2] % 10);
        Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 79, 120, TRANSPARENT_TEXT);
    }
    
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    if (channel == 0){
        sprintf(output, "%i.%icm", distances[0]/10, distances[0] % 10);
        Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 239, 120, TRANSPARENT_TEXT);
        prevDistances[0] = distances[0];
    } else if (channel == 1) {
        sprintf(output, "%i.%icm", distances[1]/10, distances[1] % 10);
        Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 159, 90, TRANSPARENT_TEXT);
        prevDistances[1] = distances[1];
    } else {
        sprintf(output, "%i.%icm", distances[2]/10, distances[2] % 10);
        Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 79, 120, TRANSPARENT_TEXT);
        prevDistances[2] = distances[2];
    }
}
