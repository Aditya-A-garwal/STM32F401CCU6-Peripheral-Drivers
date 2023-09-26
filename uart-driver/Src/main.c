#include "stm32f4xx.h"
#include "uart.h"

#include "stdio.h"

int main() {

    uint8_t buffer[1];
    uint8_t fmt[32];


    __enable_irq();

    USARTEnableClockAccess(USART_PERIPH_2);
    USARTSetPin(USART2_PA2_PA3);
    USARTSetBaud(USART_PERIPH_2, 16000000U, 115200U);
    USARTCommEnable(USART_PERIPH_2, USART_RXTX);

    USARTPeriphEnable(USART_PERIPH_2);

    USARTEnableRxCallback(USART_PERIPH_2);

    for (;;) {
        uint32_t len = USARTRecvBuf(USART_PERIPH_2, buffer, sizeof(buffer) / sizeof(buffer[0]));
        for (uint32_t i = 0, j; i < len; ++i) {
            j = sprintf((char *)fmt, "%c %d\n", buffer[i], buffer[i]);
            USARTSendBuf(USART_PERIPH_2, fmt, j);
        }
    }

    return 0;
}
