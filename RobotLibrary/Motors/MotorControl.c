#include "MotorControl.h"
#include "Odometer.h"
#include "Tachometer.h"

/*
    TODO: Replace DelayMs with interrupt
*/

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

    Tachometer_countToTravel((degrees - 6) * 2);

    Motor_stop();
}

void Motor_rotateRight(uint16_t percentSpeed, uint16_t degrees){
    P3->OUT |= BIT6 | BIT7;
    P5->OUT &= ~BIT4;
    P5->OUT |= BIT5;

    PWM_init(percentSpeed);

    Tachometer_countToTravel(degrees * 2);

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
