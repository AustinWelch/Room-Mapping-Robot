/*
 * BSP.c
 *
 *  Created on: Dec 30, 2016
 *      Author: Raz Aloni
 */

#include <driverlib.h>
#include "BSP.h"
#include "i2c_driver.h"


/* Initializes the entire board */
void BSP_InitBoard()
{
	/* Disable Watchdog */
	WDT_A_clearTimer();
	WDT_A_holdTimer();

	/* Initialize Clock */
	/* 2 flash wait states, VCORE = 1, running off DC-DC, 48 MHz */
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
    PCM_setPowerState(PCM_AM_DCDC_VCORE1);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_setDCOFrequency(48000000);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, 1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, 1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, 1);

	/* Init i2c */
	//initI2C();

	/* Init BackChannel UART */
	BackChannelInit();

}


