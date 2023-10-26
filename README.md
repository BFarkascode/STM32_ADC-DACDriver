# STM32_ADC-DACDriver
Drivers for the ADC and the DAC in STM32_L0x3.

ADC stands for analog-to-digital converter, while DAC stands for digital-to-analog converter.

I wanted to have a small project just to expand on the capabilities of the STM32_L0x3 I was using.

DAC has a wave generator to generate functions.

DAC PA4 resets to needed value due to MODER reset to 2'b11.

DOR output regsiter is what is converted to output. DOR is loaded from the DHR12R1 or DHR8R1 registers.

DAC is pretty simple and demands only a very few registers to be modified/updated.

DAC has DMA, but I don't see the immediate usecase for it.

Noise and tringle wave generation.
