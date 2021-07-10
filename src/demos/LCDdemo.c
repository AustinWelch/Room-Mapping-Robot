#include "grlib.h"
#include "kitronix320x240x16_ssd2119_spi.h"
#include "BSP.h"
#include "driverlib.h"
#include <fpu.h>
#include <stdint.h>
#include "MotorControl.h"

// Graphic library context
Graphics_Context g_sContext;

char* output[100];
uint32_t distance = 0;

void displayDistance(void);
int main(){

    BSP_InitBoard();
    FPU_enableModule();
    Motor_initialize();
    __enable_interrupt();

    Kitronix320x240x16_SSD2119Init();
    Graphics_initContext(&g_sContext, &g_sKitronix320x240x16_SSD2119);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    Graphics_clearDisplay(&g_sContext);

    displayDistance();

    while(1){

        Odometer_start();

        Motor_moveForwardDistance(50, 10);
        displayDistance();
        DelayMs(500);

        Motor_moveForwardDistance(50, 20);
        displayDistance();
        DelayMs(500);

        Motor_moveForwardDistance(50, 30);
        displayDistance();
        DelayMs(500);

        Motor_moveForwardDistance(50, 40);
        displayDistance();
        DelayMs(500);

        Motor_moveForwardDistance(50, 50);
        displayDistance();
        DelayMs(500);

        Motor_moveForwardDistance(50, 60);
       displayDistance();
       DelayMs(500);

       Motor_moveForwardDistance(50, 70);
       displayDistance();
       DelayMs(500);

       Motor_moveForwardDistance(50, 80);
       displayDistance();
       DelayMs(500);

       Motor_moveForwardDistance(50, 90);
       displayDistance();
       DelayMs(500);

       Motor_moveForwardDistance(50, 100);
       displayDistance();
       DelayMs(500);

    }
}

void displayDistance(){
    sprintf(output, "Distance Traveled: %i.%i", distance/10000, (distance/1000) % 10);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 159, 15, TRANSPARENT_TEXT);

    distance = Odometer_value();
    sprintf(output, "Distance Traveled: %i.%i", distance/10000, (distance/1000) % 10);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 159, 15, TRANSPARENT_TEXT);
    Odometer_clear();
}
