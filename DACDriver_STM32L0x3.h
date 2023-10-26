/*
 * DACDriver_custom.h		v.1.0.
 *
 *  Created on: Oct 27, 2023
 *      Author: Balazs Farkas
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
