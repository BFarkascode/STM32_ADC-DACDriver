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
The ADC, as the name suggests, converts incoming analogue signals to digital signals. It does this by running on a clock of its own – that can be picked within its control registers and then prescaled – and then sample an incoming voltage comparing it with its own calibrated value.

Mind, the ADC in the L0x3 has altogether 19 different channels, that are either connected to different pins (see the Table 15 within the datasheet to find, where they are) or to internal sensors, such as the in-built temperature sensor or the reference voltage measurement.

In general, the ADC can be configured in a myriad of ways, depending on needs on speed, control and sensitivity. The resolution of the ADC, for instance, can be chosen between 12-bits down to 6-bits which of course defines the sensitivity of the ADC as well as the speed (the conversion time). I personally have not found a reason to not use 12-bit resolution though, albeit I haven’t sampled any fast signals yet. Similarly one can let the ADC run automatic by setting the AUTOOFF and the WAIT bits plus adjusting an appropriate hardware trigger to commence conversion or keep everything manual and control all activities through software (in the code, we will use software trigger but otherwise keep everything automatic to avoid unnecessary coding).

The ADC has interrupt and DMA capabilities to facilitate a better control. (We will use the ADC to showcase the capabilities of DMA within the DMADriver project.)

There is an internal voltage regulator within the ADC that practically liberates it from fluctuations between the mcu’s internal voltage levels. This provides a handy way to supervise the voltage levels within the mcu (there is a dedicated ADC channels to do this for us actually). Of note, this voltage regulator must be given enough time to activate before we use the ADC, all is done on a hardware level and we can just ignore it.

A software-controlled calibration needs to be done once for one type of operating condition though . This calibration will be kept even if the ADC is disabled or the internal voltage regulator is turned off. The calibration is lost upon forced reset of the peripheral and loss of power.

Regarding channel scanning, one can scan upwards or downwards regarding the numbering of the channels. One can also set continuous mode where the data will be flowing in through the channels all the time.

In the end, the time to get a data converted by the ADC will be the resolution-dependent minimum conversion time plus the sampling time multiplied by the oversampling time. All in all, the timing for the ADC is rather specifically set and must be followed (Table 63).


## To read
For the DAC, the following parts are "must reads":
- 15.3 and 15.4 DAC output buffer enable and DAC channel enable: what to enable
- 15.5.2 DAC channel conversion: explains, how the DAC works
- 15.5.3 DAC output voltage: the voltage is calculated as Vref x DOR / 4096 for 12 bit.
- 15.5.4 DAC trigger selection: we can trigger the DAC using multiple sources, though we stick with the software control below
- 15.10 DAC registers

Frankly, I am not sure what the different alignment loadings are for in 15.5.1. I suppose just convenience to manage 12 bits as 16 bits...?

All in all, it must be said that – similar to the UART – we have absolutely massive amount of calibration possibilities, most of them are only useful in very specific scenarios. As such, for a simple ADC conversion, I suggest reading the following:

- 14.3. ADC functional description: the entire section is important since it shows, how to control the ADC manually
- 14.4.2 Programmable resolution: very important Table 63 shows the timing that must be respected for an accurate conversion
- 14.4.4 End of conversion sequence: tells what flag to look for when ADC conversion is done (we will use automatic off control below though)
- 15.5.1 Data register and data alignment: how the data is emerging from the ADC_DR register
- 14.6.1 Wait mode conversion: ensures that the ADC will only do an additional conversion when the previous data has been dealt with/read out from the ADC 
- 14.6.2 Auto-off mode: explains AUTO-OFF and why it is useful to be set (shuts off the ADC when not needed)
- 14.9 Temperature sensor and internal reference voltage: how the internal sensors work and what they are doing
- 14.12 ADC registers

## Particularities

### On the DAC
The refman is wrong for the L0x3, it does not have two DACs (the datasheet is correct though). On the other hand, the L0x2 does have two...which is a good example of why one must be very clear over which mcu is being used.

We don't have dual mode for the DAC. We don't have a second channel on the L0x3.

The only DAC output is wired to only PA4. Funnily, when this GPIO is being set, one does not need to change anything. That is because within the L0xx devices, the MODER for the GPIO - the register that defines what the GPIO will be used for - is reset to 2'b11 or "analogue mode". Be VERY CAREFUL about this since it is very often not indicated that the reset value is flipped!!! I had a lot of difficult bughunting sessions before I learned to remember this...

DAC is a pretty straight forwards peripheral with pretty straight forwards controls. On an L0x3, there is only one DAC (contrary to what the refman says) and it is always outputting the voltage values to PA4 (or pin A2).

### On the ADC

The ADC MUST be calibrated before use. Calibration can only occur when the ADC is not measuring and disabled (thus before every ADC readjustment, we must be sure that the ADC is off).

The ADC usually uses a lot of power, thus turning it off – either by hand or by activating the AUTO-OFF - when not needed is highly recommended.

When using the automations like WAIT or AUTO-OFF, the control flags will be dealt with automatically and would not need any setting/resetting, except for the ADSTART bit (the start conversion). Mind, the ADC has an enable and a start conversion ADSTART bit, which are not the same!

The EOC flag – the end of conversion for a channel flag – can be used to time the code to process the incoming data when not using DMA (see the DMADriver project for more on that).

Data emerges in the ADC_DR register for all (!) ADC channels.

Overrun events can easily occur when multiple channels are being sampled. Unlike other peripherals though, here an overruns event will not stop the peripheral. It will simply go on. We can only choose, what happens with the data (overwrite or discard).
The number of how many times the input voltage on the channel is sampled (oversampling) can be set as well as how long one sampling must last. This latter is particularly important and is usually a time window specifically given to a sensor: we have to wait this long that the incoming voltage from the source becomes stabile enough to be sampled.

The problem is that if the ADC is NOT clocked well or the sampling time is not respected, it will still give an output, but it will not be accurate. This can be dangerous. Consult the timings section in the refman for the ADC to understand it better.

When picking external input for the ADC, pick the right channel-GPIO combination. Also, the GPIO MUST be set as an analog pin.

## User guide

Uncomment the appropriate sections to run the DAC, the aDC or both.

The DAC test of the example code is pretty simple: one needs to give the DAC output generation function an integer value and that value will be converted to a voltage value (use the equation in 15.5.3 to estimate, what that voltage will be). Within the #ifdef-s, two manually generated function example is also provided.

We can simply switch between 8-bit and 12-bit DAC by writing to either the DHR8R1 or the DHR12R1 input buffer.

If one uses the noise generation function instead of the standard, a noise can be added to the DAC output signal. There the second input of the function scales the noise from 0 to 15.

Lastly, a suggestion: get an oscilloscope to see the change in DAC output or decrease the update time to the point where you can capture it with a multimeter.

For the ADC, the particulairty lies in the calculations to convert the ADC raw data into the temperature. The code is taken directly from the example "A.8.17 Temperature computation code example". The detected value should slightly increase when touching the mcu with a finger.
