/*
 * nrf_pin.h
 *
 *  Created on: 30 ???. 2019 ?.
 *      Author: jam_r
 */

#ifndef _NRF_PIN_H_
#define _NRF_PIN_H_

#include "nrf.h"
#include <stdbool.h>

typedef enum {
    P0 = 0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15,
    P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29, P30,
    //only for service calls
    PIN_DEFAULT,
    PIN_UNUSED
} PIN;

typedef enum {
    PIN_MODE_INPUT,       ///< Input
    PIN_MODE_OUTPUT       ///< Output
} PIN_MODE;

typedef enum {
    PIN_PULL_NOPULL    = GPIO_PIN_CNF_PULL_Disabled,     ///<  Pin pullup resistor disabled
    PIN_PULL_DOWN      = GPIO_PIN_CNF_PULL_Pulldown,   ///<  Pin pulldown resistor enabled
    PIN_PULL_UP        = GPIO_PIN_CNF_PULL_Pullup,       ///<  Pin pullup resistor enabled
} PIN_PULL;

typedef enum {
    PIN_SENSE_NO    = GPIO_PIN_CNF_SENSE_Disabled,   ///<  Pin sense level disabled.
    PIN_SENSE_LOW   = GPIO_PIN_CNF_SENSE_Low,      ///<  Pin sense low level.
    PIN_SENSE_HIGH  = GPIO_PIN_CNF_SENSE_High,    ///<  Pin sense high level.
} PIN_SENSE;



void pin_enable(PIN pin, PIN_MODE mode, PIN_PULL pull);

//void gpio_enable(unsigned int pin, GPIO_MODE mode);
void pin_set(PIN pin);
void pin_reset(PIN);
void pin_toggle(PIN pin);
bool pin_get(PIN pin);

#endif /* _NRF_PIN_H_ */
