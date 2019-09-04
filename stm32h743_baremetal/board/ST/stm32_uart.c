/*
 * stm32_uart.c
 *
 *  Created on: 8 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include "stm32.h"
#include "stm32_uart.h"
#include "stm32_pin.h"
#include "stm32_power.h"

typedef USART_TypeDef* USART_TypeDef_P;

#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {37, 38};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2};

#elif defined(STM32F10X_MD) || defined(STM32F10X_MD_VL)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {37, 38, 39};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17, 18};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3};

#elif defined(STM32F1)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {37, 38, 39, 52, 53};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17, 18, 19, 20};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3, UART4, UART5};

#elif defined(STM32F2) || defined(STM32F40_41xxx)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {37, 38, 39, 52, 53, 71};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {4, 17, 18, 19, 20, 5};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3, UART4, UART5, USART6};

#elif defined(STM32F4)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {37, 38, 39, 52, 53, 71, 82, 83};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {4, 17, 18, 19, 20, 5, 30, 31};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3, UART4, UART5, USART6, UART7, UART8};
#elif defined(STM32L0)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {27, 28};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2};
#elif defined(STM32L1)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {37, 38, 39};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17, 18};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3};
#elif defined(STM32F0)
#if ((UARTS_COUNT)== 1)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {27};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1};
#elif ((UARTS_COUNT)== 2)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {27, 28};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2};
#elif ((UARTS_COUNT)== 4)
static const unsigned int UART_VECTORS[3] =                 {27, 28, 29};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17, 18, 19};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3, USART4};
#elif ((UARTS_COUNT)== 6)
static const unsigned int UART_VECTORS[3] =                 {27, 28, 29};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17, 18, 19, 20, 5};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3, USART4, USART5, USART6};
#else
static const unsigned int UART_VECTORS[3] =                 {27, 28, 29};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17, 18, 19, 20, 5, 6, 7};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3, USART4, USART5, USART6, USART7, USART8};
#endif //UARTS_COUNT
#elif defined(STM32H7)
#if ((UARTS_COUNT) == 4)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {37, 38, 39, 52};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {4, 17, 18, 19};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3, UART4};
#elif ((UART_COUNTS) == 8)
static const unsigned int UART_VECTORS[UARTS_COUNT] =       {37, 38, 39, 52, 53, 71, 82, 83};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {4, 17, 18, 19, 20, 5, 30, 31};
static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=        {USART1, USART2, USART3, UART4, UART5, USART6, UART7, UART8};
#endif
#endif // STM32H7

#if defined(STM32L0) || defined(STM32F0) || defined(STM32H7)
#define USART_SR_TXE        USART_ISR_TXE
#define USART_SR_TC         USART_ISR_TC
#define USART_SR_PE         USART_ISR_PE
#define USART_SR_FE         USART_ISR_FE
#define USART_SR_NE         USART_ISR_NE
#define USART_SR_ORE        USART_ISR_ORE
#define USART_SR_RXNE       USART_ISR_RXNE
#define SR(port)            (UART_REGS[(port)]->ISR)
#define TXC(port, c)        (UART_REGS[(port)]->TDR = (c))
#else
#define SR(port)            (UART_REGS[(port)]->SR)
#define TXC(port, c)        (UART_REGS[(port)]->DR = (c))
#endif

void board_dbg_init()
{
    unsigned int mantissa, fraction;

#if defined(STM32F1)
    pin_enable(UART_TX_PIN, STM32_GPIO_MODE_OUTPUT_AF_PUSH_PULL_50MHZ, false);
#else
    pin_enable(UART_TX_PIN, STM32_GPIO_MODE_AF, UART_AF_NUMBER);
#endif

    //power up (required prior to reg work)
    if (UART == UART_1 || UART >= UART_6)
    {
        RCC->APB2ENR |= 1 << UART_POWER_PINS[UART];
    }
    else
    {
#if defined(STM32H7)
        RCC->APB1LENR |= 1 << UART_POWER_PINS[UART];
#else
        RCC->APB1ENR |= 1 << UART_POWER_PINS[UART];
#endif //
    }

    UART_REGS[UART]->CR1 &= ~USART_CR1_UE;

    //disable core, if was enabled before, set 8N1
    UART_REGS[UART]->CR1 = 0;
    UART_REGS[UART]->CR2 = 0;
    UART_REGS[UART]->CR3 = 0;

    mantissa = (25 * power_get_clock(POWER_CLOCK_APB2)) / (4 * (UART_BAUD));
    fraction = ((mantissa % 100) * 8 + 25)  / 50;
    mantissa = mantissa / 100;
    UART_REGS[UART]->BRR = (mantissa << 4) | fraction;

    //enable core and transmitter
    UART_REGS[UART]->CR1 |= USART_CR1_UE | USART_CR1_TE;
}


void board_dbg(const char *const buf, unsigned int size)
{
    int i;
    for(i = 0; i < size; ++i)
    {
#if defined(STM32L0) || defined(STM32F0)
        while ((SR(UART) & USART_ISR_TXE) == 0) {}
#elif defined(STM32H7)
        while ((SR(UART) & USART_ISR_TXE_TXFNF_Msk) == 0) {}
#else
        while ((SR(UART) & USART_SR_TXE) == 0) {}
#endif
        TXC(UART, buf[i]);
    }
#if defined(STM32L0) || defined(STM32F0) || defined(STM32H7)
    while ((SR(UART) & USART_ISR_TC) == 0) {}
#else
    while ((SR(UART) & USART_SR_TC) == 0) {}
#endif
}
