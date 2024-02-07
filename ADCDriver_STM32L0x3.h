/*
 *
 *  Created on: Oct 27, 2023
 *  Project: STM32_ADc-DACriver
 *  File: ADCDriver_custom.h
 *  Author: BalazsFarkas
 *  Processor: STM32L053R8
 *  Compiler: ARM-GCC (STM32 IDE)
 *  Program version: 1.0
 *  Program description: N/A
 *  Hardware description/pin distribution: N/A
 *  Modified from: N/A
 *  Change history: N/A
 */

#ifndef INC_ADCDRIVER_CUSTOM_H_
#define INC_ADCDRIVER_CUSTOM_H_

#include "stdint.h"

void ADCInit(void);
int32_t ADCIntTemp(void);
int32_t ADCSingleChannelReadout(void);

#endif /* INC_ADCDRIVER_CUSTOM_H_ */
