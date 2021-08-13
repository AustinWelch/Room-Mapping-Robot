#include "Odometer.h"
#include "Tachometer.h"

int32_t odo_distance = 0;
bool odometerActive = false;

/*
    Ensure the Tachometer is enabled BEFORE starting the odometer
*/
void Odometer_start(){
    odometerActive = true;
    Tachometer_odometerEnabled();
}

void Odometer_stop(){
    odometerActive = false;
}

void Odometer_clear(){
    odo_distance = 0;
}

void Odometer_add(uint32_t addedValue){
    odo_distance += addedValue;
}

int32_t Odometer_value(){
    return odo_distance;
}

bool Odometer_isActive(){
    return odometerActive;
}
