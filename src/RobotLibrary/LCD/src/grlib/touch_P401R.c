/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#include <stdbool.h>
#include <stdint.h>
#include <msp432.h>
#include "driverlib.h"
#include "touch_P401R.h"
#include "grlib.h"
#include "kitronix320x240x16_ssd2119_spi.h"

static touch_calibration touch_calibrationData;
static bool validateCalibration(void);
extern Graphics_Context g_sContext;

static void touch_delay(){
    uint32_t i = 0;
    uint32_t time = 480000;

    for(i = 0; i < time; i++)
    {
        ;
    }
}

void touch_initInterface(void)
{
    /* Initialize the ADC12_A Module. */

    MAP_ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_1,
                         ADC_DIVIDER_1, 0);
    MAP_ADC14_setResolution(ADC_14BIT);

    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
    MAP_ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_96, ADC_PULSE_WIDTH_96);
    MAP_ADC14_setSampleHoldTrigger(ADC_TRIGGER_ADCSC, false);

    MAP_ADC14_enableModule();

    /* Configure Y+ input to memory buffer 0. */
    MAP_ADC14_configureConversionMemory(TOUCH_Y_PLUS_MEMORY,
                                        ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                        TOUCH_Y_PLUS_INPUT,
                                        false);

    /* Configure X+ input to memory buffer 1. */
    MAP_ADC14_configureConversionMemory(TOUCH_X_PLUS_MEMORY,
                                        ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                        TOUCH_X_PLUS_INPUT,
                                        false);

    /* Configure SW1 for input. */
    MAP_GPIO_setAsInputPinWithPullUpResistor(TOUCH_SW1_PORT, TOUCH_SW1_PIN);

    /* Check if the screen has already been calibrated. */
    if(touch_calibrationData.key == TOUCH_CALIBRATION_KEY)
    {
        /* Return if the user is not manually requesting calibration. */
        if(MAP_GPIO_getInputPinValue(TOUCH_SW1_PORT, TOUCH_SW1_PIN))
        {
            return;
        }
    }

    /* Wait for SW1 to be released. */
    while(!MAP_GPIO_getInputPinValue(TOUCH_SW1_PORT, TOUCH_SW1_PIN))
    {
        ;
    }

    touch_calibration calData;

    calData.xMin = 3131;
    calData.xMax = 13094;
    calData.yMin = 4578;
    calData.yMax = 12805;

    calData.key = TOUCH_CALIBRATION_KEY;

    touch_calibrationData = calData;


    /* Calibrate the touch screen. */
//    do
//    {
//        touch_calibrate();
//    }
//    while(validateCalibration() == false);
}

/*
 * Returns true when a touch is detected.
 */
static bool touch_detectedTouch(void)
{
    uint32_t aDCtemp = 0;
    uint8_t i;
    uint_fast32_t status = 0;

    /* Set X- and Y- as output and Y+ as input (floating). */
    MAP_GPIO_setAsOutputPin(TOUCH_X_MINUS_PORT, TOUCH_X_MINUS_PIN);
    MAP_GPIO_setAsOutputPin(TOUCH_Y_MINUS_PORT, TOUCH_Y_MINUS_PIN);
    MAP_GPIO_setAsInputPin(TOUCH_Y_PLUS_PORT, TOUCH_Y_PLUS_PIN);

    /* Set X- high and Y- low to detect touch. */
    MAP_GPIO_setOutputHighOnPin(TOUCH_X_MINUS_PORT, TOUCH_X_MINUS_PIN);
    MAP_GPIO_setOutputLowOnPin(TOUCH_Y_MINUS_PORT, TOUCH_Y_MINUS_PIN);

    /* Sample the X+ ADC channel to check if there is currently a touch. */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(TOUCH_X_PLUS_PORT,
                                                    TOUCH_X_PLUS_PIN,
                                                    GPIO_TERTIARY_MODULE_FUNCTION);

    MAP_ADC14_clearInterruptFlag(TOUCH_X_PLUS_IFG | TOUCH_Y_PLUS_IFG);
    MAP_ADC14_disableConversion();
    MAP_ADC14_configureSingleSampleMode(TOUCH_X_PLUS_MEMORY, false);
    MAP_ADC14_enableConversion();

    for(i = 0; i < TOUCH_OVERSAMPLE; i++)
    {
        MAP_ADC14_toggleConversionTrigger();
        status = MAP_ADC14_getInterruptStatus();
        while(status != TOUCH_X_PLUS_IFG)
        {
            status = MAP_ADC14_getInterruptStatus();
        }
        aDCtemp += ADC14->MEM[1];
    }

    aDCtemp = (aDCtemp >> TOUCH_AVERAGE_DIVISOR);

    /* Check if the detected touch is below the threshold. */
    if(aDCtemp < TOUCH_THRESHOLD)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

static uint16_t touch_sampleX(void)
{
    uint32_t average = 0;
    uint8_t i;
    uint_fast32_t status = 0;

    /* Set X+ and X- as output and Y- as input (floating). */
    MAP_GPIO_setAsOutputPin(TOUCH_X_PLUS_PORT, TOUCH_X_PLUS_PIN);
    MAP_GPIO_setAsOutputPin(TOUCH_X_MINUS_PORT, TOUCH_X_MINUS_PIN);
    MAP_GPIO_setAsInputPin(TOUCH_Y_MINUS_PORT, TOUCH_Y_MINUS_PIN);

    /* Set X+ high and X- low. */
    MAP_GPIO_setOutputHighOnPin(TOUCH_X_PLUS_PORT, TOUCH_X_PLUS_PIN);
    MAP_GPIO_setOutputLowOnPin(TOUCH_X_MINUS_PORT, TOUCH_X_MINUS_PIN);

    /* Sample the Y+ ADC channel. */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(TOUCH_Y_PLUS_PORT,
                                                    TOUCH_Y_PLUS_PIN,
                                                    GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_ADC14_clearInterruptFlag(TOUCH_X_PLUS_IFG | TOUCH_Y_PLUS_IFG);
    MAP_ADC14_disableConversion();
    MAP_ADC14_configureSingleSampleMode(TOUCH_Y_PLUS_MEMORY, false);
    MAP_ADC14_enableConversion();

    for(i = 0; i < TOUCH_OVERSAMPLE; i++)
    {
        MAP_ADC14_toggleConversionTrigger();
        status = MAP_ADC14_getInterruptStatus();
        while(status != TOUCH_Y_PLUS_IFG)
        {
            status = MAP_ADC14_getInterruptStatus();
        }
        average += ADC14->MEM[0];
    }

    /* Return the analog result. */
    return(average >> TOUCH_AVERAGE_DIVISOR);
}

/*
 * Sample the Y analog axis.
 */
static uint16_t touch_sampleY(void)
{
    uint32_t average = 0;
    uint8_t i;
    uint_fast32_t status = 0;

    /* Set Y+ and Y- as output and X- as input (floating). */
    MAP_GPIO_setAsOutputPin(TOUCH_Y_PLUS_PORT, TOUCH_Y_PLUS_PIN);
    MAP_GPIO_setAsOutputPin(TOUCH_Y_MINUS_PORT, TOUCH_Y_MINUS_PIN);
    MAP_GPIO_setAsInputPin(TOUCH_X_MINUS_PORT, TOUCH_X_MINUS_PIN);

    /* Set Y+ low and Y- high. */
    MAP_GPIO_setOutputLowOnPin(TOUCH_Y_PLUS_PORT, TOUCH_Y_PLUS_PIN);
    MAP_GPIO_setOutputHighOnPin(TOUCH_Y_MINUS_PORT, TOUCH_Y_MINUS_PIN);

    /* Sample the X+ ADC channel. */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(TOUCH_X_PLUS_PORT,
                                                    TOUCH_X_PLUS_PIN,
                                                    GPIO_TERTIARY_MODULE_FUNCTION);

    MAP_ADC14_clearInterruptFlag(TOUCH_X_PLUS_IFG | TOUCH_Y_PLUS_IFG);
    MAP_ADC14_disableConversion();
    MAP_ADC14_configureSingleSampleMode(TOUCH_X_PLUS_MEMORY, false);
    MAP_ADC14_enableConversion();

    for(i = 0; i < TOUCH_OVERSAMPLE; i++)
    {
        MAP_ADC14_toggleConversionTrigger();
        status = MAP_ADC14_getInterruptStatus();
        while(status != TOUCH_X_PLUS_IFG)
        {
            status = MAP_ADC14_getInterruptStatus();
        }
        average += ADC14->MEM[1];
    }

    /* Return the anlog result. */
    return(average >> TOUCH_AVERAGE_DIVISOR);
}

void touch_updateCurrentTouch(touch_context *data)
{
    uint16_t adcResult;
    float q12Ratio;

    /* Sample new X and Y coordinates if touch was detected. */
    if(touch_detectedTouch())
    {
        data->touch = true;

        // Debounce touchscreen
        touch_delay();
        /* Sample the X analog axis. */
        adcResult = touch_sampleX();

        /* Map the analog reading to the display coordinates. */
        /* Map the ADC reading to the display coordinates. */
        q12Ratio =
            (float)(((float)(adcResult -
                             touch_calibrationData.xMin)) /
                    (float)(touch_calibrationData.xMax -
                            touch_calibrationData.xMin));
        if(q12Ratio >= 1)
        {
            q12Ratio = 1;
        }
        if(q12Ratio < 0)
        {
            q12Ratio = 0;
        }
        data->x = (uint16_t)(q12Ratio * LCD_HORIZONTAL_MAX);

        /* Sample the Y analog axis. */
        adcResult = touch_sampleY();

        /* Map the ADC reading to the display coordinates. */
        q12Ratio =
            (float)(((float)(adcResult -
                             touch_calibrationData.yMin)) /
                    ((float)(touch_calibrationData.yMax -
                             touch_calibrationData.yMin)));
        if(q12Ratio >= 1)
        {
            q12Ratio = 1;
        }
        if(q12Ratio < 0)
        {
            q12Ratio = 0;
        }

        data->y = (uint16_t)(q12Ratio * LCD_VERTICAL_MAX);
    }
    else
    {
        data->touch = false;
        data->x = 0xffff;
        data->y = 0xffff;
    }
}

static void touch_calibrateCircle(uint16_t *xSum,
                                  uint16_t *ySum,
                                  uint16_t xPos,
                                  uint16_t yPos)
{
    /* Draw a red circle for the user to touch. */
    Graphics_setForegroundColor(&g_sContext, ClrRed);
    Graphics_fillCircle(&g_sContext, xPos, yPos, TOUCH_CALIBRATION_RADIUS);

    /* Wait for a tocuh to be detected and wait ~4ms. */
    while(!touch_detectedTouch())
    {
        ;
    }
    touch_delay();

    /* Sample the X and Y measurements of the touch screen. */
    *xSum += touch_sampleX();
    *ySum += touch_sampleY();

    /* Wait for the touch to stop and wait ~4ms. */
    while(touch_detectedTouch())
    {
        ;
    }
    touch_delay();

    /* Clear the drawn circle. */
    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_fillCircle(&g_sContext, xPos, yPos, TOUCH_CALIBRATION_RADIUS);

    return;
}

void touch_calibrate(void)
{
    touch_calibration calData;

    /* Zero out local copies of calibration data. */
    calData.xMin = 0;
    calData.xMax = 0;
    calData.yMin = 0;
    calData.yMax = 0;

    /* Display Instructions to the screen. */

    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_drawStringCentered(&g_sContext,
                                "TOUCH DOT TO CALIBRATE",
                                AUTO_STRING_LENGTH,
                                LCD_HORIZONTAL_MAX / 2,
                                LCD_VERTICAL_MAX / 2,
                                TRANSPARENT_TEXT);

    /* Top left corner. */
    touch_calibrateCircle(
        &calData.xMin,
        &calData.yMin,
        TOUCH_CALIBRATION_RADIUS,
        TOUCH_CALIBRATION_RADIUS);

    /* Top right corner. */
    touch_calibrateCircle(
        &calData.xMax,
        &calData.yMin,
        (LCD_HORIZONTAL_MAX - 1) - TOUCH_CALIBRATION_RADIUS,
        TOUCH_CALIBRATION_RADIUS);

    /* Bottom left corner. */
    touch_calibrateCircle(
        &calData.xMin,
        &calData.yMax,
        TOUCH_CALIBRATION_RADIUS,
        (LCD_VERTICAL_MAX - 1) - TOUCH_CALIBRATION_RADIUS);

    /* Bottom right corner. */
    touch_calibrateCircle(
        &calData.xMax,
        &calData.yMax,
        (LCD_HORIZONTAL_MAX - 1) - TOUCH_CALIBRATION_RADIUS,
        (LCD_VERTICAL_MAX - 1) - TOUCH_CALIBRATION_RADIUS);

    /* Compensate for the radius offset to caluculate final X calibration values. */
    calData.xMin = (calData.xMin >> 1) - TOUCH_CALIBRATION_RADIUS;
    calData.xMax = (calData.xMax >> 1) + TOUCH_CALIBRATION_RADIUS;

    /* Compensate for the radius offset to caluculate final Y calibration values. */
    calData.yMin = (calData.yMin >> 1) - TOUCH_CALIBRATION_RADIUS;
    calData.yMax = (calData.yMax >> 1) + TOUCH_CALIBRATION_RADIUS;

    /* Write the calibration key to signal the values have been saved. */
    calData.key = TOUCH_CALIBRATION_KEY;

    touch_calibrationData = calData;

    Graphics_clearDisplay(&g_sContext);
}

static bool validateCalibration(void){
    uint16_t i = 0;
    if((touch_calibrationData.xMin < 2800) &&
       (touch_calibrationData.xMin > 2400) &&
       (touch_calibrationData.yMin < 4200) &&
       (touch_calibrationData.yMin > 3200))
    {
        return(true);
    }
    Graphics_clearDisplay(&g_sContext);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);

    Graphics_drawStringCentered(&g_sContext,
                                "INVALID CALIBRATION",
                                AUTO_STRING_LENGTH,
                                LCD_HORIZONTAL_MAX / 2,
                                LCD_VERTICAL_MAX / 2,
                                TRANSPARENT_TEXT);
    for(i = 0; i < 10; i++)
    {
        touch_delay();
        i++;
    }
    Graphics_clearDisplay(&g_sContext);

    return(false);
}
