/*
 * nrf_pin.c
 *
 *  Created on: 30 ???. 2019 ?.
 *      Author: jam_r
 */

#include "nrf_pin.h"

void pin_enable(PIN pin, PIN_MODE mode, PIN_PULL pull)
{
    NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) | \
                                (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)  | \
                                (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) | \
                                (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos);
    /* set up pull up config */
    NRF_GPIO->PIN_CNF[pin] |= (pull << GPIO_PIN_CNF_PULL_Pos);

    if(PIN_MODE_INPUT == mode)
    {
        NRF_GPIO->DIRSET &= ~(1 << pin);
        NRF_GPIO->PIN_CNF[pin] |= (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
    }
    else
    {
        NRF_GPIO->DIRSET |= (1 << pin);
        NRF_GPIO->PIN_CNF[pin] |= (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
    }
}

//void gpio_enable(unsigned int pin, GPIO_MODE mode);

void pin_set(PIN pin)
{
    NRF_GPIO->OUTSET |= (1 << pin);
}

void pin_reset(PIN pin)
{
    NRF_GPIO->OUTCLR |= (1 << pin);
}

void pin_toggle(PIN pin)
{
    NRF_GPIO->OUT ^= (1 << pin);
}

bool pin_get(PIN pin)
{
    return ((NRF_GPIO->IN >> pin) & 1);
}
