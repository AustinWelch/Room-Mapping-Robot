#include "BSP.h"

void Tachometer_init();
void Tachometer_leftMotorISR();
void Tachometer_rightMotorISR();
void Tachometer_disable();
void Tachometer_odometerEnabled();
void Tachometer_countToTravel(uint32_t count);