/*
 * kinetis_pin.c
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include "kinetis.h"
#include "kinetis_pin.h"

static const GPIO_MemMapPtr GPIO_REG[GPIO_PORT_COUNT] =                                {PTA, PTB, PTC, PTD, PTE};
static const PORT_MemMapPtr PORT_REG[GPIO_PORT_COUNT] =                                {PORTA, PORTB, PORTC, PORTD, PORTE};

void pin_enable(GPIO gpio, unsigned int mode, bool direction_out)
{
    unsigned port, pin;
    if (gpio >= GPIO_MAX)
        return;

    port = GPIO_PORT(gpio);
    pin = GPIO_PIN(gpio);

    //enable gating
    SIM->SCGC5 |= (1 << (port + SIM_SCGC5_PORTA_SHIFT));

    //configure PORT
    PORT_REG[port]->PCR[pin] = mode;
    //set direction
    if (direction_out)
    {
        GPIO_REG[port]->PCOR |= (1 << pin);
        GPIO_REG[port]->PDDR |= (1 << pin);
    }
    else
        GPIO_REG[port]->PDDR &= ~(1 << pin);
}

void pin_disable(GPIO gpio)
{
    if (gpio >= GPIO_MAX)
        return;

    GPIO_REG[GPIO_PORT(gpio)]->PDDR &= ~(1 << GPIO_PIN(gpio));
    SIM->SCGC5 &= ~(1 << (GPIO_PORT(gpio) + SIM_SCGC5_PORTA_SHIFT));
}

void pin_set(GPIO gpio)
{
    if (gpio >= GPIO_MAX)
        return;
    GPIO_REG[GPIO_PORT(gpio)]->PSOR |= (1 << GPIO_PIN(gpio));
}

void pin_reset(GPIO gpio)
{
    if (gpio >= GPIO_MAX)
        return;
    GPIO_REG[GPIO_PORT(gpio)]->PCOR |= (1 << GPIO_PIN(gpio));
}

bool pin_get(GPIO gpio)
{
    return (GPIO_REG[GPIO_PORT(gpio)]->PDIR & (1 << GPIO_PIN(gpio))) ? true : false;
}

void gpio_enable(GPIO gpio, unsigned int mode, bool direction_out)
{
    pin_enable(gpio, mode, direction_out);
}

void gpio_disable(GPIO gpio)
{
    pin_disable(gpio);
}
