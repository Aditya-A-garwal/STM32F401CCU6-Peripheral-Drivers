#include "stm32f4xx.h"
#include "uart.h"

#if defined(__USART_RX_BUF_LEN) && ((__USART_RX_BUF_LEN & 3) != 0)
#error "Length of RX Buffer not word aligned (not divisible by 4)"
#endif

#if defined(__USART_TX_BUF_LEN) && ((__USART_TX_BUF_LEN & 3) != 0)
#error "Length of TX Buffer not word aligned (not divisible by 4)"
#endif

/** Position of USART2 Clock Enable Bit */
#define     RCC_APB1ENR_USART2ENn (17)
/** Position of USART1 Clock Enable Bit */
#define     RCC_APB2ENR_USART1ENn (4)
/** Position of USART6 Clock Enable Bit */
#define     RCC_APB2ENR_USART6ENn (5)

/** Position of CTS detection bit (cleared directly by software) */
#define     USART_SR_CTSn       (9)
/** Position of LIN break detection bit (cleared directly by software) */
#define     USART_SR_LBDn       (8)
/** Position of Transmit Data register empty bit (cleared by writing to DR) */
#define     USART_SR_TXEn       (7)
/** Position of Transmission complete bit (cleared by reading from SR followed by writing to DR, or directly from software) */
#define     USART_SR_TCn        (6)
/** Position of Read data available bit (cleared by reading from DR, or directly from software) */
#define     USART_SR_RXNEn      (5)
/** Position of IDLE line detected bit (cleared by reading from SR followed by reading from DR) */
#define     USART_SR_IDLEn      (4)
/** Position of Overrun error bit (cleared by reading from SR followed by reading from DR) */
#define     USART_SR_OREn       (3)
/** Position of Noise Detected bit (cleared by reading from SR followed by reading from DR) */
#define     USART_SR_NFn        (2)
/** Position of Framing Error bit (cleared by reading from SR followed by reading from DR) */
#define     USART_SR_FEn        (1)
/** Position of Parity Error bit (cleared by reading from SR, followed by reading/writing from DR) */
#define     USART_SR_PEn        (0)

/** Position of Oversampling mode bit */
#define     USART_CR1_OVER8n    (15)
/** Position of USART Enable bit */
#define     USART_CR1_UEn       (13)
/** Position of Word length bit */
#define     USART_CR1_Mn        (12)
/** Position of Wakeup method bit */
#define     USART_CR1_WAKEn     (11)
/** Position of Parity Control Enable bit */
#define     USART_CR1_PCEn      (10)
/** Position of Parity Selection bit */
#define     USART_CR1_PSn       (9)
/** Position of Parity Error Interrupt Enable bit */
#define     USART_CR1_PEIEn     (8)
/** Position of TXE Interrupt Enable bit */
#define     USART_CR1_TXEIEn    (7)
/** Position of Transmission Complete Interrupt Enable bit */
#define     USART_CR1_TCIEn     (6)
/** Position of RXNE Interrupt Enable bit */
#define     USART_CR1_RXNEIEn   (5)
/** Position of IDLE Interrupt Enable bit */
#define     USART_CR1_IDLEIEn   (4)
/** Position of Transmitter Enable bit */
#define     USART_CR1_TEn       (3)
/** Position of Receiver Enable bit */
#define     USART_CR1_REn       (2)
/** Position of Receiver wakeup (mute mode or not) bit */
#define     USART_CR1_RWUn      (1)
/** Position of Send Break Character bit */
#define     USART_CR1_SBKn      (0)

/** Position of LIN break detection Interrupt Enable bit */
#define     USART_CR2_LBDIEn    (6)

/** Helper macro to set the bit at the specified position */
#define     USART_SET_BIT(v, i) (v) |= 1U << (i)
/** Helper macro to clear the bit at the specified position */
#define     USART_CLR_BIT(v, i) (v) &= ~(1U << (i))
/** Helper macro to get the bit at the specified index */
#define     USART_GET_BIT(v, i) (v & (1U << (i)))


#if defined(RX2_ENABLE_ASYNC)
/** Circular Buffer to asynchronously store characters as they arrive on the USART2 peripheral (must be large enough to hold all characters) */
static volatile uint8_t     rx2_buf[__USART_RX_BUF_LEN];
/** Next vacant position in the RX buffer for USART2 (this is where the next character will be placed) */
static volatile uint32_t    rx2_next_dst = 0;
/** Next occupied  position in the RX buffer for USART2 (this is where the next character will be consumed from) */
static volatile uint32_t    rx2_next_src = 0;
#endif

#if defined(RX1_ENABLE_ASYNC)
/** Circular Buffer to asynchronously store characters as they arrive on the USART1 peripheral (must be large enough to hold all characters) */
static volatile uint8_t     rx1_buf[__USART_RX_BUF_LEN];
/** Next vacant position in the RX buffer for USART1 (this is where the next character will be placed) */
static volatile uint32_t    rx1_next_dst = 0;
/** Next occupied  position in the RX buffer for USART1 (this is where the next character will be consumed from) */
static volatile uint32_t    rx1_next_src = 0;
#endif

#if defined(RX6_ENABLE_ASYNC)
/** Circular Buffer to asynchronously store characters as they arrive on the USART6 peripheral (must be large enough to hold all characters) */
static volatile uint8_t     rx6_buf[__USART_RX_BUF_LEN];
/** Next vacant position in the RX buffer for USART6 (this is where the next character will be placed) */
static volatile uint32_t    rx6_next_dst = 0;
/** Next occupied  position in the RX buffer for USART6 (this is where the next character will be consumed from) */
static volatile uint32_t    rx6_next_src = 0;
#endif

#if defined(TX2_ENABLE_ASYNC)
/** Circular Buffer to store characters that must be asynchronously transmitted from the USART2 peripheral */
static volatile uint8_t     tx2_buf[__USART_TX_BUF_LEN];
/** Next vacant position in the TX Buffer for USART2 (this is where the next character will be placed) */
static volatile uint32_t    tx2_next_dst = 0;
/** Next occupied position in the TX Buffer for USART2 (this is where the next character will be consumed from) */
static volatile uint32_t    tx2_next_src = 0;
#endif

#if defined(TX1_ENABLE_ASYNC)
/** Circular Buffer to store characters that must be asynchronously transmitted from the USART1 peripheral */
static volatile uint8_t     tx1_buf[__USART_TX_BUF_LEN];
/** Next vacant position in the TX Buffer for USART1 (this is where the next character will be placed) */
static volatile uint32_t    tx1_next_dst = 0;
/** Next occupied position in the TX Buffer for USART1 (this is where the next character will be consumed from) */
static volatile uint32_t    tx1_next_src = 0;
#endif

#if defined(TX6_ENABLE_ASYNC)
/** Circular Buffer to store characters that must be asynchronously transmitted from the USART6 peripheral */
static volatile uint8_t     tx6_buf[__USART_TX_BUF_LEN];
/** Next vacant position in the TX Buffer for USART6 (this is where the next character will be placed) */
static volatile uint32_t    tx6_next_dst = 0;
/** Next occupied position in the TX Buffer for USART6 (this is where the next character will be consumed from) */
static volatile uint32_t    tx6_next_src = 0;
#endif


void
USARTEnableClockAccess(Usart_t pUart) {

    switch (pUart) {

        case USART_PERIPH_2:
            USART_SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2ENn);
            break;

        case USART_PERIPH_1:
            USART_SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1ENn);
            break;

        case USART_PERIPH_6:
            USART_SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART6ENn);
            break;
    }
}

void
USARTSetPin(Usart_Pin_t pPins) {

    // For setting a pin as RX/TX for USART2, the sequence is -
    // 1. Enable the clock for the specific GPIO Port that the pin belongs to (set flag in RCC_AHB1ENR)
    // 2. Set the Mode of the Pin to "Alternate Function" (set 0x2 at position 2*p in GPIO_MODER register)
    // 3. Set the alternate function to USART2 (set 0x7 at position 4*p in GPIO_AFR_low register)
    // If the pin is >= 8, instead use the AFR_high register and subtract 8 from the pin value

    // For setting a pin as RX/TX for USART1, the sequence is -
    // 1. Enable the clock for the specific GPIO Port that the pin belongs to (set flag in RCC_AHB1ENR)
    // 2. Set the Mode of the Pin to "Alternate Function" (set 0x2 at position 2*p in GPIO_MODER register)
    // 3. Set the alternate function to USART1 (set 0x8 at position 4*p in GPIO_AFR_low register)
    // If the pin is >= 8, instead use the AFR_high register and subtract 8 from the pin value

    // For setting a pin as RX/TX for USART2, the sequence is -
    // 1. Enable the clock for the specific GPIO Port that the pin belongs to (set flag in RCC_AHB1ENR)
    // 2. Set the Mode of the Pin to "Alternate Function" (set 0x2 at position 2*p in GPIO_MODER register)
    // 3. Set the alternate function to USART6 (set 0x8 at position 4*p in GPIO_AFR_low register)
    // If the pin is >= 8, instead use the AFR_high register and subtract 8 from the pin value

    // usart2 tx
    if (USART_GET_BIT(pPins, __USART_PIN_A2)) {

        USART_SET_BIT(RCC->AHB1ENR, 0);

        USART_CLR_BIT(GPIOA->MODER, (2 * 2) + 0);
        USART_SET_BIT(GPIOA->MODER, (2 * 2) + 1);

        USART_SET_BIT(GPIOA->AFR[0], (4 * 2) + 0);
        USART_SET_BIT(GPIOA->AFR[0], (4 * 2) + 1);
        USART_SET_BIT(GPIOA->AFR[0], (4 * 2) + 2);
        USART_CLR_BIT(GPIOA->AFR[0], (4 * 2) + 3);
    }

    // usart2 rx
    if (USART_GET_BIT(pPins, __USART_PIN_A3)) {

        USART_SET_BIT(RCC->AHB1ENR, 0);

        USART_CLR_BIT(GPIOA->MODER, (2 * 3) + 0);
        USART_SET_BIT(GPIOA->MODER, (2 * 3) + 1);

        USART_SET_BIT(GPIOA->AFR[0], (4 * 3) + 0);
        USART_SET_BIT(GPIOA->AFR[0], (4 * 3) + 1);
        USART_SET_BIT(GPIOA->AFR[0], (4 * 3) + 2);
        USART_CLR_BIT(GPIOA->AFR[0], (4 * 3) + 3);
    }

    // usart2 tx
    if (USART_GET_BIT(pPins, __USART_PIN_D5)) {

        USART_SET_BIT(RCC->AHB1ENR, 3);

        USART_CLR_BIT(GPIOD->MODER, (2 * 5) + 0);
        USART_SET_BIT(GPIOD->MODER, (2 * 5) + 1);

        USART_SET_BIT(GPIOD->AFR[0], (4 * 5) + 0);
        USART_SET_BIT(GPIOD->AFR[0], (4 * 5) + 1);
        USART_SET_BIT(GPIOD->AFR[0], (4 * 5) + 2);
        USART_CLR_BIT(GPIOD->AFR[0], (4 * 5) + 3);
    }

    // usart2 rx
    if (USART_GET_BIT(pPins, __USART_PIN_D6)) {

        USART_SET_BIT(RCC->AHB1ENR, 3);

        USART_CLR_BIT(GPIOD->MODER, (2 * 6) + 0);
        USART_SET_BIT(GPIOD->MODER, (2 * 6) + 1);

        USART_SET_BIT(GPIOD->AFR[0], (4 * 6) + 0);
        USART_SET_BIT(GPIOD->AFR[0], (4 * 6) + 1);
        USART_SET_BIT(GPIOD->AFR[0], (4 * 6) + 2);
        USART_CLR_BIT(GPIOD->AFR[0], (4 * 6) + 3);
    }

    // usart1 tx
    if (USART_GET_BIT(pPins, __USART_PIN_A9)) {

        USART_SET_BIT(RCC->AHB1ENR, 0);

        USART_CLR_BIT(GPIOA->MODER, (2 * 9) + 0);
        USART_SET_BIT(GPIOA->MODER, (2 * 9) + 1);

        USART_SET_BIT(GPIOA->AFR[1], (4 * 1) + 0);
        USART_SET_BIT(GPIOA->AFR[1], (4 * 1) + 1);
        USART_SET_BIT(GPIOA->AFR[1], (4 * 1) + 2);
        USART_CLR_BIT(GPIOA->AFR[1], (4 * 1) + 3);
    }

    // usart1 rx
    if (USART_GET_BIT(pPins, __USART_PIN_A10)) {

        USART_SET_BIT(RCC->AHB1ENR, 0);

        USART_CLR_BIT(GPIOA->MODER, (2 * 10) + 0);
        USART_SET_BIT(GPIOA->MODER, (2 * 10) + 1);

        USART_SET_BIT(GPIOA->AFR[1], (4 * 2) + 0);
        USART_SET_BIT(GPIOA->AFR[1], (4 * 2) + 1);
        USART_SET_BIT(GPIOA->AFR[1], (4 * 2) + 2);
        USART_CLR_BIT(GPIOA->AFR[1], (4 * 2) + 3);
    }

    // usart1 tx
    if (USART_GET_BIT(pPins, __USART_PIN_B6)) {

        USART_SET_BIT(RCC->AHB1ENR, 1);

        USART_CLR_BIT(GPIOB->MODER, (2 * 6) + 0);
        USART_SET_BIT(GPIOB->MODER, (2 * 6) + 1);

        USART_SET_BIT(GPIOB->AFR[0], (4 * 6) + 0);
        USART_SET_BIT(GPIOB->AFR[0], (4 * 6) + 1);
        USART_SET_BIT(GPIOB->AFR[0], (4 * 6) + 2);
        USART_CLR_BIT(GPIOB->AFR[0], (4 * 6) + 3);
    }

    // usart1 rx
    if (USART_GET_BIT(pPins, __USART_PIN_B7)) {

        USART_SET_BIT(RCC->AHB1ENR, 1);

        USART_CLR_BIT(GPIOB->MODER, (2 * 7) + 0);
        USART_SET_BIT(GPIOB->MODER, (2 * 7) + 1);

        USART_SET_BIT(GPIOB->AFR[0], (4 * 7) + 0);
        USART_SET_BIT(GPIOB->AFR[0], (4 * 7) + 1);
        USART_SET_BIT(GPIOB->AFR[0], (4 * 7) + 2);
        USART_CLR_BIT(GPIOB->AFR[0], (4 * 7) + 3);
    }

    // usart6 tx
    if (USART_GET_BIT(pPins, __USART_PIN_C6)) {

        USART_SET_BIT(RCC->AHB1ENR, 2);

        USART_CLR_BIT(GPIOC->MODER, (2 * 6) + 0);
        USART_SET_BIT(GPIOC->MODER, (2 * 6) + 1);

        USART_CLR_BIT(GPIOC->AFR[0], (4 * 6) + 0);
        USART_CLR_BIT(GPIOC->AFR[0], (4 * 6) + 1);
        USART_CLR_BIT(GPIOC->AFR[0], (4 * 6) + 2);
        USART_SET_BIT(GPIOC->AFR[0], (4 * 6) + 3);
    }

    // usart6 rx
    if (USART_GET_BIT(pPins, __USART_PIN_C7)) {

        USART_SET_BIT(RCC->AHB1ENR, 2);

        USART_CLR_BIT(GPIOC->MODER, (2 * 7) + 0);
        USART_SET_BIT(GPIOC->MODER, (2 * 7) + 1);

        USART_CLR_BIT(GPIOC->AFR[0], (4 * 7) + 0);
        USART_CLR_BIT(GPIOC->AFR[0], (4 * 7) + 1);
        USART_CLR_BIT(GPIOC->AFR[0], (4 * 7) + 2);
        USART_SET_BIT(GPIOC->AFR[0], (4 * 7) + 3);
    }

    // usart6 tx
    if (USART_GET_BIT(pPins, __USART_PIN_A11)) {

        USART_SET_BIT(RCC->AHB1ENR, 0);

        USART_CLR_BIT(GPIOA->MODER, (2 * 11) + 0);
        USART_SET_BIT(GPIOA->MODER, (2 * 11) + 1);

        USART_CLR_BIT(GPIOA->AFR[1], (4 * 3) + 0);
        USART_CLR_BIT(GPIOA->AFR[1], (4 * 3) + 1);
        USART_CLR_BIT(GPIOA->AFR[1], (4 * 3) + 2);
        USART_SET_BIT(GPIOA->AFR[1], (4 * 3) + 3);
    }

    // usart6 rx
    if (USART_GET_BIT(pPins, __USART_PIN_A12)) {

        USART_SET_BIT(RCC->AHB1ENR, 0);

        USART_CLR_BIT(GPIOA->MODER, (2 * 12) + 0);
        USART_SET_BIT(GPIOA->MODER, (2 * 12) + 1);

        USART_CLR_BIT(GPIOA->AFR[1], (4 * 4) + 0);
        USART_CLR_BIT(GPIOA->AFR[1], (4 * 4) + 1);
        USART_CLR_BIT(GPIOA->AFR[1], (4 * 4) + 2);
        USART_SET_BIT(GPIOA->AFR[1], (4 * 4) + 3);
    }
}

void
USARTSetBaud(Usart_t pUart, const uint32_t pFreq, const uint32_t pBaud) {

    // the baudrate of the USART is not directly stored within a register
    // the core clock frequency must be divided by the desired baud rate to get a "scale" for the clock
    // this is stored in the BRR register of the corresponding USART

    switch (pUart) {

        case USART_PERIPH_2:
            USART2->BRR = pFreq / pBaud;
            break;

        case USART_PERIPH_1:
            USART1->BRR = pFreq / pBaud;
            break;

        case USART_PERIPH_6:
            USART6->BRR = pFreq / pBaud;
            break;
    }
}

void
USARTCommEnable(Usart_t pUart, Usart_Comm_t pUartComm) {


    // to enable the USART to communicate in TX only mode, the TE flag must be set in CR1
    // to enable the USART to communicate in RX only mode, the RE flag must be set in CR1
    // to enable both, hoth flags must be set

    // The Usart_comm_t variants are bitmasks, removing the need to explicitly check for all 4 cases
    // The TX and RX can be seperately handled

    switch (pUart) {

        case USART_PERIPH_2:
            if (pUartComm & USART_TX_ONLY) {
                USART_SET_BIT(USART2->CR1, USART_CR1_TEn);
            }

            if (pUartComm & USART_RX_ONLY) {
                USART_SET_BIT(USART2->CR1, USART_CR1_REn);
            }
            break;

        case USART_PERIPH_1:
            if (pUartComm & USART_TX_ONLY) {
                USART_SET_BIT(USART1->CR1, USART_CR1_TEn);
            }

            if (pUartComm & USART_RX_ONLY) {
                USART_SET_BIT(USART1->CR1, USART_CR1_REn);
            }
            break;
        case USART_PERIPH_6:
            if (pUartComm & USART_TX_ONLY) {
                USART_SET_BIT(USART6->CR1, USART_CR1_TEn);
            }

            if (pUartComm & USART_RX_ONLY) {
                USART_SET_BIT(USART6->CR1, USART_CR1_REn);
            }
            break;
    }
}

void
USARTPeriphEnable(Usart_t pUart) {

    // To enable the USART Peripheral after configuration is complete, the UE bit flag must be set in CR1

    switch (pUart) {

        case USART_PERIPH_2:
            USART_SET_BIT(USART2->CR1, USART_CR1_UEn);
            break;

        case USART_PERIPH_1:
            USART_SET_BIT(USART1->CR1, USART_CR1_UEn);
            break;

        case USART_PERIPH_6:
            USART_SET_BIT(USART6->CR1, USART_CR1_UEn);
            break;
    }
}

void
USARTPeriphDisable(Usart_t pUart) {

    // to disable the USART Peripheral after configuration is complete, the UE bit flag must be cleared in CR1

    switch (pUart) {

        case USART_PERIPH_2:
            USART_CLR_BIT(USART2->CR1, USART_CR1_UEn);
            break;

        case USART_PERIPH_1:
            USART_CLR_BIT(USART1->CR1, USART_CR1_UEn);
            break;

        case USART_PERIPH_6:
            USART_CLR_BIT(USART6->CR1, USART_CR1_UEn);
            break;
    }
}

void
USARTEnableLbCallback(Usart_t pUart) {

    switch (pUart) {

        case USART_PERIPH_2:
            NVIC_EnableIRQ(USART2_IRQn);
            USART_SET_BIT(USART2->CR2, USART_CR2_LBDIEn);
            break;

        case USART_PERIPH_1:
            NVIC_EnableIRQ(USART1_IRQn);
            USART_SET_BIT(USART1->CR2, USART_CR2_LBDIEn);
            break;

        case USART_PERIPH_6:
            NVIC_EnableIRQ(USART6_IRQn);
            USART_SET_BIT(USART6->CR2, USART_CR2_LBDIEn);
            break;
    }
}

void
USARTEnablePeCallback(Usart_t pUart) {

    switch (pUart) {

        case USART_PERIPH_2:
            NVIC_EnableIRQ(USART2_IRQn);
            USART_SET_BIT(USART2->CR1, USART_CR1_PEIEn);
            break;

        case USART_PERIPH_1:
            NVIC_EnableIRQ(USART1_IRQn);
            USART_SET_BIT(USART1->CR1, USART_CR1_PEIEn);
            break;

        case USART_PERIPH_6:
            NVIC_EnableIRQ(USART6_IRQn);
            USART_SET_BIT(USART6->CR1, USART_CR1_PEIEn);
            break;
    }
}

void
USARTEnableRxCallback(Usart_t pUart) {

    switch (pUart) {

        case USART_PERIPH_2:
            NVIC_EnableIRQ(USART2_IRQn);
            USART_SET_BIT(USART2->CR1, USART_CR1_RXNEIEn);
            break;

        case USART_PERIPH_1:
            NVIC_EnableIRQ(USART1_IRQn);
            USART_SET_BIT(USART1->CR1, USART_CR1_RXNEIEn);
            break;

        case USART_PERIPH_6:
            NVIC_EnableIRQ(USART6_IRQn);
            USART_SET_BIT(USART6->CR1, USART_CR1_RXNEIEn);
            break;
    }
}

void
USAREnableTxCallback(Usart_t pUart) {

    switch (pUart) {

        case USART_PERIPH_2:
            NVIC_EnableIRQ(USART2_IRQn);
            USART_SET_BIT(USART2->CR1, USART_CR1_TEn);
            break;

        case USART_PERIPH_1:
            NVIC_EnableIRQ(USART1_IRQn);
            USART_SET_BIT(USART1->CR1, USART_CR1_TEn);
            break;

        case USART_PERIPH_6:
            NVIC_EnableIRQ(USART6_IRQn);
            USART_SET_BIT(USART6->CR1, USART_CR1_TEn);
            break;
    }
}

void
USARTDisableLbCallback(Usart_t pUart) {

    switch (pUart) {

        case USART_PERIPH_2:
            USART_CLR_BIT(USART2->CR2, USART_CR2_LBDIEn);
            break;

        case USART_PERIPH_1:
            USART_CLR_BIT(USART1->CR2, USART_CR2_LBDIEn);
            break;

        case USART_PERIPH_6:
            USART_CLR_BIT(USART6->CR2, USART_CR2_LBDIEn);
            break;
    }
}

void
USARTDisablePeCallback(Usart_t pUart) {

    switch (pUart) {

        case USART_PERIPH_2:
            USART_CLR_BIT(USART2->CR1, USART_CR1_PEIEn);
            break;

        case USART_PERIPH_1:
            USART_CLR_BIT(USART1->CR1, USART_CR1_PEIEn);
            break;

        case USART_PERIPH_6:
            USART_CLR_BIT(USART6->CR1, USART_CR1_PEIEn);
            break;
    }
}

void
USARTDisableRxCallback(Usart_t pUart) {

    switch (pUart) {

        case USART_PERIPH_2:
            USART_CLR_BIT(USART2->CR1, USART_CR1_RXNEIEn);
            break;

        case USART_PERIPH_1:
            USART_CLR_BIT(USART1->CR1, USART_CR1_RXNEIEn);
            break;

        case USART_PERIPH_6:
            USART_CLR_BIT(USART6->CR1, USART_CR1_RXNEIEn);
            break;
    }
}

void
USARTDisableTxCallback(Usart_t pUart) {

    switch (pUart) {

        case USART_PERIPH_2:
            USART_CLR_BIT(USART2->CR1, USART_CR1_TEn);
            break;

        case USART_PERIPH_1:
            USART_CLR_BIT(USART1->CR1, USART_CR1_TEn);
            break;

        case USART_PERIPH_6:
            USART_CLR_BIT(USART6->CR1, USART_CR1_TEn);
            break;
    }
}

void
USARTRecvBufBlocking(Usart_t pUart, uint8_t *pBuf, uint32_t pCount) {

    // Incoming characters from the USART are stored in the DR register,
    // A character is available to be read only when the RXNE flag is set in the SR

    switch (pUart) {

        case USART_PERIPH_2:
            for (uint8_t *src = pBuf; src != &pBuf[pCount]; ++src) {

                while (!USART_GET_BIT(USART2->SR, USART_SR_RXNEn));
                *src = USART2->DR;
            }
            break;

        case USART_PERIPH_1:
            for (uint8_t *src = pBuf; src != &pBuf[pCount]; ++src) {

                while (!USART_GET_BIT(USART1->SR, USART_SR_RXNEn));
                *src = USART1->DR;
            }
            break;

        case USART_PERIPH_6:
            for (uint8_t *src = pBuf; src != &pBuf[pCount]; ++src) {

                while (!USART_GET_BIT(USART6->SR, USART_SR_RXNEn));
                *src = USART6->DR;
            }
            break;
    }

}

void
USARTSendBufBlocking(Usart_t pUart, uint8_t *pBuf, uint32_t pCount) {

    // Outgoing characters from the USART are stored in the DR register,
    // A character is available to be transmitted only when the TXE flag is set in the SR

    switch (pUart) {

        case USART_PERIPH_2:
            for (uint8_t *src = pBuf; src != &pBuf[pCount]; ++src) {

                while (!USART_GET_BIT(USART2->SR, USART_SR_TXEn));
                USART2->DR = (uint8_t)*src;
            }
            break;

        case USART_PERIPH_1:
            for (uint8_t *src = pBuf; src != &pBuf[pCount]; ++src) {

                while (!USART_GET_BIT(USART1->SR, USART_SR_TXEn));
                USART1->DR = (uint8_t)*src;
            }
            break;

        case USART_PERIPH_6:
            for (uint8_t *src = pBuf; src != &pBuf[pCount]; ++src) {

                while (!USART_GET_BIT(USART6->SR, USART_SR_TXEn));
                USART6->DR = (uint8_t)*src;
            }
            break;
    }
}

uint32_t
USARTRecvBuf(Usart_t pUart, uint8_t *pBuf, uint32_t pCount) {

    // If Asynchronous RX is enabled, then incoming characters to the USART will be stored within a circular buffer
    // characters are consumed from this buffer one by one, until either the end of the buffer is reached (no more characters left to consume, not greatest address)
    // or the number of characters specified by the caller have been read already

    /** The next vacant position in the buffer where a character can be placed */
    uint8_t *src = pBuf;

    switch (pUart) {

#if defined(RX2_ENABLE_ASYNC)
        case USART_PERIPH_2:
            for (; rx2_next_src != rx2_next_dst && src != &pBuf[pCount];
                    ++src, rx2_next_src = (rx2_next_src + 1) & (__USART_RX_BUF_LEN - 1)) {
                *src = rx2_buf[rx2_next_src];
            }
            break;
#endif

#if defined(RX1_ENABLE_ASYNC)
        case USART_PERIPH_1:
            for (; rx1_next_src != rx1_next_dst && src != &pBuf[pCount];
                    ++src, rx1_next_src = (rx1_next_src + 1) & (__USART_RX_BUF_LEN - 1)) {
                *src = rx1_buf[rx1_next_src];
            }
            break;
#endif

#if defined(RX6_ENABLE_ASYNC)
        case USART_PERIPH_6:
            for (; rx6_next_src != rx6_next_dst && src != &pBuf[pCount];
                    ++src, rx6_next_src = (rx6_next_src + 1) & (__USART_RX_BUF_LEN - 1)) {
                *src = rx6_buf[rx6_next_src];
            }
            break;
#endif

    }

    return (src - pBuf);
}

void
USARTSendBuf(Usart_t pUart, uint8_t *pBuf, uint32_t pCount) {

    //todo: this function copies the contents of a linear buffer into a circular buffer without optimizing for alignment
    //todo: evaluate the speed of this function and compare it to an optimized implementation that takes advantage of word alignment

    // If Asynchronous TX is enabled, then all characters to be sent out are queued onto a circular buffer
    // characters are transmitted from this buffer one by one, until all queued characters have been transmitted
    // when queueing characters onto the buffer, the consumption by the transmitter is disabled, so as to prevent
    // race conditions from occuring when accessing the vacant and occupied pointers

    switch (pUart) {

        case USART_PERIPH_2:
            USART_CLR_BIT(USART2->CR1, USART_CR1_TXEIEn);

            for (uint8_t *src = pBuf; src != &pBuf[pCount]; ++src) {
                tx2_buf[tx2_next_dst++] = *src;
                tx2_next_dst &= (__USART_TX_BUF_LEN - 1);
            }

            USART_SET_BIT(USART2->CR1, USART_CR1_TXEIEn);
            break;

        case USART_PERIPH_1:
            USART_CLR_BIT(USART1->CR1, USART_CR1_TXEIEn);

            for (uint8_t *src = pBuf; src != &pBuf[pCount]; ++src) {
                tx1_buf[tx1_next_dst++] = *src;
                tx1_next_dst &= (__USART_TX_BUF_LEN - 1);
            }

            USART_SET_BIT(USART1->CR1, USART_CR1_TXEIEn);
            break;

        case USART_PERIPH_6:
            USART_CLR_BIT(USART6->CR1, USART_CR1_TXEIEn);

            for (uint8_t *src = pBuf; src != &pBuf[pCount]; ++src) {
                tx6_buf[tx6_next_dst++] = *src;
                tx6_next_dst &= (__USART_TX_BUF_LEN - 1);
            }

            USART_SET_BIT(USART6->CR1, USART_CR1_TXEIEn);
            break;
    }
}

void
USARTSendBreak(Usart_t pUart) {

    // To transmit a break character from a USART, set the SBK flag in CR1
    // If the flag is already set, a break character is currently being transmitted, and it is necessary to wait

    switch (pUart) {

        case USART_PERIPH_2:
            while (USART_GET_BIT(USART2->CR1, USART_CR1_SBKn));
            USART_SET_BIT(USART2->CR1, USART_CR1_SBKn);
            break;

        case USART_PERIPH_1:
            while (USART_GET_BIT(USART1->CR1, USART_CR1_SBKn));
            USART_SET_BIT(USART1->CR1, USART_CR1_SBKn);
            break;

        case USART_PERIPH_6:
            while (USART_GET_BIT(USART6->CR1, USART_CR1_SBKn));
            USART_SET_BIT(USART6->CR1, USART_CR1_SBKn);
            break;
    }
}

void __attribute__((__weak__))
USARTOvITCallback(Usart_t pUart) {

    // default panic behaviour is to block further execution forever
    __disable_irq();
    for (;;);
}

void __attribute__((__weak__))
USARTLbITCallback(Usart_t pUart) {

    // default panic behaviour is to block further execution forever
    __disable_irq();
    for (;;);
}

void __attribute__((__weak__))
USARTPeITCallback(Usart_t pUart, uint8_t pC) {\

    // default panic behaviour is to block further execution forever
    __disable_irq();
    for (;;);}

void __attribute__((__weak__))
USARTRxITCallback(Usart_t pUart, uint8_t pC) {
}

void __attribute__((__weak__))
USARTTxITCallback(Usart_t pUart) {
}

void
USART2_IRQHandler() {

    uint32_t    sr  = USART2->SR;
    uint8_t     c   = 0;

    // Overrun Error detected
    if (USART_GET_BIT(sr, USART_SR_OREn)) {
        USARTOvITCallback(USART_PERIPH_2);
    }

    // Line Break detected
    else if (USART_GET_BIT(sr, USART_SR_LBDn)) {
        USARTLbITCallback(USART_PERIPH_2);
    }

    // Parity Error detected
    else if (USART_GET_BIT(sr, USART_SR_PEn)) {
        c = (uint8_t)USART2->DR;
        USARTPeITCallback(USART_PERIPH_2, c);
    }

    // Byte ready
    else if (USART_GET_BIT(sr, USART_SR_RXNEn)) {

    // if asynchronous RX is allowed, read the character and store it within an circular buffer
#if defined(RX2_ENABLE_ASYNC)
        c = (uint8_t)USART2->DR;
        rx2_buf[rx2_next_dst] = c;
        rx2_next_dst = (rx2_next_dst + 1) & (__USART_RX_BUF_LEN - 1);
#endif

        USARTRxITCallback(USART_PERIPH_2, c);
    }

    // Transmission ready
    else if (USART_GET_BIT(sr, USART_SR_TXEn)) {

    // if asynchronous TX is allowed, read the next character from the circular buffer and transmit it
#if defined(TX2_ENABLE_ASYNC)
        if (tx2_next_src != tx2_next_dst) {
            USART2->DR = tx2_buf[tx2_next_src++];
            tx2_next_src &= (__USART_TX_BUF_LEN - 1);

            // if no more characters need to be transmitted, disable the interrupt to prevent getting stuck in it
            if (tx2_next_src == tx2_next_dst) {
                USART_CLR_BIT(USART2->CR1, USART_CR1_TXEIEn);
            }
        }
        else {
            USARTTxITCallback(USART_PERIPH_2);
        }
#endif
    }
}

void
USART1_IRQHandler() {

    uint32_t    sr  = USART1->SR;
    uint8_t     c   = 0;

    // Overrun Error detected
    if (USART_GET_BIT(sr, USART_SR_OREn)) {
        USARTOvITCallback(USART_PERIPH_1);
    }

    // Line Break detected
    else if (USART_GET_BIT(sr, USART_SR_LBDn)) {
        USARTLbITCallback(USART_PERIPH_1);
    }

    // Parity Error detected
    else if (USART_GET_BIT(sr, USART_SR_PEn)) {
        c = (uint8_t)USART1->DR;
        USARTPeITCallback(USART_PERIPH_1, c);
    }

    // Byte ready to be read
    else if (USART_GET_BIT(sr, USART_SR_RXNEn)) {

    // if asynchronous RX is allowed, read the character and store it within an circular buffer
#if defined(RX1_ENABLE_ASYNC)
        c = (uint8_t)USART1->DR;
        rx1_buf[rx1_next_dst] = c;
        rx1_next_dst = (rx1_next_dst + 1) & (__USART_RX_BUF_LEN - 1);
#endif

        USARTRxITCallback(USART_PERIPH_1, c);
    }

    // Transmission ready
    else if (USART_GET_BIT(sr, USART_SR_TXEn)) {

    // if asynchronous TX is allowed, read the next character from the circular buffer and transmit it
#if defined(TX1_ENABLE_ASYNC)
        if (tx1_next_src != tx1_next_dst) {
            USART1->DR = tx1_buf[tx1_next_src++];
            tx1_next_src &= (__USART_TX_BUF_LEN - 1);

            // if no more characters need to be transmitted, disable the interrupt to prevent getting stuck in it
            if (tx1_next_src == tx1_next_dst) {
                USART_CLR_BIT(USART1->CR1, USART_CR1_TXEIEn);
            }
        }
        else {
            USARTTxITCallback(USART_PERIPH_1);
        }
#endif
    }
}


void
USART6_IRQHandler() {

    uint32_t    sr  = USART6->SR;
    uint8_t     c   = 0;

    // Overrun Error detected
    if (USART_GET_BIT(sr, USART_SR_OREn)) {
        USARTOvITCallback(USART_PERIPH_1);
    }

    // Line Break detected
    else if (USART_GET_BIT(sr, USART_SR_LBDn)) {
        USARTLbITCallback(USART_PERIPH_6);
    }

    // Parity Error detected
    else if (USART_GET_BIT(sr, USART_SR_PEn)) {
        c = (uint8_t)USART6->DR;
        USARTPeITCallback(USART_PERIPH_6, c);
    }

    // Byte ready
    else if (USART_GET_BIT(sr, USART_SR_RXNEn)) {

    // if asynchronous RX is allowed, read the character and store it within an circular buffer
#if defined(RX6_ENABLE_ASYNC)
        c = (uint8_t)USART6->DR;
        rx6_buf[rx6_next_dst] = c;
        rx6_next_dst = (rx6_next_dst + 1) & (__USART_RX_BUF_LEN - 1);
#endif

        USARTRxITCallback(USART_PERIPH_6, c);
    }

    // Transmission ready
    else if (USART_GET_BIT(sr, USART_SR_TXEn)) {

    // if asynchronous TX is allowed, read the next character from the circular buffer and transmit it
#if defined(TX1_ENABLE_ASYNC)
        if (tx6_next_src != tx6_next_dst) {
            USART6->DR = tx6_buf[tx6_next_src++];
            tx6_next_src &= (__USART_TX_BUF_LEN - 1);

            // if no more characters need to be transmitted, disable the interrupt to prevent getting stuck in it
            if (tx6_next_src == tx6_next_dst) {
                USART_CLR_BIT(USART6->CR1, USART_CR1_TXEIEn);
            }

        }
        else {
            USARTTxITCallback(USART_PERIPH_6);
        }
#endif
    }
}
