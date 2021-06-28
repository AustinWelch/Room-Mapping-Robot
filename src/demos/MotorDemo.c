#include <Motors/Bumper.h>
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

          Motor_moveForwardTime(20, 0);

          while(Motor_getBumperState() == 0);

          Motor_handleBump();
          Motor_setBumperState(0);

    }
}
