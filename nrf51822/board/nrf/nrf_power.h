/*
 * nrf_power.h
 *
 *  Created on: 30 ???. 2019 ?.
 *      Author: jam_r
 */

#ifndef _NRF_POWER_H_
#define _NRF_POWER_H_

typedef enum {
    POWER_CORE_CLOCK = 0,
} POWER_CLOCK_TYPE;

void power_init();
unsigned int power_get_clock(POWER_CLOCK_TYPE clock_type);
unsigned int power_get_core_clock();

#endif /* _NRF_POWER_H_ */
