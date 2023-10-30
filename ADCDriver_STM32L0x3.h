/*
 * ADCDriver_custom.h		v.1.0.
 *
 *  Created on: Oct 27, 2023
 *      Author: Balazs Farkas
 */

#ifndef INC_ADCDRIVER_CUSTOM_H_
#define INC_ADCDRIVER_CUSTOM_H_

#include "stdint.h"

void ADCInit(void);
int32_t ADCIntTemp(void);
int32_t ADCSingleChannelReadout(void);

#endif /* INC_ADCDRIVER_CUSTOM_H_ */
