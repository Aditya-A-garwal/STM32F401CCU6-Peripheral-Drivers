# STM32F401CCU6 GPIO-Driver

This is a GPIO-Driver for the STM32F401CCU6 microcontroller. For build and usage instructions, refer to the README at the root of this repository.

## Features Of the Driver

All ARM Cortex M4 microcontrollers (including the STM32F401CCU6) contain GPIO Ports, each with 16 GPIO Pins. The complete state of these pins can be controlled through MMRs (refer to the README at the root of this repository for more information).

The driver provides lightweight functions that can be used to do the following -

- Initialize a pin as an input/output.
- Specify whether a pin must use a pullup/pulldown resister
- Change the slew-rate (speed) of a pin
- Read the state of a pin
- Write to the state of a pin

## Demonstration Program

The included demonstration program (```Src/main.c```) is a simple "blinky" program, which toggles the builtin LED on the [Black Pill Board V1.2](https://stm32-base.org/boards/STM32F401CCU6-WeAct-Black-Pill-V1.2.html).

## Summary Of Functions And Their Purpose

|Function Name|Purpose|
|-|-|
|```GPIOEnableClockAccess```|Enable clock access to the specified GPIO Peripheral|
|```GPIOInitPinOutput```|Initialize a Pin as a general-purpose output|
|```GPIOInitPinInput```|Initialize a Pin as a general-purpose input|
|```GPIOInitPinOutputType```|Initialize whether an output Pin uses push-pull or open-drain control|
|```GPIOInitPinOutputSpeed```|Initialize the speed/slew-rate of a Pin|
|```GPIOInitPinInputType```|Initialize the pullup/pulldown/floating mode of an input Pin|
|```GPIOWrite```|Update the state of an output Pin|
|```GPIOWriteAtomic```|Atomically update the state of an output Pin|
|```GPIORead```|Read the state of an input Pin|

**The functions rely on enums to direct the caller towards passing arguments appropriately, without incurring run-time overhead. Passing improper values (such as numbers greater than 15 for pin numbers) will yield undefined behaviour.**
