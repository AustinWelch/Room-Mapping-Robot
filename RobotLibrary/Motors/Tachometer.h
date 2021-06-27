#include "BSP.h"

extern void Tachometer_init();
extern void Tachometer_leftMotorISR();
extern void Tachometer_rightMotorISR();
extern void Tachometer_disable();
extern void Tachometer_odometerEnabled();
void Tachometer_countToTravel(uint32_t count);