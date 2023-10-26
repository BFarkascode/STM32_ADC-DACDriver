# STM32_ADC-DACDriver

Drivers for the ADC and the DAC in STM32_L0x3.

## General description
I have decided to have a project on the digital-analog converter (DAC) and the analog-digital converter (ADC) at the same time since, by their own, both of them are rather simple and straight forwards to implement. Nevertheless, it is good to have a driver for either, just in case we want to:
- detect an incoming analogue signal and digitalize it with the ADC
- or we want to provide a specific voltage value on a pin using the DAC

We will use the ADC as an input for DMA in the DMADriver project. (DAC also has DMA, but unlike for ADC, I don't see an immediate usecase for it.)

### DAC
On the onset, using a DAC seems to be the same as using pulsed-width modulation (PWM): both take in a number and then generate a voltage level proportionate to that value.

There is one massive difference though and that is how the voltages are being generated:
- PWM actually generates the voltage as an integral, meaning that it rapidly switches between max voltage (digital HIGH) and zero voltage (digital LOW). The ratio between the HIGH states and the LOW states will define, how much net voltage the PWM is generating. For instance, in a 3.3V logic mcu, 1.65 V PWM will look like a square wave on the oscilloscope with an amplitude of 3.3V and a duty cycle of 50% (that is, HIGH and LOW ratio will be 1:1)
- DAC sets the voltage level as an analogue value, meaning that a 1.65V DAC output will be a just a flat line at 1.65 V

### ADC
TBD.

## To read
For the ADC, the following parts are "must reads":

TBD.

And for the DAC:
- 15.3 and 15.4 DAC output buffer enable and DAC channel enable: what to enable
- 15.5.2 DAC channel conversion: explains, how the DAC works
- 15.5.3 DAC output voltage: the voltage is calculated as Vref x DOR / 4096 for 12 bit.
- 15.5.4 DAC trigger selection: we can trigger the DAC using multiple sources, though we stick with the software control below
- 15.10 DAC registers

Frankly, I am not sure what the different alignment loadings are for in 15.5.1. I suppose just convenience to manage 12 bits as 16 bits...?

## Particularities

### On the DAC
The refman is wrong for the L0x3, it does not have two DACs (the datasheet is correct though). On the other hand, the L0x2 does have two...which is a good example of why one must be very clear over which mcu is being used.

We don't have dual mode for the DAC. We don't have a second channel on the L0x3.

The only DAC output is wired to only PA4. Funnily, when this GPIO is being set, one does not need to change anything. That is because within the L0xx devices, the MODER for the GPIO - the register that defines what the GPIO will be used for - is reset to 2'b11 or "analogue mode". Be VERY CAREFUL about this since it is very often not indicated that the reset value is flipped!!! I had a lot of difficult bughunting sessions before I learned to remember this...

DAC is a pretty straight forwards peripheral with pretty straight forwards controls. On an L0x3, there is only one DAC (contrary to what the refman says) and it is always outputting the voltage values to PA4 (or pin A2).


## User guide

The DAC test of the example code is pretty simple: one needs to add give the DAC output generation function an integer value and that value will be converted to a voltage value (use the equation in 15.5.3 to estimate, what that voltage will be). Within the #ifdef-s, two manually generated function example is also provided.

We can simply switch between 8-bit and 12-bit DAC by writing to either the DHR8R1 or the DHR12R1 input buffer.

If one uses the noise generation function instead of the standard, a noise can be added to the DAC output signal. There the second input of the function scales the noise from 0 to 15.

Lastly, a suggestion: get an oscilloscope to see the change in DAC output or decrease the update time to the point where you can capture it with a multimeter.
