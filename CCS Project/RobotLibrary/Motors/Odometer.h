#include "BSP.h"

extern bool odometerActive;

void Odometer_start();
void Odometer_stop();
void Odometer_clear();
void Odometer_add(uint32_t addedValue);
int32_t Odometer_value();
bool Odometer_isActive();
