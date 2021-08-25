/*
*   Some functions based on example library from https://github.com/pololu/opt3101-arduino
*/

#include "OPT3101.h"
#include <i2c.h>
#include <stdio.h>
#include <time.h>
#include "I2CB1.h"

static const uint32_t reg80Default = 0x4e1e;
uint8_t channelUsed = 0;
bool timingGeneratorEnabled = false;
uint8_t address = 0x58;
bool readyToSample = true;

void OPT3101_writeReg(uint8_t reg, uint32_t value)
{
    uint8_t buffer[] = {
        reg,
        value & 0xFF,
        value >> 8 & 0xFF,
        value >> 16 & 0xFF
      };

      I2CB1_Send(address, buffer, sizeof(buffer));
}

uint32_t OPT3101_readReg(uint8_t reg)
{
    uint8_t buffer[3];
      I2CB1_Send(address, &reg, 1);
      I2CB1_Recv(address, buffer, 3);
      return buffer[0] + ((uint32_t)buffer[1] << 8) + ((uint32_t)buffer[2] << 16);
}

void OPT3101_init()
{
    P6->DIR |= BIT4 | BIT5;
    P6->OUT |= BIT4 | BIT5;
    P6->SEL1 &= ~(BIT4 | BIT5);
    P6->SEL0 |= BIT4 | BIT5;

    I2CB1_Init(30); //12Mhz / 30 = 400kHz

    OPT3101_reset();
    OPT3101_configure();
}

void OPT3101_reset()
{
    timingGeneratorEnabled = false;

    P6->OUT &= ~BIT3;
    P6->DIR |= BIT3;
    DelayMs(1);
    P6->OUT |= BIT3;

    DelayMs(5);

    while (!(OPT3101_readReg(3) & (1 << 8)));
}

void OPT3101_configure()
{
    OPT3101_writeReg(0x89, 7000);     // TG_OVL_WINDOW_START = 7000
    uint32_t test = OPT3101_readReg(0x89);
    OPT3101_writeReg(0x6e, 0x0a0000); // EN_TEMP_CONV = 1
    OPT3101_writeReg(0x50, 0x200101); // CLIP_MODE_FC = 1
                                      // CLIP_MODE_TEMP = 0
                                      // CLIP_MODE_OFFSET = 0

    // IQ_READ_DATA_SEL = 2: This lets us read "raw" IQ values later.
    uint32_t reg2e = OPT3101_readReg(0x2e);
    reg2e = (reg2e & ~((uint32_t)7 << 9)) | (2 << 9);
    OPT3101_writeReg(0x2e, reg2e);
    reg2e = OPT3101_readReg(0x2e);

    OPT3101_setMonoshotMode();
    OPT3101_setFrameTiming(128);
    OPT3101_setBrightnessAdaptive();
    OPT3101_enableDataReadyInterrupt();
}

void OPT3101_setChannel(uint8_t channel)
{
    uint32_t reg2a = OPT3101_readReg(0x2a);

    if (channel == 255)
        reg2a |= (1 << 0); // EN_TX_SWITCH = 1
    
    else
    {
        reg2a &= ~((uint32_t)1 << 0); // EN_TX_SWITCH = 0
        reg2a = reg2a & ~((uint32_t)3 << 1) | (channel & 3) << 1;
    }

    OPT3101_writeReg(0x2a, reg2a);
}

void OPT3101_setBrightnessAdaptive()
{
    uint32_t reg2a = OPT3101_readReg(0x2a);

    reg2a |= (uint16_t)1 << 15; // EN_ADAPTIVE_HDR = 1

    OPT3101_writeReg(0x2a, reg2a);
}

void OPT3101_setMonoshotMode()
{
    // MONOSHOT_FZ_CLKCNT = default | MONOSHOT_NUMFRAME = 1 | MONOSHOT_MODE = 3
    OPT3101_writeReg(0x27, 0x26AC07);

    // DIS_GLB_PD_OSC = 1 | DIS_GLB_PD_AMB_DAC = 1 | DIS_GLB_PD_REFSYS = 1
    OPT3101_writeReg(0x76, 0x000121);

    // POWERUP_DELAY = 95
    OPT3101_writeReg(0x26, (uint32_t)95 << 10 | 0xF);
}

void OPT3101_setFrameTiming(uint16_t subFrameCount)
{
    // Implement equation 6 from sbau310.pdf to calculate
    // XTALK_FILT_TIME CONST, but without floating-point operations.
    uint8_t timeConst = 0;
    while ((subFrameCount << timeConst) < 1024)
        timeConst++;
    
    uint32_t reg2e = OPT3101_readReg(0x2e);

    reg2e = reg2e & ~(uint32_t)0xF00000 | (uint32_t)timeConst << 20;
    OPT3101_writeReg(0x2e, reg2e);

    // Set NUM_SUB_FRAMES and NUM_AVG_SUB_FRAMES.
    OPT3101_writeReg(0x9f, (subFrameCount - 1) | (uint32_t)(subFrameCount - 1) << 12);

    // Set TG_SEQ_INT_MASK_START and TG_SEQ_INT_MASK_END according to what
    // the OPT3101 datasheet says, but it's probably not needed.
    OPT3101_writeReg(0x97, (subFrameCount - 1) | (uint32_t)(subFrameCount - 1) << 12);

}

void OPT3101_enableTimingGenerator()
{
    OPT3101_writeReg(0x80, reg80Default | 1); // TG_EN = 1
    timingGeneratorEnabled = true;
}

void OPT3101_disableTimingGenerator()
{
    OPT3101_writeReg(0x80, reg80Default); // TG_EN = 0
    timingGeneratorEnabled = false;
}

void OPT3101_enableDataReadyInterrupt()
{
    //Set P6.2 to interrupt on data capture
    P6->DIR &= ~BIT2;
    P6->IFG &= ~BIT2;
    P6->IES &= ~BIT2;
    P6->OUT &= ~BIT2;
    P6->IE = BIT2;


    //Set GPIO1 on OPT board to be a data-ready signal
    uint32_t reg78 = OPT3101_readReg(0x78);
    uint32_t reg0b = OPT3101_readReg(0x0b);
    reg78 |= 0x1000;                   // GPIO1_OBUF_EN = 1
    reg78 = (reg78 & ~0x1C0) | 0x080;  // GPO1_MUX_SEL = 2 (DIG_GPO_0)
    reg0b = (reg0b & ~0xF) | 9;        // DIG_GPO_SEL0 = 9 (DATA_RDY)
    OPT3101_writeReg(0x78, reg78);
    OPT3101_writeReg(0x0b, reg0b);

    __NVIC_SetPriority(PORT6_IRQn, 21);
    __NVIC_EnableIRQ(PORT6_IRQn);
}

bool OPT3101_readyToSample()
{
    return readyToSample;
}

void OPT3101_startSample()
{
    if (!timingGeneratorEnabled)
        OPT3101_enableTimingGenerator();
    
    readyToSample = false;

    // Set MONOSHOT_BIT to 0 before setting it to 1, as recommended here:
    // https://e2e.ti.com/support/sensors/f/1023/p/756598/2825649#2825649
    OPT3101_writeReg(0x00, 0x000000);

    OPT3101_writeReg(0x00, 0x800000);
}

void OPT3101_readOutputRegs()
{
    uint32_t reg08 = OPT3101_readReg(0x08);
    uint32_t reg09 = OPT3101_readReg(0x09);
    uint32_t reg0a = OPT3101_readReg(0x0a);

    channelUsed = reg08 >> 18 & 3;
    if (channelUsed > 2)
        channelUsed = 2;
    
    //brightnessUsed = reg08 >> 17 & 1;

    i = OPT3101_readReg(0x3b);
    if (i > 0x7fffff)
        i -= 0x1000000;
    
    q = OPT3101_readReg(0x3c);
    if (q > 0x7fffff)
        q -= 0x1000000;
    
    amplitude = reg09 & 0xFFFF; // AMP_OUT
    phase = reg08 & 0xFFFF;     // PHASE_OUT

    // c / (2 * 10 MHz * 0x10000) = 0.22872349395 mm ~= 14990/0x10000
    distanceMillimeters = (int32_t)phase * 14990 >> 16;

    ambient = reg0a >> 2 & 0x3FF; // AMB_DATA

    amplitudes[channelUsed] = amplitude;
    distances[channelUsed] = distanceMillimeters;
}

void OPT3101_takeMeasurement(uint8_t channel)
{
    OPT3101_setChannel(channel);
    OPT3101_startSample();
}

void PORT6_IRQHandler()
{
    P6->IFG = 0x00;
    OPT3101_readOutputRegs();

    readyToSample = true;
}
