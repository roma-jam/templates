/*
 * kinetis_power.h
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#ifndef KINETIS_POWER_H_
#define KINETIS_POWER_H_

#define SMC_PMCTRL_RUNM_NORMAL                      SMC_PMCTRL_RUNM(0)
#define SMC_PMCTRL_RUNM_VERY_LOW_POWER              SMC_PMCTRL_RUNM(2)
#define SMC_PMCTRL_RUNM_HIGH_SPEED                  SMC_PMCTRL_RUNM(3)

#define SMC_PMSTAT_RUN                              (1 << 0)
#define SMC_PMSTAT_STOP                             (1 << 1)
#define SMC_PMSTAT_VLPR                             (1 << 2)
#define SMC_PMSTAT_VLPW                             (1 << 3)
#define SMC_PMSTAT_VLPS                             (1 << 4)
#define SMC_PMSTAT_LLS                              (1 << 5)
#define SMC_PMSTAT_VLLS                             (1 << 6)
#define SMC_PMSTAT_HSRUN                            (1 << 7)


#define MCG_C1_CLKS_FLL_PLL                         MCG_C1_CLKS(0)
#define MCG_C1_CLKS_IRC                             MCG_C1_CLKS(1)
#define MCG_C1_CLKS_OSC                             MCG_C1_CLKS(2)

#define MCG_S_CLKST_FLL                             MCG_S_CLKST(0)
#define MCG_S_CLKST_IRC                             MCG_S_CLKST(1)
#define MCG_S_CLKST_OSC                             MCG_S_CLKST(2)
#define MCG_S_CLKST_PLL                             MCG_S_CLKST(3)

#define MCG_C7_OSCSEL_HSE                           MCG_C7_OSCSEL(0)
#define MCG_C7_OSCSEL_LSE                           MCG_C7_OSCSEL(1)
#define MCG_C7_OSCSEL_IRC48M                        MCG_C7_OSCSEL(2)

typedef enum {
    POWER_CORE_CLOCK = 0,
    POWER_BUS_CLOCK,
    POWER_CLOCK_MAX
} POWER_CLOCK_TYPE;

void power_init();
unsigned int power_get_clock(POWER_CLOCK_TYPE clock_type);
unsigned int power_get_core_clock();

#endif /* KINETIS_POWER_H_ */
