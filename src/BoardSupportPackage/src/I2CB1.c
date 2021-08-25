// I2CB1.c
// Runs on MSP432, starter version
// Busy-wait device driver for the I2C UCB1.
// Daniel and Jonathan Valvano
// December 8, 2019
// This file originally comes from the TIDA-010021 Firmware (tidcf48.zip) and
// was modified by Pololu to support the MSP432P401R.

/* This example accompanies the book
   "Embedded Systems: Introduction to Robotics,
   Jonathan W. Valvano, ISBN: 9781074544300, copyright (c) 2019
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2019, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// hardware
// SDA    P6.4 I2C data
// SCL    P6.5 I2C clock
#include <stdint.h>
#include "I2CB1.h"
#include "msp.h"
// Uses 12 MHz SMCLK clock
// prescale sets bit rate = 12MHz/prescale
// for example prescale=30 means bit rate =12MHz/30 = 400 kHz
void I2CB1_Init(uint32_t prescale){
    // initialize eUSCI
  EUSCI_B1->CTLW0 |= 0x0001;    // hold eUSCI_B1 in reset
// configure UCB1CTLW0 for:
// bit15      UCA10 = 0; own address is 7-bit address
// bit14      UCSLA10 = 0; address slave with 7-bit address
// bit13      UCMM = 0; single master environment
// bit12      reserved
// bit11      UCMST = 1; master mode
// bits10-9   UCMODEx = 3; I2C mode
// bit8       UCSYNC = 1; synchronous mode
// bits7-6    UCSSELx = 3; eUSCI clock SMCLK
// bit5       UCTXACK = X; transmit ACK condition in slave mode
// bit4       UCTR = X; transmitter/receiver
// bit3       UCTXNACK = X; transmit negative acknowledge in slave mode
// bit2       UCTXSTP = X; transmit stop condition in master mode
// bit1       UCTXSTT = X; transmit start condition in master mode
// bit0       UCSWRST = 1; reset enabled
  EUSCI_B1->CTLW0 = 0x0FC1;

// configure UCB1CTLW1 for:
// bits15-9   reserved
// bit8       UCETXINT = X; early UCTXIFG0 in slave mode
// bits7-6    UCCLTO = 0; disable timeout clock
// bit5       UCSTPNACK = 0; send negative acknowledge before stop condition in master receiver mode
// bit4       UCSWACK = 0; slave address acknowledge controlled by hardware
// bits3-2    UCASTPx = 0; no automatic stop condition after UCB0TBCNT bytes
// bits1-0    UCGLITx = 0 deglitch time of 50 ns
  EUSCI_B1->CTLW1 = 0;

// set the baud rate for the eUSCI which gets its clock from SMCLK
// Clock_Init48MHz() from ClockSystem.c sets SMCLK = HFXTCLK/4 = 12 MHz
// if the SMCLK is set to 12 MHz, divide by 120 for 100 kHz baud clock
// prescale= clock/bit rate = 12M/400k = 30
  EUSCI_B1->BRW = prescale; // UCBR = bit clock prescaler

  P6->SEL0 |= 0x30;
  P6->SEL1 &= ~0x30;          // configure P6.4, 6.5 as UCB1SDA, UCB1SCL
  EUSCI_B1->CTLW0 &= ~0x0001; // enable eUSCI_B1
  EUSCI_B1->IE = 0x0000;      // disable all interrupts (transmit empty, receive full)
}

void I2CB1_Send(uint8_t slaveAddr, uint8_t *pData, uint32_t length){
  while(EUSCI_B1->STATW & 0x0010){}; // UCBBUSY, wait until I2C not busy
  EUSCI_B1->I2CSA = slaveAddr; // set slave address
// I2C master transmit mode
  EUSCI_B1->CTLW0 = (EUSCI_B1->CTLW0 & ~0x0004)|0x0012; // Master transmit, no stop, start condition
//write the data
  for(uint32_t i = 0; i < length; i++){
    while((EUSCI_B1->IFG & 0x0002)==0){};// wait for UCTXIFG0
    EUSCI_B1->TXBUF = pData[i];
  }
  while((EUSCI_B1->IFG & 0x0002)==0){}; // wait for UCTXIFG0

  EUSCI_B1->CTLW0 |= 0x0004; // generate UCTXSTP
  EUSCI_B1->IFG &= ~0x0002;  // clear UCTXIFG0, TX interrupt flag
}
// works for length greater than or equal to 2
// for length=1, use I2CB1_Recv1
void I2CB1_Recv(uint8_t slaveAddr, uint8_t *pData, uint16_t length){
  EUSCI_B1->I2CSA = slaveAddr;

// I2C master receive mode
  EUSCI_B1->CTLW0 &= ~0x0010; // UCTR=0, receive mode
  EUSCI_B1->CTLW0 |= 0x0002; // UCTXSTT=1, Master receive, start condition

  for(uint32_t i = 0; i < length; i++){
    if(i == length-1){
      EUSCI_B1->CTLW0 |= 0x0004;  // UCTXSTP=1, if this is the last byte, NACK and stop (must do this before byte is received?)
    }
    while((EUSCI_B1->IFG & 0x0001)==0){}; // wait for UCRXIFG0
    pData[i] = EUSCI_B1->RXBUF; // Get RX data
  }
  while(EUSCI_B1->CTLW0 & 0x0004){}; // wait while UCTXSTP=1
}
void I2CB1_Send1(uint8_t slaveAddr, uint8_t data){
  while(EUSCI_B1->STATW&0x0010){}; // wait UCBBUSY
  EUSCI_B1->I2CSA = slaveAddr; // set slave address
// I2C master transmit mode
  EUSCI_B1->CTLW0 = (EUSCI_B1->CTLW0 & ~0x0004)|0x0012; // Master transmit, no stop, start condition
  while((EUSCI_B1->IFG&0x0002)==0){};// wait UCTXIFG0
  EUSCI_B1->TXBUF = data;
  while((EUSCI_B1->IFG&0x0002)==0){}; // wait UCTXIFG0
  EUSCI_B1->CTLW0 |= 0x0004; // generate UCTXSTP
  EUSCI_B1->IFG &= ~0x0002;  // clear UCTXIFG0
}
