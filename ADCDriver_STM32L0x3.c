/*
 *  Created on: Oct 27, 2023
 *  Author: BalazsFarkas
 *  Project: STM32_ADC-DACDriver
 *  Processor: STM32L053R8
 *  Program version: 1.0
 *  File: ADCDriver_STM32L0x3.c
 *  Change history:
 *
 * v.1.0
 * Below is a custom ADC driver.
 * No external triggers, no DMA, no interrupts.
 * The driver is a setup for ALL channels of the ADC. If single channels are separately manipulated, sampling time will need to be adjusted specifically for each channel.
 * Also, ALL channels dump their data into the same output register. This is not a problem with single channel manipulation (as below) but is a problem when multiple channels are used at the same time. Overruns could occur.
 * AUTOFF and WAIT are engaged since we aren't in need to read data quickly. AUTOFF and WAIT also decrease the ADC power consumption.
 *
 * The initialization is followed by a single-channel management functions for the internal temperature sensor, and the another (currently unused) for an external photocell on the GPIO PB0.
 *
 */

#include "ADCDriver_STM32L0x3.h"
#include "stm32l053xx.h"										//device specific header file for registers

//1)We set up the basic driver for the ADC
void ADCInit(void){
	/*
	 * What are we doing here?
	 * 1)We enable the clocking of the ADC if it is not done already, then wait until we can shut off the ADC in case it was running. This is necessary since the ADC should not be modified on the fly.
	 * 2)We calibrate the ADC. assuming that the it is necessary (recommended practice)
	 * 3)We pick the clocking, which here is an APB-asynchronous HSI16 clock
	 * 4)We DON'T enable any peripherals, nor do we define a sampling time. This is decided as such to force both of those to be set locally within the sensor's own readout function (see below)
	 *
	 *
	 * 1)Set APB clocking
	 * 2)Set ADC: internal voltage regulator, trigger control, calibration
	 * 3)Configure ADC: ADC clocking source and prescaling, SMP, RES, conversion mode, watchdog/oversampling, autooff, wait
	 * 4)Enable drivers: scan direction, enable ADC
	 * */
	//1)Clocking
	RCC->APB2ENR |= (1<<9);										//enable the ADC clocking


	//2)Setup
	if (ADC1->CR & (1<<2) !=0) {								//if the ADC is measuring
		ADC1->CR |= (1<<4);										//we stop it
		while((ADC1->CR & (1<<4)) == 1);						//we wait until the ADC is stopped and the ADSTP bit goes LOW
	} else {
		//do nothing
	}

	if (ADC1->CR & (1<<0) !=0) {								//if the ADC is running already
		ADC1->CR |= (1<<1);										//we shut off the ADC
		while((ADC1->CR & (1<<1)) == 1);						//we wait until the ADC is shut off and the ADDIS bit goes LOW
	} else {
		//do nothing
	}

	ADC1->CFGR1 = 0x0;											//AUTOFF, DMA, EXTEN off (if it was on) so it won't restart
	ADC1->CFGR2 = 0x0;											//oversampling, clocking reset
	ADC1->CR = 0x0;												//we wipe the control register of the ADC

	ADC1->CR |= (1<<31);										//we start calibration
	while((ADC1->ISR & (1<<11)) == 0);							//we wait until the EOCAL flag goes HIGH and thus ADC becomes calibrated
	ADC1->ISR |= (1<<11);										//we remove the EOCAL flag by writing a 1 to it

	//3)Configure
	ADC1->CFGR1 |= (1<<15);										//AUTOOFF on
																//Note: the AUTOOFF removes the ADRDY control flag. Turning off becomes automatic.

	ADC1->CFGR1 |= (1<<14);										//WAIT on
																//RES kept at 12 bit, no external triggers, single conversion mode, no DISCEN, SCANDIR is upward, no DMA

	ADC1->CFGR2 = 0x0;											//clock source is direct HSI16 without AHB or APB prescalers
																//no oversampling

	ADC1->SMPR = 0x0;											//we put 1.5 ADC (which is chosen by leaving this at 0x0) cycles for sample time (must be adjusted to whatever the sample rate is for the device we measure)
																//for the internal temperature sensor, this needs to be (7<<1) since we need 10 us sampling time
	ADC->CCR = 0x0;												//we don't enable any in-built peripherals
																//also, no prescaler!


	//4) Enable the ADC
	//This part is only necessary if we aren't using AUTOFF.
	//Note: With AUTOFF, the ADRDY flag is not working anymore!

//	ADC1->ISR |= (1<<0);										//we remove the ADRDY flag by writing a 1 to it
//	ADC1->CR |= (1<<0);											//we enable the ADC
//	while((ADC1->ISR & (1<<0)) == 0);							//we wait until the ADRDY flag goes HIGH and thus ADC becomes ready
//	ADC1->ISR |= (1<<0);										//we remove the ADRDY flag by writing a 1 to it

}

//2)Internal tmeperature reading
int32_t ADCIntTemp(void) {
	/*
	 * What happens here?
	 * 1) We need to pick the channels within the ADC, then choose an appropriate sampling time for it that matches the demand posed by whatever is on the other end of the channel.
	 * 2) Here we will use the internal temperature sensor on channel 18 with a sampling time that must be longer than 10 us. Mind, the sensor must be enabled too!
	 * 3) Lastly, we use the the equations from section 14.9 to calculate the temperature from the raw ADC data.
	 *
	 * There is an internal temperature measurement capability within the micro that measures the junction temperature.
	 * Below we read out the values from the sensor using the ADC.
	 * We enable the peripheral and the appropriate channel. We configure the ADC to be able to deal with the temperature sensor's output.
	 * We DON'T initialize or enable the ADC itself but assume that it is already initialized and enabled at maximum 16 MHz clocking speed.
	 *
	 * 1)Enable channels
	 * 2)Start conversion: ADSTART
	 * 3)Extract data: EOC control
	 * */

	int16_t temp;

#define TS_CAL1 ((uint16_t *) 0x1FF8007A)						//sensor calibrated at 30 deg Celsius, value stored at address indicated
#define TS_CAL2 ((uint16_t *) 0x1FF8007E)						//sensor calibrated at 130 deg Celsius, value stored at address indicated
																//Note: be careful the with the data size and the pointer size. Wrong size could lose data or freeze the micro.

	ADC1->CR |= 0x0;											//we clean the control register
																//Note: we need to use ADC1, not ADC, to comply to the general naming of the register (despite only having one ADC).

	ADC->CCR |= (1<<23);										//we enable the temperature sensor
	ADC1->CHSELR |= (1<<18);									//we enable channel 18 where the temperature sensor is
	ADC1->SMPR |= (7<<0);										//we need 10 us sample time for the temperature sensor, so we need to set it here for 160.5 cycles (3'b111)
																//Note: if we change the SMPR value here or the clocking in the CFGR2, the ADC will still work...but the extracted data will not be accurate
	ADC1->CR |= (1<<2);											//we start conversion
	while((ADC1->ISR & (1<<2)) == 0);							//when the EOC goes HIGH, we go on
	temp = ADC1->DR;											//this reads out the data and clears the EOC flag

	//Note: these following lines are taken directly from the refman
	temp = ((temp * ((uint16_t) (330)) / ((uint16_t) (300))) - (int32_t) *TS_CAL1);
	temp = temp * (int32_t)(130 - 30);
	temp = temp / (int32_t) (*TS_CAL2 - *TS_CAL1);
	temp = temp + 30;
	return temp;
}


//3)External analog reading from a single channel
int32_t ADCSingleChannelReadout(void) {
	/*
	 * Below we are reading a single channel ADC value.
	 * This is merely a template to implement the ADC on a channel. The selected channel is channel 8 on PB0 (see table 15 in the datasheet).
	 * The function can only be tested with an analogue input on PB0 - say by pulling the pin LOW or HIGH.
	 *
	 * */

	int32_t channel_input;

	RCC->IOPENR |= (1<<1);										//PORTB clocking allowed
	GPIOB->MODER &= (3<<0);										//MODER is reset to 'b11. Here we want analog mode so technically no need to change anything.
																//OTYPER remains reset (push-pull), speed can stay at reset value low, no pull resistors,

	ADC1->CR |= 0x0;											//we clean the control register
																//Note: we need to use ADC1, not ADC, to comply to the general naming of the register (despite only having one ADC).

	ADC1->CHSELR |= (1<<8);										//we enable the chosen channel
																//Note: we select only one channel!
	ADC1->SMPR |= (7<<0);										//we keep as high of SMP as possible to cope with a variety of sensor input
	ADC1->CR |= (1<<2);											//we start conversion
	while((ADC1->ISR & (1<<2)) == 0);							//when the EOC goes HIGH, we go on
	channel_input = ADC1->DR;									//this reads out the data and clears the EOC flag
	return channel_input;										//we return the raw readout value
}
