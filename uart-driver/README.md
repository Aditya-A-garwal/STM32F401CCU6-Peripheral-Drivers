# STM32F401CCU6 USART-Driver

This is a USART-Driver for the STM32F401CCU6 microcontroller. For build and usage instructions, refer to the README at the root of this repository.

## Features Of the Driver

ARM Cortex M4 microcontrollers (including the STM32F401CCU6) contain USART Ports, each capable of transmitting and receiving data. The complete configuration of the USART as well as the state of its pins can be controlled through MMRs (refer to the README at the root of this repository for more information).

The driver provides lightweight functions to allow configuring the following for each USART peripheral -

- Simplex (RX Only and TX Only) and Duplex (RX and TX) communication.
- Baudrate (bitrate) of communication.
- Callback functions for the different interrupt events -
    - Overrun Error
    - Parity Error
    - Line Break Detected
    - Received Data Ready to be Read
    - Transmit Data Register Empty

The driver also provides _synchronous_ (blocking) and _asynchronous_ (non-blocking) functions to read and write data from each USART port. This is achieved by using circular buffers.


## Synchronous IO

To perform synchronous IO, no special steps have to be taken, and the USART peripheral may be normally initialized, configured and used. Using synchronous IO has the following implications -

- No extra memory is used by an internal buffer for transmission, but long transmissions can block program execution, causing important deadlines to potentially be missed.
- No extra memory is used by an internal buffer for receivals, but characters sent to the USART can be missed if the function is not called at the right time. **Additionally, the program can completely hang if the USART periperhal does not receive the exact number of characters requested by the caller.**

Synchronous IO functions are available to be used at all times, irrespective of whether the asynchronous IO functions are enabled or not. **However, it is not a good idea to mix synchronous IO with asynchronous IO for the same USART Peripheral, as this can lead to missing characters and overrun errors.**

## Asynchronous IO

To perform asynchronous IO, specific preprocessor macros have to be defined in the ```Inc/uart.h``` file. For choosing this, the driver allows granularity at the level of the USART Peripheral, as well as the direction in which to enable async communication.

The list of macros to be defined are shown below -

|Macro Name|Purpose|
|-|-|
|RX2_ENABLE_ASYNC|Enables consumption and recieving of characters to be asynchronous on USART2|
|RX1_ENABLE_ASYNC|Enables consumption and recieving of characters to be asynchronous on USART1|
|RX6_ENABLE_ASYNC|Enables consumption and recieving of characters to be asynchronous on USART6|
|TX2_ENABLE_ASYNC|Enables queuing and transmission of characters to be asynchronous on USART2|
|TX1_ENABLE_ASYNC|Enables queuing and transmission of characters to be asynchronous on USART1|
|TX6_ENABLE_ASYNC|Enables queuing and transmission of characters to be asynchronous on USART6|

Defining each macro creates a buffer for the specific USART for the specific direction of communication, which holds characters that have been queued/received, but not transmitted/consumed yet. **Each of these macros is defined by default.**

The size of the buffers are defined in the ```Inc/uart.h``` file. The size of the RX buffers is determined by the ```__USART_RX_BUF_LEN``` macro, while that of the TX buffers is determined by the ```__USART_TX_BUF_LEN``` macro. **The length of these buffers must be a power of 2.** The default values of both these macros is 1024.

It is important to make sure that these buffers are adequately large for your application. **The RX buffer for a USART must be large enough to store all characters between two consecutive reads.** Failing this will cause new characters to overwrite old characters in the buffer before they get consumed. **The TX buffer for a USART must be large enough to hold all characters that can be queued at a time without being transmitted.** Failing this, certain old characters may get overwritten by new ones before they are transmitted.

Asynchronous IO also requires global interrupts to enabled using the ```__enable_irq()``` function. Failing to call this function, or calling the ```__disable_irq()``` function will cause it to stop working.

## Callback Functions and Interrupts

The library declares callback functions for interrupts caused by the following errors/events. Each callback can be individually enabled, and it is the responsibility of the application to define/implement these functions, failing which the following default behaviors will be applied.

|Event/Error|Cause of occourence|Callback Function Name|Default Behavior|
|-|-|-|-|
|Overrun Error|A character became ready to be read while the previous was not consumed.|```USARTOvITCallback```|Disable all interrupts and permanently block program execution.|
|Parity Error|Received data does not match the parity specified (even by default).|```USARTPeITCallback```|Disable all interrupts and permanently block program execution.|
|Line Break Detected|Line break character was received on the USART|```USARTLbITCallback```|Disable all interrupts and permanently block program execution.|
|Received Data Ready|A character became ready to be read|```USARTRxITCallback```|Do nothing.|
|Data Ready For Transmission|A character can be transmitted over the USART|```USARTTxITCallback```|Do nothing.|

For callback functions to work, they must be enabled by their respective enable function. Additionally, they require global interrupts to be enabled using the ```__enable_irq()``` function. Failing to call this function, or calling the ```__disable_irq()``` function will cause it to stop working.

## Demonstration Program

The included demonstration program (```Src/main.c```) is a simple "echo" program, which echoes back whatever is sent on USART2 at a baudrate of 115200. The USART uses pins PA2 and PA3 as TX and RX respectively.

## Summary Of Functions And Their Purpose

Functions for USART initialization -

|Function Name|Purpose|
|-|-|
|```USARTEnableClockAccess```|Enable clock access to a USART Peripheral|
|```USARTSetPin```|Set the TX and RX pins for a USART Peripheral|
|```USARTSetBaud```|Set the baudrate for a USART Peripheral|
|```USARTCommEnable```|Enable RX/TX/TXRX communication on a USART Peripheral|
|```USARTPeriphEnable```|Enable communication on a USART Peripheral|
|```USARTPeriphDisable```|Disable communication on a USART Peripheral|

Functions for synchronous IO -

|Function Name|Purpose|
|-|-|
|```USARTRecvBufBlocking```|Recieve an exact number of characters over a USART into a buffer (blocking)|
|```USARTSendBufBlocking```|Transmit an exact number of characters from a buffer over a USART (blocking)|
|```USARTSendBreak```|Transmit a break character over a USART|

Functions for asynchronous IO -

|Function Name|Purpose|
|-|-|
|```USARTRecvBuf```|Recieve a maximum number of characters over a USART into a buffer (non-blocking)|
|```USARTSendBuf```|Transmit an exact number of characters from a buffer over a USART (non-blocking)|

Functions for enabling/disabling callbacks -

|Function Name|Purpose|
|-|-|
|```USARTEnableLbCallback```|Enables the callback function for when a Line break character is detected.|
|```USARTEnablePeCallback```|Enables the callback function for when a Parity error occurs.|
|```USARTEnableRxCallback```|Enables the callback function for when a character is ready to be read.|
|```USARTEnableTxCallback```|Enables the callback function for when a character can be transmitted.|
|```USARTDisableLbCallback```|Disables the callback function for when a Line break character is detected.|
|```USARTDisablePeCallback```|Disables the callback function for when a Parity error occurs.|
|```USARTDisableRxCallback```|Disables the callback function for when a character is ready to be read.|
|```USARTDisableTxCallback```|Disables the callback function for when a character can be transmitted.|
