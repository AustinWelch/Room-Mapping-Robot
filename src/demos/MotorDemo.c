#include "BSP.h"
#include "Motors/MotorControl.h"

int main(){

    BSP_InitBoard();


    /* Right Motor

        P5.5      P2.6
     Direction    PWM
         0         0      Stopped
         1         0      Stopped
         0         1      Forward
         1         1      Backward

     P3.6 is active low right motor sleep

     Left Motor

        P5.4      P2.7
     Direction    PWM
         0         0      Stopped
         1         0      Stopped
         0         1      Forward
         1         1      Backward

     P3.7 is active low left motor sleep
     */

    Motor_initialize();

    while(1){

//        Motor_moveForwardDistance(20, 20);
        DelayMs(750);
        Motor_rotateRight(20, 90);
        DelayMs(750);
        Motor_rotateLeft(20, 90);
        DelayMs(750);
        Motor_rotateRight(20, 180);
        DelayMs(750);
        Motor_rotateLeft(20, 180);
        DelayMs(750);
        Motor_rotateRight(20, 360);
        DelayMs(750);
        Motor_rotateLeft(20, 360);
        DelayMs(750);
//        DelayMs(750);
//        Motor_moveForwardDistance(20, 20);
//        DelayMs(750);
//        Motor_moveBackwardDistance(20, 20);
//        DelayMs(750);
//        Motor_rotateRight(20, 180);
//        DelayMs(10000);
    }
}
