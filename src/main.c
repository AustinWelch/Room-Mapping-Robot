#include "gridDisplay.h"
#include "MotorControl.h"
#include "OPT3101.h"
#include <msp432.h>
#include "driverlib.h"

//13.5 cm long
//6.9 cm wheel
int main(){
    //All initializations
    BSP_InitBoard(); 
    FPU_enableModule();
    __enable_interrupt();
    Motor_initialize();
    OPT3101_init();

    gridDisplay_init(); 
    uint32_t blockSize = gridDisplay_blockSizeSelection();
    coord initCoords = gridDisplay_chooseGridLocation();

    Grid_setBlockSize(blockSize);
    robotPos curPos = { .direction = 0, { .x = initCoords.x * Grid_getBlockSize() * 10000, .y = initCoords.y * Grid_getBlockSize() * 10000 }};
    Grid_setRobotPos(curPos);
    
    gridDisplay_createMainDisplay();

    int channel = 0;

    Odometer_start();
    
    // Main Loop
    while(true) {

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
                distances[2] = 35;

            if (channel == 2)
                distances[2] += 60; //Calibrated value based on testing

            if (distances[channel] < 850 && distances[channel] > 100)
                gridDisplay_drawMarker(Grid_convertDistanceToCoordinate(distances[channel], channel));
            
        } while (channel != 0);
        

        /************************** Navigation Algorithm **************************/
        uint32_t newDirection = curPos.direction;
        if (curPos.coordinates.x >= xMax || curPos.coordinates.x <= 0 || curPos.coordinates.y >= yMax ||  curPos.coordinates.y <= 0) {
            if (curPos.coordinates.x >= xMax && curPos.direction < 180) { // Outside on the right and going right so we need to turn around
                Motor_rotateLeft(30, 186);
                newDirection = (curPos.direction + 180) % 360;
            }
            else if (curPos.coordinates.x <= 0 && curPos.direction > 180) { // Outside on the left and going left so we need to turn around
                Motor_rotateLeft(30, 186);
                newDirection = (curPos.direction + 180) % 360;
            }
            else if (curPos.coordinates.y >= yMax && curPos.direction < 90 && curPos.direction > 270) { // Outside on the top and going upward so we need to turn around
                Motor_rotateLeft(30, 186);
                newDirection = (curPos.direction + 180) % 360;
            }
            else if (curPos.coordinates.y <= 0 && curPos.direction > 90 && curPos.direction < 270) { // Outside on the bottom and going downward so we need to turn around
                Motor_rotateLeft(30, 186);
                newDirection = (curPos.direction + 180) % 360;
            }
            
        }
        else if ((distances[LEFT] < 100) && (distances[CENTER] < 100) && (distances[RIGHT] < 100)) {
            Motor_rotateLeft(30, 186); //Turn values also calibrated based on testing
            newDirection = (curPos.direction + 180) % 360;
        }
        else if (distances[CENTER] < 100) {
            if (distances[LEFT] > distances[RIGHT]) {
                // Turning Left
                Motor_rotateLeft(30, 42);
                newDirection = (curPos.direction + 315) % 360;
            }
            else { 
                // Turning Right
                Motor_rotateRight(30, 42);
                newDirection = (curPos.direction + 45) % 360;
            }
        }
        else if (distances[LEFT] < 100) {
            Motor_rotateRight(30, 42);
            newDirection = (curPos.direction + 45) % 360;
        }
        else if (distances[RIGHT] < 100) {
            Motor_rotateLeft(30, 42);
            newDirection = (curPos.direction + 315) % 360;
        }
        curPos.direction = newDirection;
        Grid_setRobotPos(curPos);

        if(gridDisplay_isTouched()) {
            Motor_stop();
            break;
        }
    }

    while (true);
}
