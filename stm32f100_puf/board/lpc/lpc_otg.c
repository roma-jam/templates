/*
 * lpc_otg.c
 *
 *  Created on: 14 рту. 2017 у.
 *      Author: RLeonov
 */

#include "lpc.h"
#include "lpc_otg.h"
#include "lpc_pin.h"
#include "../../dbg.h"
#include "../../system.h"
#include <string.h>

#include "../core/lpc/LPC18xx.h"
#include "../core/lpc/lpc18xx_bits.h"

//each is 16KB, 96KB limited by LPC18xx RAM size.
#define USB_DTD_COUNT           6
#define USB_DTD_CHUNK           0x4000

#pragma pack(push, 1)
typedef struct
{
    uint32_t next;
    uint32_t size_flags;
    void* buf[5];
    uint32_t align[9];
} DTD;

typedef struct
{
    uint32_t capa;
    DTD* cur;
    DTD* next;
    uint32_t shadow_size_flags;
    void* shadow_buf[5];
    uint32_t reserved;
    uint32_t stp[2];
    uint32_t align[4];
} DQH;
#pragma pack(pop)

#define EP_CTRL(port)                       ((uint32_t*)(&(port->ENDPTCTRL0)))
#define EP_BIT(num)                         (1 << (USB_EP_NUM(num) + (((num) & USB_EP_IN) ? 16 : 0)))

static inline EP* ep_data(SYSTEM* system, unsigned int num)
{
    // comment for decrease size
//    if(USB_EP_NUM(num) > USB_EP_COUNT_MAX)
//        return NULL;

    return (num & USB_EP_IN) ? (&system->usbh.in[USB_EP_NUM(num)]) : (&system->usbh.out[USB_EP_NUM(num)]);
}

static inline DQH* ep_dqh(unsigned int num)
{
    return &(((DQH*)LPC_USB0->ENDPOINTLISTADDR)[USB_EP_NUM(num) * 2 + ((num & USB_EP_IN) ? 1 : 0)]);
}

static inline DTD* ep_dtd(unsigned int num, unsigned int i)
{
    return &(((DTD*)(LPC_USB0->ENDPOINTLISTADDR + sizeof(DQH) * USB_EP_COUNT_MAX * 2))[(USB_EP_NUM(num) * 2 + ((num & USB_EP_IN) ? 1 : 0)) * USB_DTD_COUNT + i]);
}

static void lpc_otg_bus_reset(SYSTEM* system)
{
    int i;
    //clear all SETUP token semaphores
    LPC_USB0->ENDPTSETUPSTAT = LPC_USB0->ENDPTSETUPSTAT;
    //Clear all the endpoint complete status bits
    LPC_USB0->ENDPTCOMPLETE = LPC_USB0->ENDPTCOMPLETE;
    //Cancel all primed status
    while (LPC_USB0->ENDPTPRIME) {}
    //And flush all endpoints
    LPC_USB0->ENDPTFLUSH = 0xffffffff;
    while (LPC_USB0->ENDPTFLUSH) {}
    //Disable all EP (except EP0)
    for (i = 1; i < USB_EP_COUNT_MAX; ++i)
        EP_CTRL(LPC_USB0)[i] = 0x0;

    for (i = 0; i < USB_EP_COUNT_MAX; ++i)
    {
        ep_dqh(i)->capa = 0x0;
        ep_dqh(i)->next = (void*)USB0_DQH_NEXT_T_Msk;

        ep_dqh(USB_EP_IN | i)->capa = 0x0;
        ep_dqh(USB_EP_IN | i)->next = (void*)USB0_DQH_NEXT_T_Msk;
    }
    LPC_USB0->DEVICEADDR = 0;
}

static void ep_transfer(SYSTEM* system, unsigned int num, void* addr, unsigned int size)
{
    unsigned int i;
    DTD* dtd;
    DQH* dqh;

    EP* ep = ep_data(system, num);

//    if ((num & USB_EP_IN) == 0)
//    {
//        size = ((ep_dtd(num, 0)->size_flags & USB0_DTD_SIZE_FLAGS_SIZE_Msk) >> USB0_DTD_SIZE_FLAGS_SIZE_Pos);
//    }

    if (size > USB_DTD_CHUNK * USB_DTD_COUNT)
        return;

    i = 0;

    do {
        //prepare DTD
        dtd = ep_dtd(num, i);

        if (size > USB_DTD_CHUNK)
        {
            dtd->size_flags = (USB_DTD_CHUNK << USB0_DTD_SIZE_FLAGS_SIZE_Pos) | USB0_DTD_SIZE_FLAGS_ACTIVE_Msk;
            dtd->next = (unsigned int)ep_dtd(num, i + 1);
            size -= USB_DTD_CHUNK;
        }
        else
        {
            dtd->next = USB0_DQH_NEXT_T_Msk;
            dtd->size_flags = (size << USB0_DTD_SIZE_FLAGS_SIZE_Pos) | USB0_DTD_SIZE_FLAGS_IOC_Msk | USB0_DTD_SIZE_FLAGS_ACTIVE_Msk;
            size = 0;
        }

        dtd->buf[0] = (uint8_t*)addr + i * USB_DTD_CHUNK;
        dtd->buf[1] = (void*)(((unsigned int)(dtd->buf[0]) + 0x1000) & 0xfffff000);
        dtd->buf[2] = dtd->buf[1] + 0x1000;
        dtd->buf[3] = dtd->buf[1] + 0x2000;
        dtd->buf[4] = dtd->buf[1] + 0x3000;

        ++i;
    } while (size);

    //insert in EQH
    dqh = ep_dqh(num);
    dqh->next = ep_dtd(num, 0);
    dqh->shadow_size_flags &= ~USB0_DTD_SIZE_FLAGS_STATUS_Msk;

    //required before EP0 transfers
    if (USB_EP_NUM(num) == 0)
        while (LPC_USB0->ENDPTSETUPSTAT & (1 << 0)) {}
    LPC_USB0->ENDPTPRIME = EP_BIT(num);
}

void board_usb_flush_ep(SYSTEM* system, int num)
{
    EP* ep = ep_data(system, num);

    // comment for decrease size
//    if (ep == NULL)
//        return;

    //flush
    LPC_USB0->ENDPTFLUSH = EP_BIT(num);
    while (LPC_USB0->ENDPTFLUSH & EP_BIT(num)) {}
    // reset sequence
    EP_CTRL(LPC_USB0)[USB_EP_NUM(num)] |= USB0_ENDPTCTRL_R_Msk << ((num & USB_EP_IN) ? 16 : 0);
}

void board_usb_tx(SYSTEM* system, int num, char *data, int size)
{
    EP* ep = ep_data(system, num);

    // comment for decrease size
//    if (ep == NULL)
//        return;

    ep->ptr = data;
    ep->processed = 0;
    ep->total = size;
    ep_transfer(system, num, ep->ptr, ep->total);
}

void board_usb_rx(SYSTEM* system, int num, char *data, int size)
{
    EP* ep = ep_data(system, num);

    // comment for decrease size
//    if (ep == NULL)
//        return;

    ep->ptr = data;
    ep->processed = 0;
    ep->total = size;
    ep_transfer(system, num, ep->ptr, ep->total);
}

void board_usb_open_ep(SYSTEM* system, int num, USB_EP_TYPE type, int size)
{
    unsigned int reg;
    DQH* dqh;

    EP* ep = ep_data(system, num);

    // comment for decrease size
//    if (ep == NULL)
//        return;

    dqh = ep_dqh(num);
    dqh->capa = (size << USB0_DQH_CAPA_MAX_PACKET_LENGTH_Pos) | USB0_DQH_CAPA_ZLT_Msk;
    if (USB_EP_NUM(num) == 0)
        dqh->capa |= USB0_DQH_CAPA_IOS_Msk;
    dqh->next = (void*)USB0_DQH_NEXT_T_Msk;

    if (USB_EP_NUM(num))
        reg = (type << USB0_ENDPTCTRL_T_Pos) | USB0_ENDPTCTRL_E_Msk | USB0_ENDPTCTRL_R_Msk;
    else
        reg = USB0_ENDPTCTRL_E_Msk;

    if (num & USB_EP_IN)
        EP_CTRL(LPC_USB0)[USB_EP_NUM(num)] = (EP_CTRL(LPC_USB0)[USB_EP_NUM(num)] & 0xffff) | (reg << 16);
    else
        EP_CTRL(LPC_USB0)[USB_EP_NUM(num)] = (EP_CTRL(LPC_USB0)[USB_EP_NUM(num)] & (0xffff << 16)) | reg;
}

void board_usb_close_ep(SYSTEM* system, int num)
{
    board_usb_flush_ep(system, num);
}

void board_usb_set_stall(SYSTEM* system, int num)
{
    board_usb_flush_ep(system, num);
    EP_CTRL(LPC_USB0)[USB_EP_NUM(num)] |= USB0_ENDPTCTRL_S_Msk << ((num & USB_EP_IN) ? 16 : 0);
}

void board_usb_clear_stall(SYSTEM* system, int num)
{
    board_usb_flush_ep(system, num);
    EP_CTRL(LPC_USB0)[USB_EP_NUM(num)] &= ~(USB0_ENDPTCTRL_S_Msk << ((num & USB_EP_IN) ? 16 : 0));
    EP_CTRL(LPC_USB0)[USB_EP_NUM(num)] |= USB0_ENDPTCTRL_R_Msk << ((num & USB_EP_IN) ? 16 : 0);
}

bool board_usb_is_stall(SYSTEM* system, int num)
{
    return (EP_CTRL(LPC_USB0)[USB_EP_NUM(num)] & (USB0_ENDPTCTRL_S_Msk << ((num & USB_EP_IN) ? 16 : 0))) ? true : false;
}

bool board_usb_start(SYSTEM* system)
{
    //enable device pullap
    LPC_USB0->USBCMD_D |= USB0_USBCMD_D_RS_Msk;
    return true;
}

void board_usb_stop(SYSTEM* system)
{
   //disable device
    LPC_USB0->USBCMD_D &= ~USB0_USBCMD_D_RS_Msk;
}

void board_usb_init(SYSTEM* system)
{
    int i = 0;
    system->usbh.addr = 0;
    system->usbh.addr_pending = false;
    system->usbh.suspended = false;
    system->usbh.speed = USB_FULL_SPEED;

    //Clock registers
   LPC_CGU->BASE_USB0_CLK |= CGU_CLK_PLL0USB;
   LPC_CGU->BASE_USB0_CLK &= ~CGU_BASE_USB0_CLK_PD_Msk;

   //power on. Turn USB0 PLL 0n
   LPC_CGU->PLL0USB_CTRL = CGU_PLL0USB_CTRL_PD_Msk;
   LPC_CGU->PLL0USB_CTRL |= CGU_PLL0USB_CTRL_DIRECTI_Msk | CGU_CLK_HSE | CGU_PLL0USB_CTRL_DIRECTO_Msk;
   LPC_CGU->PLL0USB_MDIV = USBPLL_M;
   LPC_CGU->PLL0USB_CTRL &= ~CGU_PLL0USB_CTRL_PD_Msk;

   //power on. USBPLL must be turned on even in case of SYS PLL used. Why?
   //wait for PLL lock
   for (i = 0; i < PLL_LOCK_TIMEOUT; ++i)
       if (LPC_CGU->PLL0USB_STAT & CGU_PLL0USB_STAT_LOCK_Msk)
           break;
   if ((LPC_CGU->PLL0USB_STAT & CGU_PLL0USB_STAT_LOCK_Msk) == 0)
       return;
   //enable PLL clock
   LPC_CGU->PLL0USB_CTRL |= CGU_PLL0USB_CTRL_CLKEN_Msk;

   //turn on USB0 PHY
   LPC_CREG->CREG0 &= ~CREG_CREG0_USB0PHY_Msk;

   //USB must be reset before mode change
   LPC_USB0->USBCMD_D = USB0_USBCMD_D_RST_Msk;
   //lowest interrupt latency
   LPC_USB0->USBCMD_D &= ~USB0_USBCMD_D_ITC_Msk;
   while (LPC_USB0->USBCMD_D & USB0_USBCMD_D_RST_Msk) {}
   //set device mode
   LPC_USB0->USBMODE_D = USB0_USBMODE_CM_DEVICE;

   LPC_USB0->ENDPOINTLISTADDR = AHB_SRAM2_BASE + AHB_SRAM2_SIZE - 2 * 2048;

   memset((void*)LPC_USB0->ENDPOINTLISTADDR, 0, (sizeof(DQH) + sizeof(DTD)) * USB_EP_COUNT_MAX * 2);

   //clear any spurious pending interrupts
   LPC_USB0->USBSTS_D = USB0_USBSTS_D_UI_Msk | USB0_USBSTS_D_UEI_Msk | USB0_USBSTS_D_PCI_Msk | USB0_USBSTS_D_SEI_Msk | USB0_USBSTS_D_URI_Msk |
                    USB0_USBSTS_D_SLI_Msk;

   //Unmask common interrupts
   LPC_USB0->USBINTR_D = USB0_USBINTR_D_UE_Msk | USB0_USBINTR_D_PCE_Msk | USB0_USBINTR_D_URE_Msk | USB0_USBINTR_D_SLE_Msk;

   //start
   board_usb_stop(system);
}

static void otg_data(SYSTEM* system, int num)
{
    if(USB_EP_NUM(num) == 0)
    {
        if (num & USB_EP_IN)
            usbd_write_complete(system);
        else
            usbd_read_complete(system);
    }
    else
    {
        if (num & USB_EP_IN)
            class_write_complete(system);
        else
            class_read_complete(system);
    }
}

static void otg_reset(SYSTEM* system)
{
    system->usbh.suspended = false;
    //reset device address
    system->usbh.addr_pending = false;

    usbd_reset(system);
}

void board_usb_request(SYSTEM* system)
{
    int i = 0;
    if (LPC_USB0->USBSTS_D & USB0_USBSTS_D_URI_Msk)
    {
        lpc_otg_bus_reset(system);
        system->usbh.suspended = false;
        LPC_USB0->USBSTS_D = USB0_USBSTS_D_URI_Msk;
    }

    if (LPC_USB0->USBSTS_D & USB0_USBSTS_D_SLI_Msk)
    {
//        lpc_otg_suspend(port, core);
        system->usbh.suspended = true;
        LPC_USB0->USBSTS_D = USB0_USBSTS_D_SLI_Msk;
    }

    if (LPC_USB0->USBSTS_D & USB0_USBSTS_D_PCI_Msk)
    {
        system->usbh.speed = LPC_USB0->PORTSC1_D & USB0_PORTSC1_D_HSP_Msk ? USB_HIGH_SPEED : USB_FULL_SPEED;
        LPC_USB0->USBSTS_D = USB0_USBSTS_D_PCI_Msk;
        if (system->usbh.suspended)
        {
            system->usbh.suspended = false;
            //lpc_otg_wakeup(port, core);
        }
        else
            otg_reset(system);
    }

    for (i = 0; LPC_USB0->ENDPTCOMPLETE && (i < USB_EP_COUNT_MAX); ++i )
    {
        if (LPC_USB0->ENDPTCOMPLETE & EP_BIT(i))
        {
            otg_data(system, i);
            LPC_USB0->ENDPTCOMPLETE = EP_BIT(i);
        }
        if (LPC_USB0->ENDPTCOMPLETE & EP_BIT(USB_EP_IN | i))
        {
            otg_data(system, USB_EP_IN | i);
            LPC_USB0->ENDPTCOMPLETE = EP_BIT(USB_EP_IN | i);
        }
    }

    //Only for EP0
    if (LPC_USB0->ENDPTSETUPSTAT & (1 << 0))
    {
        memcpy((uint8_t*)&system->usbd.setup, ep_dqh(0)->stp, sizeof(SETUP));
        LPC_USB0->ENDPTSETUPSTAT = (1 << 0);
        usbd_setup(system);
    }
    LPC_USB0->USBSTS_D = USB0_USBSTS_D_UI_Msk;
}

void board_usb_set_address(SYSTEM* system, unsigned int address)
{
    //according to datasheet, no special action is required if status will go in 2 ms
    LPC_USB0->DEVICEADDR = (address << USB0_DEVICEADDR_USBADR_Pos) | USB0_DEVICEADDR_USBADRA_Msk;
}
