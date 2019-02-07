/*
 * lpc_power.h
 *
 *  Created on: 14 рту. 2017 у.
 *      Author: RLeonov
 */

#ifndef LPC_POWER_H_
#define LPC_POWER_H_

typedef enum {
    POWER_MODE_HIGH = 0,
    POWER_MODE_LOW,
    POWER_MODE_ULTRA_LOW,
    POWER_MODE_STOP,
    POWER_MODE_STANDY
} POWER_MODE;

typedef enum {
    POWER_CORE_CLOCK = 0,
    POWER_BUS_CLOCK,
    POWER_CLOCK_MAX
} POWER_CLOCK_TYPE;

void power_init();
unsigned int power_get_clock(POWER_CLOCK_TYPE clock_type);
unsigned int power_get_core_clock();

#endif /* LPC_POWER_H_ */
