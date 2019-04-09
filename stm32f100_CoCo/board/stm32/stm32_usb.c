/*
 * stm32_usb.c
 *
 *  Created on: 15 рту. 2017 у.
 *      Author: RLeonov
 */

#include "stm32_usb.h"
#include "stm32.h"
#include "../system.h"

#include "../core/stm32/STM32F0xx/stm32f072xb.h" // TODO: remove me

#if (DFU_DEBUG) && (USB_DEBUG_ERRORS)
#include "../../dbg.h"
#endif

#ifndef NULL
#define NULL                                        0
#endif

#define USB_DM                                      A11
#define USB_DP                                      A12

#define USB_EP_TOTAL_COUNT                          8

#pragma pack(push, 1)

typedef struct {
    uint16_t ADDR_TX;
    uint16_t COUNT_TX;
    uint16_t ADDR_RX;
    uint16_t COUNT_RX;
} USB_BUFFER_DESCRIPTOR;

#define USB_BUFFER_DESCRIPTORS                      ((USB_BUFFER_DESCRIPTOR*) USB_PMAADDR)

#pragma pack(pop)

static inline uint16_t* ep_reg_data(int num)
{
    return (uint16_t*)(USB_BASE + USB_EP_NUM(num) * 4);
}

static inline EP* ep_data(SYSTEM* system, int num)
{
    return (num & USB_EP_IN) ? &(system->usbh.in[USB_EP_NUM(num)]) : &(system->usbh.out[USB_EP_NUM(num)]);
}

static inline void ep_toggle_bits(int num, int mask, int value)
{
    *ep_reg_data(num) = ((*ep_reg_data(num)) & USB_EPREG_MASK) | (((*ep_reg_data(num)) & mask) ^ value);
}

static inline void memcpy16(void* dst, void* src, unsigned int size)
{
    int i;
    size = (size + 1) / 2;
    for (i = 0; i < size; ++i)
        ((uint16_t*)dst)[i] = ((uint16_t*)src)[i];
}

void board_usb_init(SYSTEM* system)
{
    system->usbh.addr = 0;
    system->usbh.suspended = false;

    //enable DM/DP
    pin_enable(USB_DM, STM32_GPIO_MODE_AF | GPIO_OT_PUSH_PULL | GPIO_SPEED_HIGH, AF0);
    pin_enable(USB_DP, STM32_GPIO_MODE_AF | GPIO_OT_PUSH_PULL | GPIO_SPEED_HIGH, AF0);

    //enable clock
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    //close device, probably opened by main device
    board_usb_stop(system);
}

void board_usb_flush_ep(SYSTEM* system, int num)
{
    EP* ep = ep_data(system, num);
    ep->ptr = NULL;
    if (num & USB_EP_IN)
        ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_NAK);
    else
        ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_NAK);
}

void board_usb_close_ep(SYSTEM* system, int num)
{
    board_usb_flush_ep(system, num);
    if (num & USB_EP_IN)
        ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_DIS);
    else
        ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_DIS);
    EP* ep = ep_data(system, num);
    ep->ptr = NULL;
    ep->ep_size = 0;
    ep->mps = 0;
}

void board_usb_open_ep(SYSTEM* system, int num, USB_EP_TYPE type, int size)
{
    //find free addr in FIFO
    unsigned int fifo, i;
    fifo = 0;
    for (i = 0; i < USB_EP_COUNT_MAX; ++i)
    {
        fifo += system->usbh.in[i].mps;
        fifo += system->usbh.out[i].mps;
    }
    fifo += sizeof(USB_BUFFER_DESCRIPTOR) * USB_EP_COUNT_MAX;

    EP* ep = ep_data(system, num);

    uint16_t ctl = USB_EP_NUM(num);
    //setup ep type
    switch (type)
    {
    case USB_EP_CONTROL:
        ctl |= 1 << 9;
        break;
    case USB_EP_BULK:
        ctl |= 0 << 9;
        break;
    case USB_EP_INTERRUPT:
        ctl |= 3 << 9;
        break;
    case USB_EP_ISOCHRON:
        ctl |= 2 << 9;
        break;
    }

    //setup FIFO
    if (num & USB_EP_IN)
    {
        USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].ADDR_TX = fifo;
        USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].COUNT_TX = 0;
    }
    else
    {
        USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].ADDR_RX = fifo;
        if (size <= 62)
            USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].COUNT_RX = ((size + 1) >> 1) << 10;
        else
            USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].COUNT_RX = ((((size + 3) >> 2) - 1) << 10) | (1 << 15);
    }
    ep->mps = size;

    *ep_reg_data(num) = ctl;
    //set NAK, clear DTOG
    if (num & USB_EP_IN)
        ep_toggle_bits(num, USB_EPTX_STAT | USB_EP_DTOG_TX, USB_EP_TX_NAK);
    else
        ep_toggle_bits(num, USB_EPRX_STAT | USB_EP_DTOG_RX, USB_EP_RX_NAK);
}

void usb_fifo_tx(SYSTEM* system, int num)
{
    EP* ep = ep_data(system, num);
    int size = ep->ep_size - ep->processed;
    if (size > ep->mps)
        size = ep->mps;
    USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].COUNT_TX = size;
    memcpy16((void*)(USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].ADDR_TX + USB_PMAADDR), ep->ptr +  ep->processed, size);
    ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_VALID);
    ep->processed += size;
}

void board_usb_tx(SYSTEM* system, int num, char* data, int size)
{
    EP* ep = ep_data(system, num);
    ep->ep_size = size;
    ep->ptr = data;
    ep->processed = 0;

    usb_fifo_tx(system, num);
}

void board_usb_rx(SYSTEM* system, int num, char* data, int size)
{
    EP* ep = ep_data(system, num);
    //no blocks for ZLP
    ep->ep_size = size;
    ep->ptr = data;
    ep->processed = 0;

    ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_VALID);
}

void board_usb_set_stall(SYSTEM* system, int num)
{
    board_usb_flush_ep(system, num);
    if (USB_EP_IN & num)
        ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_STALL);
    else
        ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_STALL);
}

void board_usb_clear_stall(SYSTEM* system, int num)
{
    board_usb_flush_ep(system, num);
    if (USB_EP_IN & num)
        ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_NAK);
    else
        ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_NAK);
}

bool board_usb_is_stall(SYSTEM* system, int num)
{
    if (USB_EP_IN & num)
        return ((*ep_reg_data(num)) & USB_EPTX_STAT) == USB_EP_TX_STALL;
    else
        return ((*ep_reg_data(num)) & USB_EPRX_STAT) == USB_EP_RX_STALL;
}

bool board_usb_start(SYSTEM* system)
{
    //enable clock
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

#if defined(STM32F0)
#if (HSE_VALUE)
    RCC->CFGR3 |= RCC_CFGR3_USBSW;
#else
    //Turn USB HSI On (crystall less)
    RCC->CR2 |= RCC_CR2_HSI48ON;
    while ((RCC->CR2 & RCC_CR2_HSI48RDY) == 0) {}
    //setup CRS
    RCC->APB1ENR |= RCC_APB1ENR_CRSEN;
    CRS->CR |= CRS_CR_AUTOTRIMEN;
    CRS->CR |= CRS_CR_CEN;
#endif //HSE_VALUE
#endif //STM32F0

    //power up and wait tStartup
    USB->CNTR &= ~USB_CNTR_PDWN;
    delay_us(1);
    USB->CNTR &= ~USB_CNTR_FRES;

    USB->CNTR |= USB_CNTR_CTRM;

    //clear any spurious pending interrupts
    USB->ISTR = 0;

    //buffer descriptor table at top
    USB->BTABLE = 0;

    //pullup device
    USB->BCDR |= USB_BCDR_DPPU;
    return true;
}

void board_usb_stop(SYSTEM* system)
{
    //disable pullup
    USB->BCDR &= ~USB_BCDR_DPPU;

    int i;
    //close all endpoints
    for (i = 0; i < USB_EP_COUNT_MAX; ++i)
    {
        board_usb_close_ep(system, i);
        board_usb_close_ep(system, USB_EP_IN | i);
    }

    for (i = USB_EP_COUNT_MAX; i < USB_EP_TOTAL_COUNT; ++i)
    {
        ep_toggle_bits(i, USB_EPTX_STAT | USB_EP_DTOG_TX, USB_EP_TX_DIS);
        ep_toggle_bits(i, USB_EPRX_STAT | USB_EP_DTOG_TX, USB_EP_RX_DIS);
        *ep_reg_data(i) = 0;
    }

    //power down, disable all interrupts
    USB->DADDR = 0;
    USB->CNTR = USB_CNTR_PDWN | USB_CNTR_FRES;
    //disable clock
    RCC->APB1ENR &= ~RCC_APB1ENR_USBEN;
}

static inline void usb_ctr(SYSTEM* system)
{
    uint8_t num = USB->ISTR & USB_ISTR_EP_ID;
    uint16_t size;
    EP* ep = NULL;
    if((*ep_reg_data(num)) & USB_EP_CTR_RX)
        ep = &system->usbh.out[num];

    if((*ep_reg_data(num)) & USB_EP_CTR_TX)
        ep = &system->usbh.in[num];

    //got SETUP
    if (num == 0)
    {
        if(*ep_reg_data(num) & USB_EP_SETUP)
        {
            memcpy16((uint8_t*)&system->usbd.setup, (void*)(USB_BUFFER_DESCRIPTORS[0].ADDR_RX + USB_PMAADDR), 4);
            memcpy16((uint8_t*)&system->usbd.setup + 4, (void*)(USB_BUFFER_DESCRIPTORS[0].ADDR_RX + USB_PMAADDR + 4), 4);
            *ep_reg_data(num) = (*ep_reg_data(num)) & USB_EPREG_MASK & ~USB_EP_CTR_RX;
            usbd_setup(system);
            return;
        }

        if ((*ep_reg_data(num)) & USB_EP_CTR_RX)
        {
            size = USB_BUFFER_DESCRIPTORS[num].COUNT_RX & 0x3ff;
            memcpy16(ep->ptr + ep->processed, (void*)(USB_BUFFER_DESCRIPTORS[num].ADDR_RX + USB_PMAADDR), size);
            *ep_reg_data(num) = (*ep_reg_data(num)) & USB_EPREG_MASK & ~USB_EP_CTR_RX;
            ep->processed += size;

            if (ep->processed >= ep->ep_size)
                usbd_read_complete(system);
            else
                ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_VALID);
            return;
        }

        if ((*ep_reg_data(num)) & USB_EP_CTR_TX)
        {
            *ep_reg_data(num) = (*ep_reg_data(num)) & USB_EPREG_MASK & ~USB_EP_CTR_TX;
            //handle STATUS in for set address
            if (system->usbh.addr && USB_BUFFER_DESCRIPTORS[num].COUNT_TX == 0)
            {
                USB->DADDR = USB_DADDR_EF | system->usbh.addr;
                system->usbh.addr = 0;
            }

            if (ep->processed >= ep->ep_size)
                    usbd_write_complete(system);
            else
                usb_fifo_tx(system, USB_EP_IN | num);
            return;
        }
    }
    // other ep
    if ((*ep_reg_data(num)) & USB_EP_CTR_RX)
    {
        size = USB_BUFFER_DESCRIPTORS[num].COUNT_RX & 0x3ff;
        memcpy16(ep->ptr + ep->processed, (void*)(USB_BUFFER_DESCRIPTORS[num].ADDR_RX + USB_PMAADDR), size);
        *ep_reg_data(num) = (*ep_reg_data(num)) & USB_EPREG_MASK & ~USB_EP_CTR_RX;
        ep->processed += size;

        if (ep->processed >= ep->ep_size || size < ep->mps)
        {
            system->ccidd.data_size = ep->processed;
            class_read_complete(system);
        }
        else
            ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_VALID);
        return;
    }

    if ((*ep_reg_data(num)) & USB_EP_CTR_TX)
    {
        *ep_reg_data(num) = (*ep_reg_data(num)) & USB_EPREG_MASK & ~USB_EP_CTR_TX;

        if (ep->processed >= ep->ep_size || size < ep->mps)
            class_write_complete(system);
        else
            usb_fifo_tx(system, USB_EP_IN | num);
        return;
    }

}

static inline void usb_reset(SYSTEM* system)
{
    system->usbh.suspended = false;
    //enable function
    USB->DADDR = USB_DADDR_EF;
    usbd_reset(system);
}

static inline void usb_suspend(SYSTEM* system)
{
    system->usbh.suspended = true;
    usbd_suspend(system);
}

static inline void usb_wakeup(SYSTEM* system)
{
    system->usbh.suspended = false;
    usbd_wakeup(system);
}

void board_usb_request(SYSTEM* system)
{
    uint16_t sta = USB->ISTR;

    //transfer complete. Most often called
    if (sta & USB_ISTR_CTR)
    {
        usb_ctr(system);
        return;
    }
    //rarely called
    if (sta & USB_ISTR_RESET)
    {
        usb_reset(system);
        USB->ISTR &= ~USB_ISTR_RESET;
        return;
    }
    if ((sta & USB_ISTR_SUSP) && (!system->usbh.suspended))
    {
        usb_suspend(system);
        USB->ISTR &= ~USB_ISTR_SUSP;
        return;
    }
    USB->ISTR &= ~USB_ISTR_SUSP;
    if (sta & USB_ISTR_WKUP)
    {
        usb_wakeup(system);
        USB->ISTR &= ~USB_ISTR_WKUP;
        return;
    }
#if (USB_DEBUG_ERRORS)
    if (sta & USB_ISTR_PMAOVR)
    {
#if (DFU_DEBUG)
        printf("USB overflow\n\r");
#endif
        USB->ISTR &= ~USB_ISTR_PMAOVR;
        return;
    }
    if (sta & USB_ISTR_ERR)
    {
#if (DFU_DEBUG)
        printf("USB hardware error\n\r");
#endif
        USB->ISTR &= ~USB_ISTR_ERR;
        return;
    }
#endif
}

void board_usb_set_address(SYSTEM* system, unsigned int address)
{
    //address will be set after STATUS IN packet
    if (address)
        system->usbh.addr = address;
    else
        USB->DADDR = USB_DADDR_EF;
}
