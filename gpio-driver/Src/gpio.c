#include "stm32f4xx.h"
#include "gpio.h"

#define GPIO_SET_BIT(v, i)  (v) |= 1U << (i)
#define GPIO_CLR_BIT(v, i)  (v) &= ~(1U << (i))

#define GPIO_GET_BIT(v, i)  (((v) >> (i)) & 1U)


void
GPIOEnableClockAccess(Gpio_Port_t pPort) {
    // in the stm32f401ccu6, the ahb1 bus is used to provide clock access to the GPIO ports
    // enabling the bit in the enable register that corresponds to the GPIO Port should do the job
    switch (pPort) {
        case GPIO_PORT_A:
            GPIO_SET_BIT(RCC->AHB1ENR, 0);
            break;
        case GPIO_PORT_B:
            GPIO_SET_BIT(RCC->AHB1ENR, 1);
            break;
        case GPIO_PORT_C:
            GPIO_SET_BIT(RCC->AHB1ENR, 2);
            break;
        case GPIO_PORT_D:
            GPIO_SET_BIT(RCC->AHB1ENR, 3);
            break;
        case GPIO_PORT_E:
            GPIO_SET_BIT(RCC->AHB1ENR, 4);
            break;
        case GPIO_PORT_H:
            GPIO_SET_BIT(RCC->AHB1ENR, 7);
            break;
    }
}

void
GPIOInitPinOutput(Gpio_Port_t pPort, uint32_t pPin) {
    // to set pin X as a general purpose output, the value (0, 1) should be written at (2X + 1, 2X) in the MODER register
    // of the correct GPIO Port
    switch (pPort) {
        case GPIO_PORT_A:
            GPIO_SET_BIT(GPIOA->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOA->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_B:
            GPIO_SET_BIT(GPIOB->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOB->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_C:
            GPIO_SET_BIT(GPIOC->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOC->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_D:
            GPIO_SET_BIT(GPIOD->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOD->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_E:
            GPIO_SET_BIT(GPIOE->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOE->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_H:
            GPIO_SET_BIT(GPIOH->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOH->MODER, (2*pPin) + 1);
            break;
    }
}

void GPIOInitPinInput(Gpio_Port_t pPort, uint32_t pPin) {
    // to set pin X as a general purpose input, the value (0, 0) should be written at (2X + 1, 2X) in the MODER register
    // of the correct GPIO Port
    switch (pPort) {
        case GPIO_PORT_A:
            GPIO_CLR_BIT(GPIOA->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOA->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_B:
            GPIO_CLR_BIT(GPIOB->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOB->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_C:
            GPIO_CLR_BIT(GPIOC->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOC->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_D:
            GPIO_CLR_BIT(GPIOD->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOD->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_E:
            GPIO_CLR_BIT(GPIOE->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOE->MODER, (2*pPin) + 1);
            break;
        case GPIO_PORT_H:
            GPIO_CLR_BIT(GPIOH->MODER, 2*pPin);
            GPIO_CLR_BIT(GPIOH->MODER, (2*pPin) + 1);
            break;
    }
}

void
GPIOInitPinOutputType(Gpio_Port_t pPort, uint32_t pPin, Gpio_Out_Type_t pOutType) {
    // to set pin X as a push-pull output, bit X of the OTYPER register of the correct GPIO Port should be cleared
    // to set pin X as an open-drain output, bit X of the OTYPER register of the correct GPIO Port should be set
    switch (pPort) {
        case GPIO_PORT_A:
            (pOutType == GPIO_OUTPUT_OPEN_DRAIN) ? (GPIO_SET_BIT(GPIOA->OTYPER, pPin)) : (GPIO_CLR_BIT(GPIOA->OTYPER, pPin));
            break;
        case GPIO_PORT_B:
            (pOutType == GPIO_OUTPUT_OPEN_DRAIN) ? (GPIO_SET_BIT(GPIOB->OTYPER, pPin)) : (GPIO_CLR_BIT(GPIOB->OTYPER, pPin));
            break;
        case GPIO_PORT_C:
            (pOutType == GPIO_OUTPUT_OPEN_DRAIN) ? (GPIO_SET_BIT(GPIOC->OTYPER, pPin)) : (GPIO_CLR_BIT(GPIOC->OTYPER, pPin));
            break;
        case GPIO_PORT_D:
            (pOutType == GPIO_OUTPUT_OPEN_DRAIN) ? (GPIO_SET_BIT(GPIOD->OTYPER, pPin)) : (GPIO_CLR_BIT(GPIOD->OTYPER, pPin));
            break;
        case GPIO_PORT_E:
            (pOutType == GPIO_OUTPUT_OPEN_DRAIN) ? (GPIO_SET_BIT(GPIOE->OTYPER, pPin)) : (GPIO_CLR_BIT(GPIOE->OTYPER, pPin));
            break;
        case GPIO_PORT_H:
            (pOutType == GPIO_OUTPUT_OPEN_DRAIN) ? (GPIO_SET_BIT(GPIOH->OTYPER, pPin)) : (GPIO_CLR_BIT(GPIOH->OTYPER, pPin));
            break;
    }
}

void
pinSetSpeed(GPIO_TypeDef *pPort, uint32_t pPin, Gpio_Out_Speed_t pOutSpeed) {
    // this is a utility function to make it easy for setting the speed (slew-rate) of an output pin

    // to set the speed of pin X to low, the value (0, 0) should be written at (2X + 1, 2X) in the OSPEEDR register
    // to set the speed of pin X to medium, the value (0, 1) should be written at (2X + 1, 2X) in the OSPEEDR register
    // to set the speed of pin X to high, the value (1, 0) should be written at (2X + 1, 2X) in the OSPEEDR register
    // to set the speed of pin X to very-high, the value (1, 1) should be written at (2X + 1, 2X) in the OSPEEDR register


    switch (pOutSpeed) {
        case GPIO_SPEED_LOW:
            GPIO_CLR_BIT(pPort->OSPEEDR, pPin*2);
            GPIO_CLR_BIT(pPort->OSPEEDR, (pPin*2) + 1);
            break;
        case GPIO_SPEED_MEDIUM:
            GPIO_SET_BIT(pPort->OSPEEDR, pPin*2);
            GPIO_CLR_BIT(pPort->OSPEEDR, (pPin*2) + 1);
            break;
        case GPIO_SPEED_HIGH:
            GPIO_CLR_BIT(pPort->OSPEEDR, pPin*2);
            GPIO_SET_BIT(pPort->OSPEEDR, (pPin*2) + 1);
            break;
        case GPIO_SPEED_VERY_HIGH:
            GPIO_SET_BIT(pPort->OSPEEDR, pPin*2);
            GPIO_SET_BIT(pPort->OSPEEDR, (pPin*2) + 1);
            break;
    }
}

void
pinSetPPDR(GPIO_TypeDef *pPort, uint32_t pPin, Gpio_In_Type_t pInType) {
    // this is a utility function to make it easy to set the type of an input pin

    // to disable pullup and pulldown on pin X, the value (0, 0) should be written at (2X + 1, 2X) in the PUPDR register
    // to enable pullup on pin X, the value (0, 1) should be written at (2X + 1, 2X) in the PUPDR register
    // to enable pulldown on pin X, the value (1, 0) should be written at (2X + 1, 2X) in the PUPDR register

    switch (pInType) {
        case GPIO_INPUT_FLOATING:
            GPIO_CLR_BIT(pPort->PUPDR, pPin*2);
            GPIO_CLR_BIT(pPort->PUPDR, (pPin*2) + 1);
            break;
        case GPIO_INPUT_PULLUP:
            GPIO_SET_BIT(pPort->PUPDR, pPin*2);
            GPIO_CLR_BIT(pPort->PUPDR, (pPin*2) + 1);
            break;
        case GPIO_INPUT_PULLDOWN:
            GPIO_CLR_BIT(pPort->PUPDR, pPin*2);
            GPIO_SET_BIT(pPort->PUPDR, (pPin*2) + 1);
            break;
    }
}

void
GPIOInitPinOutputSpeed(Gpio_Port_t pPort, uint32_t pPin, Gpio_Out_Speed_t pOutSpeed) {
    // refer to the pinSetPPDR function for the register manipulation to set the output-speed of a pin

    switch (pPort) {
        case GPIO_PORT_A:
            pinSetSpeed(GPIOA, pPin, pOutSpeed);
            break;
        case GPIO_PORT_B:
            pinSetSpeed(GPIOB, pPin, pOutSpeed);
            break;
        case GPIO_PORT_C:
            pinSetSpeed(GPIOC, pPin, pOutSpeed);
            break;
        case GPIO_PORT_D:
            pinSetSpeed(GPIOD, pPin, pOutSpeed);
            break;
        case GPIO_PORT_E:
            pinSetSpeed(GPIOE, pPin, pOutSpeed);
            break;
        case GPIO_PORT_H:
            pinSetSpeed(GPIOH, pPin, pOutSpeed);
            break;
    }
}

void
GPIOInitPinInputType(Gpio_Port_t pPort, uint32_t pPin, Gpio_In_Type_t pInType) {
    // refer to the pinSetPPDR function for the register manipulation to set the input-type of a pin

    switch (pPort) {
        case GPIO_PORT_A:
            pinSetPPDR(GPIOA, pPin, pInType);
            break;
        case GPIO_PORT_B:
            pinSetPPDR(GPIOB, pPin, pInType);
            break;
        case GPIO_PORT_C:
            pinSetPPDR(GPIOC, pPin, pInType);
            break;
        case GPIO_PORT_D:
            pinSetPPDR(GPIOD, pPin, pInType);
            break;
        case GPIO_PORT_E:
            pinSetPPDR(GPIOE, pPin, pInType);
            break;
        case GPIO_PORT_H:
            pinSetPPDR(GPIOH, pPin, pInType);
            break;
    }
}


void
GPIOWrite(Gpio_Port_t pPort, uint32_t pPin, Gpio_Out_Val_t pVal) {
    // to set the state of pin X to high, bit X of the ODR register must be set
    // to set the state of pin X to low, bit X of the ODR register must be cleared
    // this method is not atomic, as updating the ODR register requires a full load-update-store cycle to be executed
    // therefore, two threads trying to simultaneously update the state of the register may end up writing different values to the register

    switch (pPort) {
        case GPIO_PORT_A:
            (pVal == GPIO_STATE_ON) ? (GPIO_SET_BIT(GPIOA->ODR, pPin)) : (GPIO_CLR_BIT(GPIOA->ODR, pPin));
            break;
        case GPIO_PORT_B:
            (pVal == GPIO_STATE_ON) ? (GPIO_SET_BIT(GPIOB->ODR, pPin)) : (GPIO_CLR_BIT(GPIOB->ODR, pPin));
            break;
        case GPIO_PORT_C:
            (pVal == GPIO_STATE_ON) ? (GPIO_SET_BIT(GPIOC->ODR, pPin)) : (GPIO_CLR_BIT(GPIOC->ODR, pPin));
            break;
        case GPIO_PORT_D:
            (pVal == GPIO_STATE_ON) ? (GPIO_SET_BIT(GPIOD->ODR, pPin)) : (GPIO_CLR_BIT(GPIOD->ODR, pPin));
            break;
        case GPIO_PORT_E:
            (pVal == GPIO_STATE_ON) ? (GPIO_SET_BIT(GPIOE->ODR, pPin)) : (GPIO_CLR_BIT(GPIOE->ODR, pPin));
            break;
        case GPIO_PORT_H:
            (pVal == GPIO_STATE_ON) ? (GPIO_SET_BIT(GPIOH->ODR, pPin)) : (GPIO_CLR_BIT(GPIOH->ODR, pPin));
            break;
    }
}

void
GPIOWriteAtomic(Gpio_Port_t pPort, uint32_t pPin, Gpio_Out_Val_t pVal) {
    // to atomically set the state of pin X to high, bit X of the ODR register must be set
    // to atomically set the state of pin X to low, bit (16+X) of the ODR register must be cleared
    // this method is atomic, because a thread does not have to load-update-store to update the value of the register
    // a thread only has to write a single bit to this register to either reset or set the pin

    switch (pPort) {
        case GPIO_PORT_A:
            GPIO_SET_BIT(GPIOA->BSRR, pPin + pVal);
            break;
        case GPIO_PORT_B:
            GPIO_SET_BIT(GPIOB->BSRR, pPin + pVal);
            break;
        case GPIO_PORT_C:
            GPIO_SET_BIT(GPIOC->BSRR, pPin + pVal);
            break;
        case GPIO_PORT_D:
            GPIO_SET_BIT(GPIOD->BSRR, pPin + pVal);
            break;
        case GPIO_PORT_E:
            GPIO_SET_BIT(GPIOE->BSRR, pPin + pVal);
            break;
        case GPIO_PORT_H:
            GPIO_SET_BIT(GPIOH->BSRR, pPin + pVal);
            break;
    }
}

uint8_t GPIORead(Gpio_Port_t pPort, uint32_t pPin) {
    // to read the value of pin X, the value of bit X in the IDR register needs to be fetched

    switch (pPort) {
        case GPIO_PORT_A:
            return GPIO_GET_BIT(GPIOA->IDR, pPin);
            break;
        case GPIO_PORT_B:
            return GPIO_GET_BIT(GPIOB->IDR, pPin);
            break;
        case GPIO_PORT_C:
            return GPIO_GET_BIT(GPIOC->IDR, pPin);
            break;
        case GPIO_PORT_D:
            return GPIO_GET_BIT(GPIOD->IDR, pPin);
            break;
        case GPIO_PORT_E:
            return GPIO_GET_BIT(GPIOE->IDR, pPin);
            break;
        case GPIO_PORT_H:
            return GPIO_GET_BIT(GPIOH->IDR, pPin);
            break;
        default:
            return 0U;
    }
}
