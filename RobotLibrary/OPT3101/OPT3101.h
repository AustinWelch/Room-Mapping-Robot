#include "BSP.h"

extern uint8_t channelUsed;
extern bool timingGeneratorEnabled;
extern uint8_t address;

uint16_t ambient;
int32_t i, q;
uint16_t amplitude;
int16_t phase;
int16_t distanceMillimeters;

uint32_t amplitudes[3];
uint32_t distances[3];

void OPT3101_writeReg(uint8_t reg, uint32_t value);
uint32_t OPT3101_readReg(uint8_t reg);

void OPT3101_init();
void OPT3101_reset();
void OPT3101_configure();
void OPT3101_setChannel(uint8_t channel);
//void OPT3101_nextChannel();
void OPT3101_setBrightnessAdaptive();
void OPT3101_setMonoshotMode();
void OPT3101_setContinuousMode();
void OPT3101_setFrameTiming(uint16_t subFrameCount);
void OPT3101_enableTimingGenerator();
void OPT3101_disableTimingGenerator();
void OPT3101_enableDataReadyInterrupt();
bool OPT3101_readyToSample();
void OPT3101_startSample();
bool OPT3101_isSampleDone();
void OPT3101_readOutputRegs();
void OPT3101_takeMeasurement(uint8_t channel);
