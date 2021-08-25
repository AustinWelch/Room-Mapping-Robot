/**
 * @file      I2CB1.h
 * @brief     Provide busy-wait device driver for the I2C UCB1
 * @details   MSP432 is master. EUSCI_B1 I2C is connected to an external device<br>
 * This file originally comes from the TIDA-010021 Firmware (tidcf48.zip) and<br>
 * was modified by Pololu to support the MSP432P401R.
 * @remark    P6.4 I2C data
 * @remark    P6.5 I2C clock
 * @version   TI-RSLK MAX v1.1
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2020 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      June 2, 2020
<table>
<caption id="I2Cports">I2C connected an external device</caption>
<tr><th>Pin  <th>signal
<tr><td>P6.4 <td>I2C data (bidirectional)
<tr><td>P6.5 <td>I2C clock (MSP432 output)
</table>
 ******************************************************************************/

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

#include <stdint.h>

/*!
 * @defgroup I2C
 * @brief
 * @{*/
/**
 * @details   Initialize EUSCI_B1 for I2C operation<br>
 * Uses 12 MHz SMCLK clock,<br>
 * 7-bit address, 8 bit data<br>
 * for example prescale=30 means bit rate =12MHz/30 = 400 kHz
 * @param  prescale is clock scale, baud rate is 12MHz/prescale
 * @return none
 * @note   assumes 48 MHz bus and 12 MHz SMCLK
 * @brief  Initialize EUSCI_B1, I2C master
 */
void I2CB1_Init(uint32_t prescale);

/**
 * @details  Send a buffer of data to external device using I2C
 * @param  slaveAddr is the 7-bit slave address
 * @param  pData pointer to an array of 8-bit data (contains data)
 * @param  length is the number of elements in the array
 * @return none
 * @brief  Transmit data
 */
void I2CB1_Send(uint8_t slaveAddr, uint8_t *pData, uint32_t length);

/**
 * @details  Receive a buffer of data from external device using I2C<br>
 * Returns data in the pData buffer. Works for length greater than or equal to 2
 * for length=1, use I2CB1_Recv1
 * @param  slaveAddr is the 7-bit slave address
 * @param  pData pointer to an array of 8-bit data (initially empty data)
 * @param  length is the number of bytes one wished to receive
 * @return none
 * @brief  Receive data
 */
void I2CB1_Recv(uint8_t slaveAddr, uint8_t *pData, uint16_t length);

/**
 * @details  Receive one byte from external device using I2C<br>
 * @param  slaveAddr is the 7-bit slave address
 * @return data
 * @brief  Receive one byte of data
 */
uint8_t  I2CB1_Recv1(int8_t slaveAddr);

/**
 * @details  Send one byte data to external device using I2C
 * @param  slaveAddr is the 7-bit slave address
 * @param  data is the 8-bit data
 * @return none
 * @brief  Transmit one byte of data
 */
void I2CB1_Send1(uint8_t slaveAddr, uint8_t data);

/**
 * @details  Send two bytes data to external device using I2C
 * @param  slaveAddr is the 7-bit slave address
 * @param  regNum is the first 8-bit data (typically contains register number)
 * @param  data is the second 8-bit data (typically contains register value)
 * @return none
 * @brief  Transmit data
 */
void I2CB1_Send2(uint8_t slaveAddr, uint8_t regNum, uint8_t data);

/**
 * @details  Send three bytes data to external device using I2C
 * @param  slaveAddr is the 7-bit slave address
 * @param  regNum is the first 8-bit data (typically contains register number)
 * @param  data is an array of two 8-bit data (typically contains register values)
 * @return none
 * @brief  Transmit data
 */
void I2CB1_Send3(uint8_t slaveAddr, uint8_t regNum, uint8_t data[2]);

/**
 * @details  Send four bytes data to external device using I2C
 * @param  slaveAddr is the 7-bit slave address
 * @param  regNum is the first 8-bit data (typically contains register number)
 * @param  data is an array of three 8-bit data (typically contains register values)
 * @return none
 * @brief  Transmit data
 */
void I2CB1_Send4(uint8_t slaveAddr, uint8_t regNum, uint8_t data[3]);
