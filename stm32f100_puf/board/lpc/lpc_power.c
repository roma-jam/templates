/*
 * lpc_power.c
 *
 *  Created on: 14 рту. 2017 у.
 *      Author: RLeonov
 */

#include "lpc.h"
#include "lpc_power.h"

#define IRC_VALUE                               12000000
#define PLL_LOCK_TIMEOUT                        10000

//timer is not yet active on power on
static void delay_clks(unsigned int clks)
{
    int i;
    for (i = 0; i < clks; ++i)
        __NOP();
}
static unsigned int lpc_get_source_clock(unsigned int source);

static inline unsigned int lpc_get_pll1_clock()
{
    unsigned int pll1_out;
    unsigned int pll1_in = lpc_get_source_clock(LPC_CGU->PLL1_CTRL & CGU_PLL1_CTRL_CLK_SEL_Msk);
    pll1_out = pll1_in / (((LPC_CGU->PLL1_CTRL & CGU_PLL1_CTRL_NSEL_Msk) >> CGU_PLL1_CTRL_NSEL_Pos) + 1)
                       *  (((LPC_CGU->PLL1_CTRL & CGU_PLL1_CTRL_MSEL_Msk) >> CGU_PLL1_CTRL_MSEL_Pos) + 1);
    if ((LPC_CGU->PLL1_CTRL & CGU_PLL1_CTRL_DIRECT_Msk) == 0)
        pll1_out /= 2 * (1 << ((LPC_CGU->PLL1_CTRL & CGU_PLL1_CTRL_PSEL_Msk) >> CGU_PLL1_CTRL_PSEL_Pos));
    return pll1_out;
}

static unsigned int lpc_get_source_clock(unsigned int source)
{
    switch (source)
    {
    case CGU_CLK_IRC:
        return IRC_VALUE;
    case CGU_CLK_LSE:
        return LSE_VALUE;
    case CGU_CLK_HSE:
        return HSE_VALUE;
    case CGU_CLK_PLL1:
        return lpc_get_pll1_clock();
    case CGU_CLK_IDIVA:
    case CGU_CLK_IDIVB:
    case CGU_CLK_IDIVC:
    case CGU_CLK_IDIVD:
    case CGU_CLK_IDIVE:
        return lpc_get_source_clock(CGU_IDIVx_CTRL[(source - CGU_CLK_IDIVA) >> CGU_CLK_CLK_SEL_POS] & CGU_IDIVx_CTRL_CLK_SEL_Msk) /
                (((CGU_IDIVx_CTRL[(source - CGU_CLK_IDIVA) >> CGU_CLK_CLK_SEL_POS] & CGU_IDIVx_CTRL_IDIV_Msk) >> CGU_IDIVx_CTRL_IDIV_Pos) + 1);
    default:
        return 0;

    }
}

unsigned int lpc_power_get_core_clock_inside()
{
    return lpc_get_source_clock(LPC_CGU->BASE_M3_CLK & CGU_BASE_M3_CLK_CLK_SEL_Msk);
}


static inline void lpc_setup_clock()
{
    int i;
    //1. Switch to internal source
    LPC_CGU->BASE_M3_CLK = CGU_CLK_IRC;
    __NOP();
    __NOP();
    __NOP();

    //2. Turn HSE on if enabled
    LPC_CGU->XTAL_OSC_CTRL = CGU_XTAL_OSC_CTRL_ENABLE_Msk;
#if (HSE_VALUE)
#if (HSE_VALUE) > 15000000
    LPC_CGU->XTAL_OSC_CTRL = CGU_XTAL_OSC_CTRL_HF_Msk;
#endif
    LPC_CGU->XTAL_OSC_CTRL &= ~CGU_XTAL_OSC_CTRL_ENABLE_Msk;
    delay_clks(500);
#endif //HSE_VALUE

    //3. Turn PLL1 on
    LPC_CGU->PLL1_CTRL = CGU_PLL1_CTRL_PD_Msk;
    __NOP();
    __NOP();
    __NOP();

    LPC_CGU->PLL1_CTRL |= (0 << CGU_PLL1_CTRL_PSEL_Pos) | ((PLL_N - 1) << CGU_PLL1_CTRL_NSEL_Pos) | ((PLL_M - 1) << CGU_PLL1_CTRL_MSEL_Pos);
#if (HSE_VALUE)
    LPC_CGU->PLL1_CTRL |= CGU_CLK_HSE;
#else
    LPC_CGU->PLL1_CTRL |= CGU_CLK_IRC;
#endif

#if (PLL_P == 1)
    LPC_CGU->PLL1_CTRL |= CGU_PLL1_CTRL_DIRECT_Msk;
#else
    LPC_CGU->PLL1_CTRL |= (32 - __builtin_clz(PLL_P)) << CGU_PLL1_CTRL_PSEL_Pos;
#endif
    LPC_CGU->PLL1_CTRL &= ~CGU_PLL1_CTRL_PD_Msk;

    //wait for PLL lock
    for (i = 0; i < PLL_LOCK_TIMEOUT; ++i)
        if (LPC_CGU->PLL1_STAT & CGU_PLL1_STAT_LOCK_Msk)
            break;
    //HSE failure? switch to IRC
    if ((LPC_CGU->PLL1_STAT & CGU_PLL1_STAT_LOCK_Msk) == 0)
    {
        LPC_CGU->PLL1_CTRL |= CGU_PLL1_CTRL_PD_Msk;
        __NOP();
        __NOP();
        __NOP();
        LPC_CGU->PLL1_CTRL &= ~CGU_PLL1_CTRL_CLK_SEL_Msk;
        LPC_CGU->PLL1_CTRL |= CGU_CLK_IRC;
        LPC_CGU->PLL1_CTRL &= ~CGU_PLL1_CTRL_PD_Msk;
        while ((LPC_CGU->PLL1_STAT & CGU_PLL1_STAT_LOCK_Msk) == 0) {}
    }

    //4. configure IDIVB divider
    LPC_CGU->IDIVB_CTRL = CGU_IDIVB_CTRL_PD_Msk;
    LPC_CGU->IDIVB_CTRL |= ((POWER_LO_DIVIDER - 1) << CGU_IDIVB_CTRL_IDIV_Pos) | CGU_IDIVB_CTRL_AUTOBLOCK_Msk | CGU_CLK_PLL1;
    LPC_CGU->IDIVB_CTRL &= ~CGU_IDIVB_CTRL_PD_Msk;
    //5. connect M3 clock to IDIVB
    LPC_CGU->BASE_M3_CLK = CGU_CLK_IDIVB;
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    //6. Tune flash accelerator.
    LPC_CREG->FLASHCFGA = CREG_FLASHCFG_MAGIC_Msk | (8 << CREG_FLASHCFG_FLASHTIM_Pos) | CREG_FLASHCFG_POW_Msk;
    LPC_CREG->FLASHCFGB = CREG_FLASHCFG_MAGIC_Msk | (8 << CREG_FLASHCFG_FLASHTIM_Pos) | CREG_FLASHCFG_POW_Msk;

    //7. Power down unused hardware
    LPC_CGU->IDIVA_CTRL = CGU_IDIVA_CTRL_PD_Msk;
    LPC_CGU->IDIVC_CTRL = CGU_IDIVC_CTRL_PD_Msk;
    LPC_CGU->IDIVD_CTRL = CGU_IDIVD_CTRL_PD_Msk;
    LPC_CGU->IDIVE_CTRL = CGU_IDIVE_CTRL_PD_Msk;
    LPC_CGU->BASE_USB0_CLK = CGU_BASE_USB0_CLK_PD_Msk;
    LPC_CGU->BASE_SPIFI_CLK = CGU_BASE_SPIFI_CLK_PD_Msk;
    LPC_CGU->BASE_SDIO_CLK = CGU_BASE_SDIO_CLK_PD_Msk;
    LPC_CGU->BASE_SSP0_CLK = CGU_BASE_SSP0_CLK_PD_Msk;
    LPC_CGU->BASE_SSP1_CLK = CGU_BASE_SSP1_CLK_PD_Msk;
    LPC_CGU->BASE_UART0_CLK = CGU_BASE_UART0_CLK_PD_Msk;
    LPC_CGU->BASE_UART1_CLK = CGU_BASE_UART1_CLK_PD_Msk;
    LPC_CGU->BASE_UART2_CLK = CGU_BASE_UART2_CLK_PD_Msk;
    LPC_CGU->BASE_UART3_CLK = CGU_BASE_UART3_CLK_PD_Msk;
    LPC_CGU->BASE_OUT_CLK = CGU_BASE_OUT_CLK_PD_Msk;
    LPC_CGU->BASE_CGU_OUT0_CLK = CGU_BASE_CGU_OUT0_CLK_PD_Msk;
    LPC_CGU->BASE_CGU_OUT1_CLK = CGU_BASE_CGU_OUT1_CLK_PD_Msk;

#if defined(LPC183x) || defined(LPC185x)
    LPC_CGU->BASE_USB1_CLK = CGU_BASE_USB1_CLK_PD_Msk;
    LPC_CGU->BASE_PHY_RX_CLK = CGU_BASE_PHY_RX_CLK_PD_Msk;
    LPC_CGU->BASE_PHY_TX_CLK = CGU_BASE_PHY_TX_CLK_PD_Msk;
#endif //defined(LPC183x) || defined(LPC185x)

#if defined(LPC185x)
    LPC_CGU->BASE_LCD_CLK = CGU_BASE_LCD_CLK_PD_Msk;
#endif //defined(LPC183x) || defined(LPC185x)
}

static void lpc_power_switch_core_clock(unsigned int divider)
{
#if (POWER_MID_DIVIDER)
    //switch to MID clock frequency
    LPC_CGU->IDIVB_CTRL = (LPC_CGU->IDIVB_CTRL & ~CGU_IDIVB_CTRL_IDIV_Msk) | ((POWER_MID_DIVIDER - 1) << CGU_IDIVB_CTRL_IDIV_Pos);
    //need to wait 50us before switching to HI/LO freq.
    delay_clks(90 * 50);
#endif //POWER_MID_DIVIDER
    LPC_CGU->IDIVB_CTRL = (LPC_CGU->IDIVB_CTRL & ~CGU_IDIVB_CTRL_IDIV_Msk) | ((divider - 1) << CGU_IDIVB_CTRL_IDIV_Pos);
    //Wait till stabilization
    __NOP();
    __NOP();
    __NOP();
}

unsigned int power_get_clock(POWER_CLOCK_TYPE clock_type)
{
    switch (clock_type)
    {
#ifdef LPC18xx
    case POWER_BUS_CLOCK:
        return lpc_get_pll1_clock();
#endif //LPC18xx
    case POWER_CORE_CLOCK:
        return lpc_power_get_core_clock_inside();
    }

    return 0;
}

unsigned int power_get_core_clock()
{
    return power_get_clock(POWER_CORE_CLOCK);
}

void power_init()
{
    lpc_setup_clock();
    lpc_power_switch_core_clock(POWER_MID_DIVIDER);
}
