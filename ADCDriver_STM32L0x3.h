/*
 *  Created on: Oct 27, 2023
 *  Author: BalazsFarkas
 *  Project: STM32_ADC-DACDriver
 *  Processor: STM32L053R8
 *  Header version: 1.0
 *  File: ADCDriver_STM32L0x3.h
 */

#ifndef INC_ADCDRIVER_CUSTOM_H_
#define INC_ADCDRIVER_CUSTOM_H_

#include "stdint.h"

void ADCInit(void);
int32_t ADCIntTemp(void);
int32_t ADCSingleChannelReadout(void);

#endif /* INC_ADCDRIVER_CUSTOM_H_ */
