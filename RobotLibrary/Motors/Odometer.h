#include "BSP.h"

extern bool odometerActive;

extern void Odometer_start();
extern void Odometer_stop();
extern void Odometer_clear();
extern void Odometer_add(uint32_t addedValue);
extern uint32_t Odometer_value();
extern bool Odometer_isActive();
