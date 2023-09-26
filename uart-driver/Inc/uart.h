#pragma once

#include "stdint.h"

/** Whether to create a buffer for asynchronously storing incoming characters on USART2 */
#define     RX2_ENABLE_ASYNC    1
/** Whether to create a buffer for asynchronously storing incoming characters on USART1 */
#define     RX1_ENABLE_ASYNC    1
/** Whether to create a buffer for asynchronously storing incoming characters on USART6 */
#define     RX6_ENABLE_ASYNC    1

/** Whether to create a buffer to asynchronously transmit characters from on USART2 */
#define     TX2_ENABLE_ASYNC    1
/** Whether to create a buffer to asynchronously transmit characters from on USART1 */
#define     TX1_ENABLE_ASYNC    1
/** Whether to create a buffer to asynchronously transmit characters from on USART6 */
#define     TX6_ENABLE_ASYNC    1

/** Length of the buffer that holds incoming characters on the USART peripherals */
#define     __USART_RX_BUF_LEN  (1024)
/** Length of the buffer that holds outgoing characters from the USART peripherals */
#define     __USART_TX_BUF_LEN  (1024)

/** Maximum number of characters to read from the stream to empty it */
#define     USART_STREAM_FULL   USART_RX_BUF_LEN

#define     __USART_PIN_A2      (0)
#define     __USART_PIN_A3      (1)
#define     __USART_PIN_D5      (2)
#define     __USART_PIN_D6      (3)
#define     __USART_PIN_A9      (4)
#define     __USART_PIN_A10     (5)
#define     __USART_PIN_B6      (6)
#define     __USART_PIN_B7      (7)
#define     __USART_PIN_C6      (8)
#define     __USART_PIN_C7      (9)
#define     __USART_PIN_A11     (10)
#define     __USART_PIN_A12     (11)

/**
 * @brief               Available USART Peripherals on the processor
 *
 */
typedef enum {
    USART_PERIPH_2,
    USART_PERIPH_1,
    USART_PERIPH_6,
} Usart_t;

/**
 * @brief               Types of communication that the USART can perform
 *
 */
typedef enum {
    /** Communication only involving transmission of messages */
    USART_TX_ONLY   = 0b01,
    /** Communication only involving receival of messages */
    USART_RX_ONLY   = 0b10,
    /** Communication involving both transmission and receival of messages */
    USART_RXTX      = 0b11,
} Usart_Comm_t;

/**
 * @brief               Possible pairs of TX and RX pins for the available USART peripherals
 *
 */
typedef enum {
    /** PA2 as TX and PA3 as RX for USART2 */
    USART2_PA2_PA3  = (1U << __USART_PIN_A2) | (1U << __USART_PIN_A3),
    /** PA2 as TX and PD6 as RX for USART2 */
    USART2_PA2_PD6  = (1U << __USART_PIN_A2) | (1U << __USART_PIN_D6),
    /** PD5 as TX and PA3 as RX for USART2 */
    USART2_PD5_PA3  = (1U << __USART_PIN_D5) | (1U << __USART_PIN_A3),
    /** PD5 as TX and PD6 as RX for USART2 */
    USART2_PD5_PD6  = (1U << __USART_PIN_D5) | (1U << __USART_PIN_D6),

    /** PA8 as TX and PA10 as RX for USART1 */
    USART1_PA9_PA10 = (1U << __USART_PIN_A9) | (1U << __USART_PIN_A10),
    /** PA9 as TX and PB7 as RX for USART1 */
    USART1_PA9_PB7  = (1U << __USART_PIN_A9) | (1U << __USART_PIN_B7),
    /** PB6 as TX and PA10 as RX for USART1 */
    USART1_PB6_PA10 = (1U << __USART_PIN_B6) | (1U << __USART_PIN_A10),
    /** PB6 as TX and PB7 as RX for USART1 */
    USART1_PB6_PB7  = (1U << __USART_PIN_B6) | (1U << __USART_PIN_B7),

    /** PC6 as TX and PC7 as RX for USART6 */
    USART6_PC6_PC7  = (1U << __USART_PIN_C6) | (1U << __USART_PIN_C7),
    /** PC6 as TX and PA12 as RX for USART6 */
    USART6_PC6_PA12 = (1U << __USART_PIN_C6) | (1U << __USART_PIN_A12),
    /** PA11 as TX and PC7 as RX for USART6 */
    USART6_PA11_PC7 = (1U << __USART_PIN_A11) | (1U << __USART_PIN_C7),
    /** PA11 as TX and PA12 as RX for USART6 */
    USART6_PA11_PA12= (1U << __USART_PIN_A11) | (1U << __USART_PIN_A12)
} Usart_Pin_t;


/**
 * @brief               Enable clock access to the specified USART Peripheral from the RCC Peripheral
 *
 * @param pUart         The USART Peripheral to whom clock access must be enabled
 */
void        USARTEnableClockAccess(Usart_t pUart);

/**
 * @brief               Set the specified pair of pins as TX and RX for the corresponding USART Peripheral
 *
 * @param pPins         The pair of pins to be used as TX and RX
 */
void        USARTSetPin(Usart_Pin_t pPins);

/**
 * @brief               Set the baud (bit-rate) of the specified USART Peripheral
 *
 * @param pUart         The USART peripheral whose baud is to be set
 * @param pFreq         The frequency of the core clock in Hz (16 MHz if the internal oscillator is being used)
 * @param pBaud         The bit-rate used for communication
 */
void        USARTSetBaud(Usart_t pUart, const uint32_t pFreq, const uint32_t pBaud);

/**
 * @brief               Initialize the type of communication used by a USART peripheral
 *
 * @param pUart         The USART peripheral whose communication type is to be set
 * @param pUartComm     The type of communication that should occour (TX only, RX only or both)
 */
void        USARTCommEnable(Usart_t pUart, Usart_Comm_t pUartComm);

/**
 * @brief               Enable the specified USART (begin communication on the peripheral)
 *
 * @param pUart         The USART Peripheral on which to begin communication
 */
void        USARTPeriphEnable(Usart_t pUart);


/**
 * @brief               Disable the specified USART (stop communication on the peripheral)
 *
 * @param pUart         The USART Peripheral on which to cease communication
 */
void        USARTPeriphDisable(Usart_t pUart);

/**
 * @brief               Enables the callback function for when break characters are detected for the specified USART
 *
 * @note                This function requires global interrupts to be enabled (by calling the __enable_irq() function)
 * @note                When this function is called, the USARTLbITCallback function is called whenever a break character is detected,
 *                      and the software sequence to clear the flag must be provided in the callback function
 *
 * @param pUart         The USART Peripheral on which to enable the callback function
 */
void        USARTEnableLbCallback(Usart_t pUart);

/**
 * @brief               Enables the callback function for parity errors are detected for the specified USART
 *
 * @note                This function requires global interrupts to be enabled (by calling the __enable_irq() function)
 * @note                When this function is called, the USARTPeITCallback function is called whenever a parity error is detected,
 *                      and the software sequence to clear the flag must be provided in the callback function
 *
 * @param pUart         The USART Peripheral on which to enable the callback function
 */
void        USARTEnablePeCallback(Usart_t pUart);

/**
 * @brief               Enables the callback function for when a character is ready to read from the specified USART (or when an overrun error occurs)
 *
 * @note                This function requires global interrupts to be enabled (by calling the __enable_irq() function)
 * @note                When this function is called, the USARTRxITCallback function is called whenever a character is ready to be read,
 *                      and the software sequence to clear the flag must be provided in the callback function
 * @note                This function must be called to enable asynchronously reading the USART
 *
 * @param pUart         The USART peripheral on which to enable the callback function
 */
void        USARTEnableRxCallback(Usart_t pUart);

/**
 * @brief               Enables the callback function for when a character is ready to be transmitted on the specified USART
 *
 * @param pUart         The USART peripheral on which to enable the callback function
 */
void        USARTEnableTxCallback(Usart_t pUart);

/**
 * @brief               Disables the callback function for when break characters are detected for the specified USART
 *
 * @param pUart         The USART peripheral on which to disable the callback function
 */
void        USARTDisableLbCallback(Usart_t pUart);

/**
 * @brief               Disables the callback function for when parity errors are detected on the specified USART
 *
 * @param pUart         The USART peripheral on which to disable the callback function
 */
void        USARTDisablePeCallback(Usart_t pUart);

/**
 * @brief               Disables the callback function for when a character is ready to be read from the specified USART (or when an overrun errors occurs)
 *
 * @param pUart         The USART Peripheral on which to disable the callback function
 */
void        USARTDisableRxCallback(Usart_t pUart);

/**
 * @brief               Disables the callback function for when a character is ready to be transmitted on the specified USART
 *
 * @param pUart         The USART Peripheral on which to disable the callback function
 */
void        USARTDisableTxCallback(Usart_t pUart);

/**
 * @brief               Read the specified number of characters from the specified USART Peripheral into a buffer (blocking)
 *
 *                      This function blocks the program execution until the specified number of characters have been read
 *
 * @param pUart         The USART peripheral from which to read characters
 * @param pBuf          The buffer into which the characters should be read
 * @param pCount        The number of characters to read
 */
void        USARTRecvBufBlocking(Usart_t pUart, uint8_t *pBuf, uint32_t pCount);

/**
 * @brief               Send the specified number of characters on the specified USART Peripheral from a buffer (blocks execution till the exact number of characters are sent)
 *
 * @param pUart         The USART peripheral on which to transmit characters
 * @param pBuf          The buffer from which to send characters
 * @param pCount        The number of characters to transmit
 */
void        USARTSendBufBlocking(Usart_t pUart, uint8_t *pBuf, uint32_t pCount);

/**
 * @brief               Read a maximum number of characters from the specified USART Peripheral into a buffer
 *
 *                      If the specified number of characters have not been received on the USART since the last read,
 *                      the function does not block, and returns the number of characters that were available and could be read
 *
 * @note                The USARTEnableRxCallback function must be called before this function is called to enable asynchronously reading from the USART
 *
 * @param pUart         The USART peripheral from which to read characters
 * @param pBuf          The buffer into which the characters should be read
 * @param pCount        THe maximum number of characters to read from the stream
 *
 * @return uint32_t     The number of characters that could be read from the stream without blocking
 */
uint32_t    USARTRecvBuf(Usart_t pUart, uint8_t *pBuf, uint32_t pCount);

/**
 * @brief               Send the specified number of characters on the specified USART Peripheral from a buffer (does not block execution)
 *
 *                      The function copies specified number of characters from the buffer into an internal queue, and transmits them
 *                      on the USART asynchronously and from the rest of the program, which is executed concurrently
 *
 * @param pUart         The USART peripheral on which to transmit characters
 * @param pBuf          The buffer from which to send characters
 * @param pCount        The number of characters to transmit
 */
void        USARTSendBuf(Usart_t pUart, uint8_t *pBuf, uint32_t pCount);

/**
 * @brief               Sends the break character on the specified USART peripheral
 *
 * @param pUart         The USART Peripheral on which to transmit the break character
 */
void        USARTSendBreak(Usart_t pUart);

/**
 * @brief               Callback function that is called when an overrun error occurs
 *
 * @note                This function must be defined by the user, and is only called after the USARTEnableOvIT function is called for the same USART
 * @note                If not defined, this function disables all interrupts and blocks further program execution (default panic behaviour)
 *
 * @param pUart         The USART peripheral on which the error occured
 */
void        USARTOvITCallback(Usart_t pUart);

/**
 * @brief               Callback function that is called when a break character is detected
 *
 * @note                This function must be defined by the user, and is only called after the USARTEnableLbIT function is called for the same USART
 * @note                If not defined, this function disables all interrupts and blocks further program execution (default panic behaviour)
 *
 * @param pUart         The USART Peripheral on which the break character was detected
 */
void        USARTLbITCallback(Usart_t pUart);

/**
 * @brief               Callback function that is called when a parity error occurs
 *
 * @note                This function must be defined by the user, and is only called after the USARTEnablePeIT function is called for the same USART
 * @note                If not defined deliberately, this function disables all interrupts and blocks further program execution (default panic behaviour)
 *
 * @param pUart         The USART Peripheral on which the parity error occured
 * @param pC            The character that was received (undefined)
 */
void        USARTPeITCallback(Usart_t pUart, uint8_t pC);

/**
 * @brief               Callback function that is called when a byte is available to read from the USART
 *
 * @note                This function must be defined by the user, and is only called after the USARTEnableRxCallback function is called for the same USART
 *
 * @param pUart         The USART Peripheral on which the byte is available to be read from
 * @param pC            The character that was available on the USART
 */
void        USARTRxITCallback(Usart_t pUart, uint8_t pC);

/**
 * @brief               Callback function that is called when the USART is ready to transmit a character
 *
 * @note                This function must be defined by the user, and is only called after the USARTEnableTxIT function is called for the same USART
 *
 * @note                It is not recommended to use this function along with the USARTSendBuf method, and it only executes when all characters
 *                      from previous calls to USARTSendBuf have been consumed
 *
 * @param pUart         The USART Peripheral which is ready for transmission
 */
void        USARTTxITCallback(Usart_t pUart);
