#include "stm32f4xx.h"
#include "gpio.h"

int main() {

    // enable clock access to GPIO Port C
    GPIOEnableClockAccess(GPIO_PORT_C);

    // initialize Pin 13 as an output pin
    GPIOInitPinOutput(GPIO_PORT_C, 13);
    GPIOInitPinOutputType(GPIO_PORT_C, 13, GPIO_OUTPUT_PUSH_PULL);
    GPIOInitPinOutputSpeed(GPIO_PORT_C, 13, GPIO_SPEED_LOW);

    for (;;) {

        // for loops have been used to create delays so as to avoid needing to operate the systick peripheral,
        // which is usually used for creating delay functions

        // switch the pin off and wait for some time (on the black pill, the LED should switch on)
        GPIOWriteAtomic(GPIO_PORT_C, 13, GPIO_STATE_OFF);
        for (volatile uint32_t i = 0; i < (1 << 20); ++i);

        // switch the pin on and wait for some time (on the black pill, the LED should switch off)
        GPIOWriteAtomic(GPIO_PORT_C, 13, GPIO_STATE_ON);
        for (volatile uint32_t i = 0; i < (1 << 20); ++i);
    }

    return 0;
}
