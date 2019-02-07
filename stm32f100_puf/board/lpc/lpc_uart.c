/*
 * lpc_uart.c
 *
 *  Created on: 14 рту. 2017 у.
 *      Author: RLeonov
 */

#include "lpc.h"
#include "lpc_pin.h"
#include "lpc_power.h"
#include "lpc_uart.h"

#if defined(LPC11U6x)
static const uint8_t __UART_RESET_PINS[] =                      {SYSCON_PRESETCTRL_USART1_RST_N_POS, SYSCON_PRESETCTRL_USART2_RST_N_POS, SYSCON_PRESETCTRL_USART3_RST_N_POS,
                                                                 SYSCON_PRESETCTRL_USART4_RST_N_POS};
static const uint8_t __UART_POWER_PINS[] =                      {SYSCON_SYSAHBCLKCTRL_USART0_POS, SYSCON_SYSAHBCLKCTRL_USART1_POS, SYSCON_SYSAHBCLKCTRL_USART2_POS,
                                                                 SYSCON_SYSAHBCLKCTRL_USART3_4_POS, SYSCON_SYSAHBCLKCTRL_USART3_4_POS};
static const uint8_t __UART_VECTORS[] =                         {21, 11, 12, 12, 11};
#define LPC_USART                                               LPC_USART0

typedef LPC_USART4_Type* LPC_USART4_Type_P;

static const LPC_USART4_Type_P __USART1_REGS[] =                {LPC_USART1, LPC_USART2, LPC_USART3, LPC_USART4};
typedef LPC_USART_Type* LPC_USART_Type_P
static const LPC_USART_Type_P __USART_REGS[UARTS_COUNT] =       {LPC_USART};
#elif defined(LPC11Uxx)
static const uint8_t __UART_POWER_PINS[] =                      {SYSCON_SYSAHBCLKCTRL_USART0_POS};
static const uint8_t __UART_VECTORS[] =                         {21};

typedef LPC_USART_Type* LPC_USART_Type_P;
static const LPC_USART_Type_P __USART_REGS[UARTS_COUNT] =       {LPC_USART};
#else //LPC18xx
static const uint8_t __UART_VECTORS[UARTS_COUNT] =              {24, 25, 26, 27};

typedef LPC_USARTn_Type* LPC_USARTn_Type_P;
static const LPC_USARTn_Type_P __USART_REGS[UARTS_COUNT] =      {LPC_USART0, (LPC_USARTn_Type_P)LPC_UART1, LPC_USART2, LPC_USART3};

#define LPC_CGU_UART0_CLOCK_BASE                                0x4005009c

#endif


static inline void board_dbg_set_baudrate(UART_PORT port, unsigned int baud)
{
    unsigned int divider;

    divider = power_get_clock(POWER_BUS_CLOCK) / (((__USART_REGS[port]->OSR >> 4) & 0x7ff) + 1 ) / baud;

#ifdef LPC11U6x
    if (port > UART_0)
    {
        __USART1_REGS[port - 1]->CFG = ((baudrate.data_bits - 7) << USART4_CFG_DATALEN_POS) | ((baudrate.stop_bits - 1) << USART4_CFG_STOPLEN_POS);
        switch (baudrate.parity)
        {
        case 'O':
            __USART1_REGS[port - 1]->CFG |= USART4_CFG_PARITYSEL_ODD;
            break;
        case 'E':
            __USART1_REGS[port - 1]->CFG |= USART4_CFG_PARITYSEL_EVEN;
            break;
        }
        __USART1_REGS[port - 1]->BRG = divider & 0xffff;
        __USART1_REGS[port - 1]->CFG |= USART4_CFG_ENABLE
    }
    else
#endif
    {
        __USART_REGS[port]->LCR = ((UART_DATA_BITS - 5) << USART0_LCR_WLS_Pos) | ((UART_STOP_BITS - 1) << USART0_LCR_SBS_Pos) | USART0_LCR_DLAB_Msk;
        switch (UART_PARITY)
        {
        case 'O':
            __USART_REGS[port]->LCR |= USART0_LCR_PE_Msk | USART0_LCR_PS_ODD;
            break;
        case 'E':
            __USART_REGS[port]->LCR |= USART0_LCR_PE_Msk | USART0_LCR_PS_EVEN;
            break;
        }
        __USART_REGS[port]->DLM = (divider >> 8) & 0xff;
        __USART_REGS[port]->DLL = divider & 0xff;
        __USART_REGS[port]->LCR &= ~USART0_LCR_DLAB_Msk;
    }
}

void board_dbg_init()
{
    // enable pin
    pin_enable(UART_TX_PIN, UART_TX_PIN_MODE);
    //power up
#ifdef LPC11Uxx
    LPC_SYSCON->SYSAHBCLKCTRL |= 1 << __UART_POWER_PINS[UART];
#else //LPC18xx
    //map PLL1 output to UART clock
    ((uint32_t*)LPC_CGU_UART0_CLOCK_BASE)[UART] = CGU_BASE_UART0_CLK_PD_Msk;
    ((uint32_t*)LPC_CGU_UART0_CLOCK_BASE)[UART] |= CGU_CLK_PLL1;
    ((uint32_t*)LPC_CGU_UART0_CLOCK_BASE)[UART] &= ~CGU_BASE_UART0_CLK_PD_Msk;
#endif //LPC11Uxx

    //remove reset state. Only for LPC11U6x
#ifdef LPC11U6x
    if (port > UART_0)
    {
        LPC_SYSCON->PRESETCTRL |= 1 << __UART_RESET_PINS[UART - 1];
        if (mode & UART_RX_STREAM)
            __USART1_REGS[UART - 1]->INTENCSET = USART4_INTENSET_RXRDYEN | USART4_INTENSET_OVERRUNEN | USART4_INTENSET_FRAMERREN
                                          | USART4_INTENSET_PARITTERREN | USART4_INTENSET_RXNOISEEN | USART4_INTENSET_DELTARXBRKEN;
    }
    else
#endif
    {
         //enable FIFO
        __USART_REGS[UART]->FCR = USART0_FCR_FIFOEN_Msk | USART0_FCR_TXFIFORES_Msk | USART0_FCR_RXFIFORES_Msk;
    }

    board_dbg_set_baudrate(UART, UART_BAUD);

}

void board_dbg(const char *const buf, unsigned int size)
{
    int i;
    for(i = 0; i < size; ++i)
    {
#ifdef LPC11U6x
        if (port > UART_0)
        {
            while ((__USART1_REGS[port - 1]->STAT & USART4_STAT_TXRDY) == 0) {}
            __USART1_REGS[port - 1]->TXDAT = buf[i];
        }
        else
#endif
        {
            while ((__USART_REGS[UART]->LSR & USART0_LSR_THRE_Msk) == 0) {}
                __USART_REGS[UART]->THR = buf[i];
        }
    }
}
