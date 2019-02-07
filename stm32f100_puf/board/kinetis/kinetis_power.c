/*
 * kinetis_power.c
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include <stdbool.h>
#include "kinetis.h"
#include "kinetis_power.h"
#include "../boot_kinetis_config.h"

#define LSI_VALUE                                   32768
#define HSI_VALUE                                   4000000
#define IRC48M_VALUE                                48000000

#define BUS_MAX_CLOCK_HS                            60000000
#define BUS_MAX_CLOCK                               50000000
#if defined(MK22F51212)
#define FLEXBUS_MAX_CLOCK                           30000000
#endif //MK22F51212
#define FLASH_MAX_CLOCK                             24000000


static const unsigned int __FLL_MUL[] =             {640, 1280, 1920, 2560, 732, 1464, 2197, 2929};

#if (HSE_VALUE)
#if (HSE_VALUE) >= 4000000 && (HSE_VALUE) <= 5000000
#define FRDIV                                       2
#elif (HSE_VALUE) >= 8000000 && (HSE_VALUE) <= 10000000
#define FRDIV                                       3
#elif (HSE_VALUE) >= 16000000 && (HSE_VALUE) <= 20000000
#define FRDIV                                       4
#elif (HSE_VALUE) == 32000000
#define FRDIV                                       5
#else
#error Target HSE cant be configured for FEE/FBE!
#endif //MCG_C1_FRDIV
#elif (KINETIS_USB_CRYSTALLESS)
#define FRDIV                                       6
#endif //HSE_VALUE

static unsigned int power_get_osc_clock()
{
    switch (MCG->C7 & MCG_C7_OSCSEL_MASK)
    {
    case MCG_C7_OSCSEL_HSE:
        return HSE_VALUE;
    case MCG_C7_OSCSEL_LSE:
        return LSE_VALUE;
    //MCG_C7_OSCSEL_IRC48M
    default:
        return IRC48M_VALUE;
    }
}

static unsigned int power_get_pll_clock()
{
    return power_get_osc_clock() / PLL_PRDIV * PLL_MUL;
}

static unsigned int power_get_irc_clock()
{
    return (MCG->S & MCG_S_IRCST_MASK) ? HSI_VALUE : LSI_VALUE;
}

static inline unsigned int power_get_fll_clock()
{
    bool slow_reference = false;
    unsigned int ref_clock;
    if (MCG->S & MCG_S_IREFST_MASK)
    {
        ref_clock = power_get_irc_clock();
        if (ref_clock == LSI_VALUE)
            slow_reference = true;
    }
    else
    {
        ref_clock = power_get_osc_clock();
        if (ref_clock == LSE_VALUE)
            slow_reference = true;
    }
    ref_clock *= __FLL_MUL[(MCG->C4 & (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) >> MCG_C4_DRST_DRS_SHIFT];
    return ref_clock / (1 << ((FRDIV) + slow_reference ? 0 : 5));
}

static unsigned int power_get_mcgout_clock()
{
    switch (MCG->S & MCG_S_CLKST_MASK)
    {
    case MCG_S_CLKST_FLL:
        return power_get_fll_clock();
    case MCG_S_CLKST_IRC:
        return power_get_irc_clock();
    case MCG_S_CLKST_OSC:
        return power_get_osc_clock();
    //MCG_S_CLKST_PLL
    default:
        return power_get_pll_clock();
    }
}

static unsigned int core_clock_divider(unsigned int clock, unsigned int max_clock)
{
    unsigned int i;
    for (i = 1; clock / i > max_clock; ++i) {}
    return i;
}

#if (KINETIS_USB_CRYSTALLESS)
static inline void power_fei_fbe_hsi48()
{
    //divide flash clock to get 26.6MHz
    SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV4(1);

    //select HSI48 as OSC
    MCG->C7 = MCG_C7_OSCSEL_IRC48M;

    MCG->C1 = MCG_C1_CLKS_OSC | MCG_C1_FRDIV(FRDIV);
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST_OSC)
    {
        __NOP();
        __NOP();
    }
}
#endif //KINETIS_USB_CRYSTALLESS

#if (HSE_VALUE) || (KINETIS_USB_CRYSTALLESS)
static inline void power_fbe_pee()
{
    unsigned int core_clock = power_get_pll_clock();

    //enter high speed run mode
    SMC->PMCTRL = SMC_PMCTRL_RUNM_HIGH_SPEED;
    while (SMC->PMSTAT != SMC_PMSTAT_HSRUN) {}

    SIM->CLKDIV1 = ((core_clock_divider(core_clock, BUS_MAX_CLOCK_HS) - 1) << SIM_CLKDIV1_OUTDIV2_SHIFT) |
#if defined(MK22F51212)

                   ((core_clock_divider(core_clock, FLEXBUS_MAX_CLOCK) - 1) << SIM_CLKDIV1_OUTDIV3_SHIFT) |
#endif //MK22F51212
                   ((core_clock_divider(core_clock, FLASH_MAX_CLOCK) - 1) << SIM_CLKDIV1_OUTDIV4_SHIFT);

    //FBE -> PBE
    MCG->C5 = MCG_C5_PRDIV0((PLL_PRDIV) - 1) | MCG_C5_PLLCLKEN0_MASK;
    MCG->C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0((PLL_MUL) - 24);
    MCG->C8 |= MCG_C8_LOLRE_MASK;

    //wait until pll selected
    while ((MCG->S & MCG_S_PLLST_MASK) == 0) {}

    //wait until pll locked
    while ((MCG->S & MCG_S_LOCK0_MASK) == 0) {}

    //PBE -> PEE
    MCG->C1 = (MCG->C1 & ~(MCG_C1_CLKS_MASK)) | MCG_C1_CLKS_FLL_PLL;
    //wait until source switched to PLL
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {}
}
#endif //(HSE_VALUE) || (KINETIS_USB_CRYSTALLESS)

static inline unsigned int power_get_bus_clock()
{
    return power_get_mcgout_clock() / (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> SIM_CLKDIV1_OUTDIV2_SHIFT) + 1);
}

unsigned int power_get_core_clock()
{
    return power_get_mcgout_clock() / (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT) + 1);
}


unsigned int power_get_clock(POWER_CLOCK_TYPE clock_type)
{
    switch (clock_type)
    {
    case POWER_CORE_CLOCK:
        return power_get_core_clock();
    case POWER_BUS_CLOCK:
        return power_get_bus_clock();
    default:
        return 0;
    }
}

void power_init()
{
    SMC->PMPROT = SMC_PMPROT_AHSRUN_MASK;

#if (HSE_VALUE)
    //
#elif (KINETIS_USB_CRYSTALLESS)
    power_fei_fbe_hsi48();
#endif

#if (HSE_VALUE) || (KINETIS_USB_CRYSTALLESS)
    //FBE -> PEE
//    power_fbe_pee();
#endif //(HSE_VALUE) || (KINETIS_USB_CRYSTALLESS)
}
