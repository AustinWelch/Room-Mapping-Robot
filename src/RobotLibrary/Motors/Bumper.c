#include "Bumper.h"
#include "MotorControl.h"

/*                  BMP3    BMP2
 *         BMP4                     BMP1
 * BMP5                                     BMP0
 *
 * BMP0 = BIT0
 * BMP1 = BIT2
 * BMP2 = BIT3
 * BMP3 = BIT5
 * BMP4 = BIT6
 * BMP5 = BIT7
 *
 * */

void Bumper_init() {
    P4->DIR &= ~(BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7);
    P4->IFG &= ~(BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7);
    P4->IES |= BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7;
    P4->REN |= BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7;
    P4->OUT |= BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7;

    P4->IE |= BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7;
    __NVIC_SetPriority(PORT4_IRQn, 32);
    __NVIC_EnableIRQ(PORT4_IRQn);
}

void PORT4_IRQHandler() {
    int8_t bumperState = P4->IFG;

    P4->IFG &= ~(BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7);
    P4->IE &= ~(BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7);

    Motor_stop();

    Motor_setBumperState(bumperState);
}
