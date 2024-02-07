/*
 *
 *  Created on: Oct 27, 2023
 *  Project: STM32_ADC-DACriver
 *  File: DACDriver_custom.h
 *  Author: BalazsFarkas
 *  Processor: STM32L053R8
 *  Compiler: ARM-GCC (STM32 IDE)
 *  Program version: 1.0
 *  Program description: N/A
 *  Hardware description/pin distribution: N/A
 *  Modified from: N/A
 *  Change history: N/A
 */

#ifndef INC_DACDRIVER_CUSTOM_H_
#define INC_DACDRIVER_CUSTOM_H_

#include "stdint.h"

//LOCAL CONSTANT

//LOCAL VARIABLE

//EXTERNAL VARIABLE

//FUNCTION PROTOTYPES

void DACInit(void);
void DACGenerate(int16_t DAC_input);
void DACNoise(int16_t DAC_input, uint8_t DAC_noise_level);


#endif /* INC_DACDRIVER_CUSTOM_H_ */
