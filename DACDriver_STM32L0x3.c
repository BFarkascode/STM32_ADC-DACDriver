/*
 * DACDriver_custom.c			v.1.0
 *
 *  Created on: Oct 27, 2023
 *      Author: Balazs Farkas
 *
 *
 * v.1.0
 * Below is a custom DAC driver.
 * No external triggers, no DMA, no interrupts.
 * The driver runs just one DAC channels to allow direct compatibility across the L0xx devices.
 * The initialization is followed by feeding data into the DAC.
 * We are using 12-bit output (though changing to 8-bit is just using a different input register).
 *
 */

#include "DACDriver_STM32L0x3.h"
#include "stm32l053xx.h"										//device specific header file for registers

//1)We set up the basic driver for the ADC
void DACInit(void){
	/*
	 * Below is an initialization for the DAC within the stm32L0x3
	 * Mind, according to the datasheet, we only have 1 channel available on the L0x3. This is inaccurately discussed in the refman where it is assumed to have 2 instead (like in L0x2).
	 * For compatibility across the L0xx family, we only use 1 channel.
	 *
	 * 1)Set GPIO pins and choose APB clocking (if needed)
	 * 2)Set DAC: enable channel, choose wave generator (if any) and trigger mode
	 * 3)Configure ADC: ADC clocking source and prescaling, SMP, RES, conversion mode, watchdog/oversampling, autooff, wait
	 * 4)Enable drivers: scan direction, enable ADC
	 * */

	//1)GPIOs
	RCC->IOPENR |= (1<<0);										//PORTA clocking allowed
																//DAC output is on PA4
																//GPIO analogue mode necessary with push-pull, no pull resistors, low speed output
																//Note: on the L0xx, apart from the clocking of the port, all values reset to what we need

	//2)Clocking
	RCC->APB1ENR |= (1<<29);									//enable the DAC clocking


	//3)Set DAC up
	DAC->CR |= (7<<3);											//we use software trigger to control the DAC so we write 3'b111 in here
																//we want our code to start the conversion by writing to the SWTRIG register (see below)
	DAC->CR |= (1<<2);											//we enable the trigger channel 1
	DAC->CR &= ~(1<<1);											//we enable the output buffer
																	//Note: for this bit, enable is RESET, disable is SET
																//We don't use the wave generator, masking and DMA.

	//4) Enable the DAC
	DAC->CR |= (1<<0);
}


//2)DAC voltage output
void DACGenerate(int16_t DAC_input){
	/*
	 * Below is a simple DAC loading function.
	 * It is currently hard-wired to have an 12-bit DAC
	 * Bit depth is changed by simply putting the desired value in a different register (to DHR8R1)
	 * */
//	DAC->DHR8R1 = DAC_input;									//8-bit depth
	DAC->CR  &= ~(3<<6);										//we disable the wave generators (just in case)
	DAC->DHR12R1 = DAC_input;									//we load the value we want to generate into the DAC
	DAC->SWTRIGR |= (1<<0);										//we trigger conversion
	while(DAC->SWTRIGR & (1<<0) == 1);							//trigger is pulled LOW when the DHR register has been loaded into the DOR output register within the DAC
}

//3)DAC noise generator
void DACNoise(int16_t DAC_input, uint8_t DAC_noise_level){
	/*
	 * Below is the noise generator of the DAC.
	 * The noise is added to the voltage level defined by the DHR8R1/DHR12R1 register.
	 * The function's second input defines the noise level between 0 and 15, where 0 is no noise.
	 *
	 * */
//	DAC->DHR8R1 = DAC_input;									//8-bit depth
	DAC->DHR12R1 = DAC_input;									//we load the value we want to generate into the DAC
	DAC->CR  |= (DAC_noise_level<<8);							//we unmask all the noise bits
	DAC->CR  |= (1<<6);											//noise generator activated
	DAC->SWTRIGR |= (1<<0);										//we trigger conversion
	while(DAC->SWTRIGR & (1<<0) == 1);							//trigger is pulled LOW when the DHR register has been loaded into the DOR output register within the DAC
}

