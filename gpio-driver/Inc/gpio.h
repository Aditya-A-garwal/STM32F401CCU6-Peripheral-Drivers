#pragma once

#include "stdint.h"

/**
 * @brief               Available GPIO Peripheral Ports on the processor
 *
 */
typedef enum {
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_H
} Gpio_Port_t;

/**
 * @brief               Possible output-types (methods of controlling the pin state) of a pin
 *
 */
typedef enum {
    /** The GPIO state is controlled using two transistors, each responsible for either pulling the pin low or high (default) */
    GPIO_OUTPUT_PUSH_PULL   = 0,
    /** The GPIO state is controlled one transistor, responsible for pulling the pin low; a pullup resistor is always pulling the pin towards high */
    GPIO_OUTPUT_OPEN_DRAIN  = 1
} Gpio_Out_Type_t;

/**
 * @brief               Possible input-types of a pin
 *
 */
typedef enum {
    /** The GPIO pin is not connected to any builtin pullup or pulldown resistors */
    GPIO_INPUT_FLOATING,
    /** The GPIO pin is connected to a builtin pullup resistor */
    GPIO_INPUT_PULLUP,
    /** The GPIO pin is connected to a builtin pulldown resistor */
    GPIO_INPUT_PULLDOWN
} Gpio_In_Type_t;

/**
 * @brief               Possible output-speeds (slew-rate) of a pin
 *
 */
typedef enum {
    /** lowest slew-rate, still absolutely imperceivable by most humans and devices */
    GPIO_SPEED_LOW,
    /** medium slew-rate */
    GPIO_SPEED_MEDIUM,
    /** high slew-rate */
    GPIO_SPEED_HIGH,
    /** highest slew-rate, still absolutely imperceivable by most humans and devices */
    GPIO_SPEED_VERY_HIGH
} Gpio_Out_Speed_t;

/**
 * @brief               Possible output-states of a pin
 *
 */
typedef enum {
    /** Switch the GPIO On (pull it high) */
    GPIO_STATE_ON          = 0,
    /** Switch the GPIO Off (pull it low) */
    GPIO_STATE_OFF         = 16
} Gpio_Out_Val_t;

/**
 * @brief               Enable clock access to the specified GPIO Peripheral from the RCC peripheral
 *
 * @param pPort         The GPIO Peripheral Port to whom clock access must be enabled
 */
void    GPIOEnableClockAccess(Gpio_Port_t pPort);

/**
 * @brief               Initialize the specified Pin on the specified Port as a general-purpose output
 *
 * @note                By default, the output-type is set to push-pull and speed to low
 * @note                Passing values to pPin that are outside the range [0, 15] will produce undefined results
 *
 * @param pPort         The GPIO Peripheral Port to which the Pin belongs
 * @param pPin          The Pin to be used as a general-purpose output
 */
void    GPIOInitPinOutput(Gpio_Port_t pPort, uint32_t pPin);

/**
 * @brief               Initialize the specified Pin on the specified Port as a general-purpose input
 *
 * @note                By default, the input pin does not use any of the builtin pullup or pulldown resistors
 * @note                Passing values to pPin that are outside the range [0, 15] will produce undefined results
 *
 * @param pPort         The GPIO Peripheral Port to which the Pin belongs
 * @param pPin          The Pin to be used as a general-purpose input
 */
void    GPIOInitPinInput(Gpio_Port_t pPort, uint32_t pPin);

/**
 * @brief               Set the output-type/method of state control (push-pull or open-drain) of the specified Pin on the specified Port
 *
 * @note                Passing values to pPin that are outside the range [0, 15], or pins that have not been
 *                      declared as an output will produce undefined results
 *
 * @param pPort         The GPIO Peripheral Port to which the Pin belongs
 * @param pPin          The Pin whose output-type needs to be set
 * @param pOutType      The type of output this Pin should be used as
 */
void    GPIOInitPinOutputType(Gpio_Port_t pPort, uint32_t pPin, Gpio_Out_Type_t pOutType);

/**
 * @brief               Set the output-speed/slew-rate (low, medium, high or very-high) of the specified Pin on the specified Port
 *
 * @note                Passing values to pPin that are outside the range [0, 15], or pins that have not been
 *                      declared as an output will produce undefined results
 *
 * @param pPort         The GPIO Peripheral Port to which the Pin belongs
 * @param pPin          The Pin whose output-speed needs to be set
 * @param pOutSpeed     The speed of output of this Pin
 */
void    GPIOInitPinOutputSpeed(Gpio_Port_t pPort, uint32_t pPin, Gpio_Out_Speed_t pOutSpeed);

/**
 * @brief               Set the input-type (floating, pullup or pulldown) of the specified Pin on the specified Port
 *
 * @note                Passing values to pPin that are outside the range [0, 15], or pins that have not been
 *                      declared as an input will produce undefined results
 *
 * @param pPort         The GPIO Peripheral Port to which the Pin belongs
 * @param pPin          The Pin whose input-type needs to be set
 * @param pInType       The type of input of this Pin
 */
void    GPIOInitPinInputType(Gpio_Port_t pPort, uint32_t pPin, Gpio_In_Type_t pInType);

/**
 * @brief               Non-atomically update the state (on or off) of the specified Pin on the specified Port (by writing directly to the ODR register)
 *
 * @note                Passing values to pPin that are outside the range [0, 15], or pins that have not been
 *                      declared as an input will produce undefined results
 *
 * @param pPort         The GPIO Peripheral Port to which the Pin belongs
 * @param pPin          The Pin whose output-state needs to be set
 * @param pVal          The value of the output (on or off)
 */
void    GPIOWrite(Gpio_Port_t pPort, uint32_t pPin, Gpio_Out_Val_t pVal);

/**
 * @brief               Atomically update the state (on or off) of the specified Pin on the specified Port (by writing to the BSRR register)
 *
 * @note                Passing values to pPin that are outside the range [0, 15], or pins that have not been
 *                      declared as an input will produce undefined results
 *
 * @param pPort         The GPIO Peripheral Port to which the Pin belongs
 * @param pPin          The Pin whose output-state needs to be set
 * @param pVal          The value of the output (on or off)
 */
void    GPIOWriteAtomic(Gpio_Port_t pPort, uint32_t pPin, Gpio_Out_Val_t pVal);

/**
 * @brief               Read the digital state (true of false) of the specified Pin on the specified Port
 *
 * @note                Passing values to pPin that are outside the range [0, 15], or pins that have not been
 *                      declared as an input will produce undefined results
 *
 * @param pPort         The GPIO Peripheral Port to which the Pin belongs
 * @param pPin          The Pin whose output-state needs to be set
 *
 * @return uint8_t      Zero if the pin is being pulled low, otherwise a non-zero value
 */
uint8_t GPIORead(Gpio_Port_t pPort, uint32_t pPin);
