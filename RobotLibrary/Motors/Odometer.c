#include "Odometer.h"
#include "Tachometer.h"

uint32_t distance = 0;
bool odometerActive = false;

/*
    Ensure the Tachometer is enabled BEFORE starting the odometer
*/
void Odometer_start(){
    bool odometerActive = true;
    Tachometer_odometerEnabled();
}

void Odometer_stop(){
    bool odometerActive = false;
}

void Odometer_clear(){
    distance = 0;
}

void Odometer_add(uint32_t addedValue){
    distance += addedValue;
}

uint32_t Odometer_value(){
    return distance;
}

bool Odometer_isActive(){
    return odometerActive;
}
