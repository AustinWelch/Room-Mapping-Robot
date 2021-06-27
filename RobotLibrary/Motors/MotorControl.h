#include "BSP.h"

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

extern void Motor_initialize();
extern void Motor_moveForwardTime(uint16_t percentSpeed, uint16_t time);
void Motor_moveForwardDistance(uint16_t percentSpeed, uint16_t distance);
extern void Motor_moveBackwardTime(uint16_t percentSpeed, uint16_t time);
void Motor_moveBackwardDistance(uint16_t percentSpeed, uint16_t distance);
extern void Motor_rotateLeft(uint16_t percentSpeed, uint16_t degrees);
extern void Motor_rotateRight(uint16_t percentSpeed, uint16_t degrees);
extern void Motor_stop();
extern void PWM_init(uint16_t percentSpeed);
