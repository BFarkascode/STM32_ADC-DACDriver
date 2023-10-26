# STM32_ADC-DACDriver
Drivers for the ADC and the DAC in STM32_L0x3.

DAC is a pretty straight forwards peripheral with pretty straight forwards controls. On an L0x3, there is only one DAC (contrary to what the refman says) and it is always otuputting the voltage values to PA4 (or pin A2).

ADC stands for analog-to-digital converter, while DAC stands for digital-to-analog converter.

I wanted to have a small project just to expand on the capabilities of the STM32_L0x3 I was using.

DAC has a wave generator to generate functions.

DAC PA4 resets to needed value due to MODER reset to 2'b11.

DOR output regsiter is what is converted to output. DOR is loaded from the DHR12R1 or DHR8R1 registers.
- 15.3 and 15.4 DAC output buffer enable and DAC channel enable: what to enable
- 15.5.2 DAC channel convertion: explains, how the DAC works
- 15.5.3 DAC output voltage: the voltage is calculated as Vref x DOR / 4096 for 12 bit.
- 15.5.4 DAC trigger selection: we cna trigger the DAC using multiple sources, though we stick with the software control below
- 15.10 DAC registers

I am not sure what the different alignment loadings are for in 15.5.1. I suppose just convenience to manage 12 bits as 16 bits...

DAC is pretty simple and demands only a very few registers to be modified/updated.

DAC has DMA, but I don't see the immediate usecase for it.

Noise and tringle wave generation.
