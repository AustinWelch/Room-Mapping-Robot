#include "MotorControl.h"
#include "Odometer.h"
#include "Tachometer.h"
#include "Bumper.h"

/*
    TODO: Replace DelayMs with interrupt
*/

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

uint8_t bumperState = 0;

void Motor_initialize(){

    //Initialize I/O
    P2->DIR |= BIT6 | BIT7;
    P3->DIR |= BIT6 | BIT7;
    P5->DIR |= BIT4 | BIT5;

    P3->OUT &= ~(BIT6 | BIT7);
    P5->OUT &= ~(BIT4 | BIT5);

    //Set P2 I/O to use Timer A compare module direct output
    P2->SEL0 |= BIT6 | BIT7;
    P2->SEL1 |= ~(BIT6 | BIT7);

    TIMER_A0->CCTL[3] |= OUTMOD_7;
    TIMER_A0->CCTL[4] |= OUTMOD_7;

    Bumper_init();
}

void Motor_moveForwardTime(uint16_t percentSpeed, uint16_t time){
    P3->OUT |= BIT6 | BIT7;
    P5->OUT &= ~(BIT4 | BIT5);

    PWM_init(percentSpeed);

    if(time == 0)
        return;

    DelayMs(time);
    Motor_stop();
}

void Motor_moveForwardDistance(uint16_t percentSpeed, uint16_t distance){
    P3->OUT |= BIT6 | BIT7;
    P5->OUT &= ~(BIT4 | BIT5);

    PWM_init(percentSpeed);

    Tachometer_countToTravel(distance * 10000 * 360 / 219911);

    Motor_stop();
}

void Motor_moveBackwardTime(uint16_t percentSpeed, uint16_t time){
    P3->OUT |= BIT6 | BIT7;
    P5->OUT |= BIT4 | BIT5;

    PWM_init(percentSpeed);

    if(time == 0)
        return;

    DelayMs(time);
    Motor_stop();
}

void Motor_moveBackwardDistance(uint16_t percentSpeed, uint16_t distance){
    P3->OUT |= BIT6 | BIT7;
    P5->OUT |= BIT4 | BIT5;

    PWM_init(percentSpeed);

    Tachometer_countToTravel(distance * 10000 * 360 / 219911);
     
    Motor_stop();
}

void Motor_rotateLeft(uint16_t percentSpeed, uint16_t degrees){
    P3->OUT |= BIT6 | BIT7;
    P5->OUT |= BIT4;
    P5->OUT &= ~BIT5;

    PWM_init(percentSpeed);

    Tachometer_countToTravel((degrees - degrees/20) * 2);

    Motor_stop();
}

void Motor_rotateRight(uint16_t percentSpeed, uint16_t degrees){
    P3->OUT |= BIT6 | BIT7;
    P5->OUT &= ~BIT4;
    P5->OUT |= BIT5;

    PWM_init(percentSpeed);

    Tachometer_countToTravel((degrees - degrees/20) * 2);

    Motor_stop();
}

void Motor_stop(){
    TIMER_A0->CTL |= MC__STOP;
    P3->OUT &= ~(BIT6 | BIT7);
    Tachometer_disable();
}

void PWM_init(uint16_t percentSpeed){
    TIMER_A0->CTL |= TACLR;

    TIMER_A0->CCR[0] = 1000;
    TIMER_A0->CCR[3] = 10 * percentSpeed / 2;
    TIMER_A0->CCR[4] = 10 * percentSpeed / 2;

    TIMER_A0->CTL |= TASSEL__SMCLK;

    TIMER_A0->CTL |= MC__UP;

    Tachometer_init();
}

void Motor_handleBump(){
    switch (bumperState) {
          case BIT0:
              Motor_rotateLeft(20, 40);
              break;
          case BIT2:
              Motor_rotateLeft(20, 60);
              break;
          case BIT3:
              Motor_rotateLeft(20, 90);
              break;
          case BIT5:
              Motor_rotateRight(20, 90);
              break;
          case BIT6:
              Motor_rotateRight(20, 60);
              break;
          case BIT7:
              Motor_rotateRight(20, 40);
              break;
      }

    P4->IFG = 0;
    P4->IE |= BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7;
}

uint8_t Motor_getBumperState(){
    return bumperState;
}

void Motor_setBumperState(uint8_t newState){
    bumperState = newState;
}
