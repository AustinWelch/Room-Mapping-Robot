#include "Tachometer.h"
#include "Odometer.h"


uint32_t Rcount = 0;
uint32_t Lcount = 0;
uint32_t LcountPrev = 0;
int32_t RcountPrev = 0;
bool odometerJustEnabled = false;
uint32_t speed = 0;
uint32_t period = 0;
uint32_t prevPeriod = 0;


void Tachometer_init(){
    //Pin 10.4 for right tachometer
    //Pin 10.5 for left tachometer

    //Set Pins 10.4 and 10.5 as inputs and to capture on timer A3
    P10->DIR &= ~(BIT4 | BIT5);
    P10->SEL1 &= ~(BIT4 | BIT5);
    P10->SEL0 |= BIT4 | BIT5;

    //Set up timer A3 to interrupt on P10.4 and P10.5 rising edge, use system CLK, sync capture source, and capture mode
    TIMER_A3->CTL |= TACLR;
    TIMER_A3->CTL = TASSEL__SMCLK | MC__STOP | TAIE;

    TIMER_A3->CCTL[0] = CM__RISING | CCIS__CCIA | SCS | CAP | CCIE;
    TIMER_A3->CCTL[1] = CM__RISING | CCIS__CCIA | SCS | CAP | CCIE;

    //Enable Interrupts
    __NVIC_SetPriority(TA3_0_IRQn, 20);
    __NVIC_EnableIRQ(TA3_0_IRQn);
    __NVIC_SetPriority(TA3_N_IRQn, 20);
    __NVIC_EnableIRQ(TA3_N_IRQn);

    //Start Timer
    TIMER_A3->CTL |= MC__UP;
}

void TA3_N_IRQHandler(){
    TIMER_A3->CCTL[1] &= ~BIT0;
    
    Lcount++;
    if(Lcount - LcountPrev == 20){// || odometerJustEnabled){
        //odometerJustEnabled = false;

        LcountPrev = Lcount;
        period = TIMER_A3->CCR[0] - prevPeriod;
        prevPeriod = TIMER_A3->CCR[0];

        //Calculate speed (assume robot is traveling straight, in units of 0.0001 cm)
        speed = Lcount * 219911 * 12000000 / (360 * period);

//        if(Odometer_isActive()){
//            uint32_t distanceTraveled = Lcount * 219911 / 360;
//            Odometer_add(distanceTraveled);
//        }
    }
}

void TA3_0_IRQHandler() {//Tachometer_rightMotorISR(){
    TIMER_A3->CCTL[0] &= ~BIT0;
    
    Rcount++;
    if(Rcount - RcountPrev == 20 || odometerJustEnabled){
        odometerJustEnabled = false;

        RcountPrev = Rcount;
        period = TIMER_A3->CCR[0] - prevPeriod;
        prevPeriod = TIMER_A3->CCR[0];

        //Calculate speed (assume robot is traveling straight, in units of 0.0001 cm)
        speed = Rcount * 219911 * 12000000 / (360 * period);

        if(Odometer_isActive()){
            uint32_t distanceTraveled = (20 * 219911) / 360;
            Odometer_add(distanceTraveled);
        }

    }
}

void Tachometer_disable(){
    TIMER_A3->CTL |= MC__STOP;
    __NVIC_DisableIRQ(TA3_0_IRQn);
    __NVIC_DisableIRQ(TA3_N_IRQn);
}

void Tachometer_odometerEnabled(){
    odometerJustEnabled = true;
}

void Tachometer_countToTravel(uint32_t count){
    uint32_t target = count;
    uint32_t Rorig = Rcount;
    uint32_t Lorig = Lcount;

    //TODO: replace with mutex
    while(1){
       if(Rcount-Rorig == target)
           P3->OUT &= ~BIT6;

       if(Lcount-Lorig == target)
           P3->OUT &= ~BIT7;

       if(Rcount-Rorig >= target && Lcount-Lorig >= target)
           break;
    }
}
