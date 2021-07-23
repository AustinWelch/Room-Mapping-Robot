#include "gridDisplay.h"
#include "MotorControl.h"
#include "OPT3101.h"

int main(){
    // All initializations
    BSP_InitBoard(); 
    // Maybe FPU init
    __enable_interrupt();
    Motor_initialize();
    OPT3101_init();

    robotPos curPos = { .direction = 0, { .x = 80 * Grid_getBlockSize() * 10000, .y = 0}};
    Grid_setRobotPos(curPos);
    Grid_setBlockSize(5);

    gridDisplay_init();
    
    int channel = 0;

    Odometer_start();
    
    // Main Loop
    while(1) {
        curPos = Grid_getRobotPos();

        /************************** Movement **************************/
        Motor_moveForwardTime(30, 0);

        
        /******** Sensor Readings/ Position Updating/ Plotting ********/
        do {
            int32_t distanceTraveled = Odometer_value();
            Odometer_clear();
            uint32_t adjustedAngle = (curPos.direction + 270) % 360;
            coord newLocation;
            newLocation.x = (curPos.coordinates.x + (distanceTraveled * cos_LUT[adjustedAngle] / 10000));
            newLocation.y = (curPos.coordinates.y + (distanceTraveled * cos_LUT[curPos.direction] / 10000));
        
            curPos.coordinates = newLocation;
            Grid_setRobotPos(curPos);

            gridDisplay_addToTotalDistance(distanceTraveled);
            gridDisplay_updateInfo();

            OPT3101_takeMeasurement(channel);

            while(!OPT3101_readyToSample()); //150 ms

            OPT3101_readOutputRegs(); //readOutputRegs writes current channel to distances[]
            channel = (channel + 1) % 3;

            if (distances[2] > 100000)
                distances[2] = 10;

            if (distances[channel] < 850 && distances[channel] > 50)
                gridDisplay_drawMarker(Grid_convertDistanceToCoordinate(distances[channel], channel));
            
        } while (channel != 0);
        
        
                
        /************************** Navigation Algorithm **************************/
        //TODO: consider bumpers
        //TODO: Cage considerations
        uint32_t newDirection = curPos.direction;
        if ((distances[LEFT] < 100) && (distances[CENTER] < 100) && (distances[RIGHT] < 100)) {
            Motor_rotateLeft(30, 180);
            newDirection = (curPos.direction + 180) % 360;
        }
        else if (distances[CENTER] < 100) {
            if (distances[LEFT] > distances[RIGHT]) {
                // Turning Left
                Motor_rotateLeft(30, 45);
                newDirection = (curPos.direction + 315) % 360;
            }
            else { 
                // Turning Right
                Motor_rotateRight(30, 45);
                newDirection = (curPos.direction + 45) % 360;
            }
        }
        else if (distances[LEFT] < 100) {
            Motor_rotateRight(30, 45);
            newDirection = (curPos.direction + 45) % 360;
        }
        else if (distances[RIGHT] < 100) {
            Motor_rotateLeft(30, 45);
            newDirection = (curPos.direction + 315) % 360;
        }
        curPos.direction = newDirection;
        Grid_setRobotPos(curPos);


        /*if near edge of cage 
            if top

            if right

            if bottom

            if left
            */
    }
}
