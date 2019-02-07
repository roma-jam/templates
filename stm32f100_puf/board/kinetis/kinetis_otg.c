/*
 * kinetis_otg.c
 *
 *  Created on: 6 θώνÿ 2017 γ.
 *      Author: RLeonov
 */


#include <string.h>
#include "kinetis.h"
#include "kinetis_otg.h"
#include "../../dbg.h"
#include "../../system.h"


#define USB_CLOCK                                       48000000
#define BDT_BASE                                        (SRAM_BASE + SRAM_USB)
#define TRANSFER_SIZE_MAX                               1024
//EP0 is bidirectional pipe
#define DTOG_SHIFT(num)                                 (USB_EP_NUM(num) ? (USB_EP_NUM(num) + ((USB_EP_IN & (num)) >> 3)) : 0)

#pragma pack(push, 1)
typedef struct {
    uint32_t ctl;
    void* addr;
} BD;
#pragma pack(pop)

#define BDT                                             ((BD*)(BDT_BASE))

#define USB_BD_BC_MASK                                  (0x3ff << 16)
#define USB_BD_BC(val)                                  ((val) << 16)
#define USB_BD_BC_SHIFT                                 16
#define USB_BD_OWN_MASK                                 (1 << 7)
#define USB_BD_DATA1_MASK                               (1 << 6)
#define USB_BD_KEEP_MASK                                (1 << 5)
#define USB_BD_NINC_MASK                                (1 << 4)
#define USB_BD_DTS_MASK                                 (1 << 3)
#define USB_BD_PID_MASK                                 (0xf << 2)

//device mode
#define USB_BD_PID_OUT                                  (0x1 << 2)
#define USB_BD_PID_IN                                   (0x9 << 2)
#define USB_BD_PID_SETUP                                (0xd << 2)

//host mode
#define USB_BD_PID_DATA0                                (0x3 << 2)
#define USB_BD_PID_DATA1                                (0xb << 2)
#define USB_BD_PID_ACK                                  (0x2 << 2)
#define USB_BD_PID_STALL                                (0xe << 2)
#define USB_BD_PID_NAK                                  (0xa << 2)
#define USB_BD_PID_BUS_TIMEOUT                          (0x0 << 2)
#define USB_BD_PID_DATA_ERROR                           (0xf << 2)


static inline EP* ep_data(SYSTEM* system, unsigned int num)
{
    if(USB_EP_NUM(num) > USB_EP_COUNT_MAX)
        return NULL;

    return (num & USB_EP_IN) ? &system->usbh.in[USB_EP_NUM(num)] : &system->usbh.out[USB_EP_NUM(num)];
}

static inline BD* ep_bd(unsigned int num, bool odd)
{
    unsigned int idx = USB_EP_NUM(num) << 2;
    if (num & USB_EP_IN)
        idx |= 1 << 1;
    if (odd)
        idx |= 1 << 0;
    return &(BDT[idx]);
}

static void ep_transfer(SYSTEM* system, unsigned int num, bool odd, void* addr, unsigned int size)
{
    EP* ep;
    BD* bd = ep_bd(num, odd);
    ep = ep_data(system, num);
    if (size > ep->ep_size)
        size = ep->ep_size;

    if ((num & USB_EP_IN) == 0)
        size = ep->ep_size;

    bd->addr = addr;
    bd->ctl = USB_BD_BC(size);

    if (system->usbh.dtog & (1 << DTOG_SHIFT(num)))
        bd->ctl |= USB_BD_DATA1_MASK;

    system->usbh.dtog ^= 1 << DTOG_SHIFT(num);
    bd->ctl |= USB_BD_OWN_MASK;
}

void board_usb_flush_ep(SYSTEM* system, int num)
{
    EP* ep = ep_data(system, num);
    if (ep == NULL)
        return;

    //cancel owned buffer descriptors
    ep_bd(num, false)->ctl = 0;
    ep_bd(num, true)->ctl = 0;
}

void board_usb_tx(SYSTEM* system, int num, char *data, int size)
{
    EP* ep = ep_data(system, num);

    /* comment for decrease size */
//    if (ep == NULL)
//        return;

    ep->ptr = data;
    ep->processed = 0;
    ep->total = size;
    ep_transfer(system, num, false, ep->ptr, ep->total);
}

void board_usb_rx(SYSTEM* system, int num, char *data, int size)
{
    EP* ep = ep_data(system, num);

    /* comment for decrease size */
//    if (ep == NULL)
//        return;

    ep->ptr = data;
    ep->processed = 0;
    ep->total = size;
    ep_transfer(system, num, false, ep->ptr, ep->total);
}

void board_usb_open_ep(SYSTEM* system, int num, USB_EP_TYPE type, int size)
{
    EP* ep = ep_data(system, num);

    /* comment for decrease size */
//    if(ep == NULL)
//        return;

    ep->ep_size = size;
    ep_bd(num, false)->ctl = 0;
    ep_bd(num, true)->ctl = 0;

    //configure endpoint register
    USB0->ENDPOINT[USB_EP_NUM(num)].ENDPT &= ~(USB_ENDPT_EPCTLDIS_MASK | USB_ENDPT_EPSTALL_MASK | USB_ENDPT_EPHSHK_MASK);
    //disable control transfers for EPs greater than EP0
    switch(type)
    {
    case USB_EP_CONTROL:
        USB0->ENDPOINT[USB_EP_NUM(num)].ENDPT |= USB_ENDPT_EPHSHK_MASK;
        break;
    case USB_EP_ISOCHRON:
        USB0->ENDPOINT[USB_EP_NUM(num)].ENDPT |= USB_ENDPT_EPCTLDIS_MASK;
        break;
    default:
        USB0->ENDPOINT[USB_EP_NUM(num)].ENDPT |= USB_ENDPT_EPCTLDIS_MASK | USB_ENDPT_EPHSHK_MASK;
    }

    //enable operation
    USB0->ENDPOINT[USB_EP_NUM(num)].ENDPT |= ((num & USB_EP_IN) ? USB_ENDPT_EPTXEN_MASK : USB_ENDPT_EPRXEN_MASK);
    //configure EP0 OUT for SETUP
    if (num == 0)
        ep_transfer(system, num, false, system->usbh.setup, USB_MAX_EP0_SIZE);
}

void board_usb_close_ep(SYSTEM* system, int num)
{
    board_usb_flush_ep(system, num);

    USB0->ENDPOINT[USB_EP_NUM(num)].ENDPT &= ~((num & USB_EP_IN) ? USB_ENDPT_EPTXEN_MASK : USB_ENDPT_EPRXEN_MASK);
    system->usbh.dtog &= ~(1 << DTOG_SHIFT(num));
}

void board_usb_set_stall(SYSTEM* system, int num)
{
    board_usb_flush_ep(system, num);
    USB0->ENDPOINT[USB_EP_NUM(num)].ENDPT |= USB_ENDPT_EPSTALL_MASK;
}

void board_usb_clear_stall(SYSTEM* system, int num)
{
    if (ep_data(system, num) == NULL)
        return;

    USB0->ENDPOINT[USB_EP_NUM(num)].ENDPT &= ~USB_ENDPT_EPSTALL_MASK;
    system->usbh.dtog &= ~(1 << DTOG_SHIFT(num));
    //configure EP0 OUT for SETUP
    if (num == 0)
        ep_transfer(system, num, false, system->usbh.setup, USB_MAX_EP0_SIZE);
}

bool board_usb_is_stall(SYSTEM* system, int num)
{
    if (ep_data(system, num) == NULL)
        return false;

    return (USB0->ENDPOINT[USB_EP_NUM(num)].ENDPT & USB_ENDPT_EPSTALL_MASK) >> USB_ENDPT_EPSTALL_SHIFT;
}

bool board_usb_start(SYSTEM* system)
{
    //enable device pullap
    USB0->CONTROL = USB_CONTROL_DPPULLUPNONOTG_MASK;

    return true;
}

void board_usb_stop(SYSTEM* system)
{
   //disable device
   USB0->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK;
}

void board_usb_init(SYSTEM* system)
{
    int i;
    system->usbh.addr = 0;
    system->usbh.addr_pending = false;
    system->usbh.suspended = false;

#if (KINETIS_USB_CRYSTALLESS)
    SIM->CLKDIV2 = 0;
    //IRC48M as clock source
    SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(3);
#else
    //setup USB divisor
    div = kinetis_power_get_core_clock() / (USB_CLOCK >> 1);
    if (div & 1)
        SIM->CLKDIV2 = SIM_CLKDIV2_USBDIV(div - 1) | SIM_CLKDIV2_USBFRAC_MASK;
    else
        SIM->CLKDIV2 = SIM_CLKDIV2_USBDIV((div >> 1) - 1);
    //PLL as clock source
    SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(1);
#endif //KINETIS_USB_CRYSTALLESS

    //enable clock after configuration
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;

#if (KINETIS_USB_CRYSTALLESS)
    //enable IRC48M source
    USB0->CLK_RECOVER_IRC_EN |= USB_CLK_RECOVER_IRC_EN_IRC_EN_MASK | USB_CLK_RECOVER_IRC_EN_REG_EN_MASK;
    //enable clock recovery
    USB0->CLK_RECOVER_CTRL |= USB_CLK_RECOVER_CTRL_CLOCK_RECOVER_EN_MASK;
#endif //KINETIS_USB_CRYSTALLESS

    //hard reset OTG controller
    USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
    __NOP();
    __NOP();

    //set all BDs, owned by MCU
    for (i = 0; i < 16 * 4; ++i)
        BDT[i].ctl = 0;

    //configure buffer descriptor table
    USB0->BDTPAGE1 = (BDT_BASE >> 8) & 0xff;
    USB0->BDTPAGE2 = (BDT_BASE >> 16) & 0xff;
    USB0->BDTPAGE3 = (BDT_BASE >> 24) & 0xff;

    //disable all endpoints
    for (i = 0; i < 16; ++i)
        USB0->ENDPOINT[i].ENDPT = 0;

    //enable USB module
    USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
    //remove tranceiver from suspended state
    USB0->USBCTRL &= ~USB_USBCTRL_SUSP_MASK;

    //clear interrupt flags if already set
    USB0->ISTAT = 0xff;
    USB0->ERRSTAT = 0xbf;

    //close device, probably opened by main device
    board_usb_stop(system);
}

static inline void otg_data(SYSTEM* system, int num, bool odd)
{
    unsigned int transferred;
    EP* ep;
    BD* bd = ep_bd(num, odd);

    if ((bd->ctl & USB_BD_PID_MASK) == USB_BD_PID_SETUP)
    {
        memcpy((uint8_t*)&system->usbd.setup, bd->addr, sizeof(SETUP));
        system->usbh.status_direction = (system->usbd.setup.bmRequestType & BM_REQUEST_DIRECTION) ? 0 : USB_EP_IN;
        usbd_setup(system);
        USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
        return;
    }

    ep = ep_data(system, num);
    transferred = (bd->ctl & USB_BD_BC_MASK) >> USB_BD_BC_SHIFT;

    //closed before
    if (ep == NULL)
        return;

    //status stage of control transfers
    if (transferred == 0 && num == system->usbh.status_direction)
    {
        //SETUP has always DATA0 token PID
        system->usbh.dtog &= ~(1 << 0);
        //set address only after status stage complete
        if (system->usbh.addr_pending)
        {
            USB0->ADDR |= system->usbh.addr;
            system->usbh.addr_pending = false;
        }
        ep_transfer(system, 0, false, system->usbh.setup, USB_MAX_EP0_SIZE);
        return;
    }

    ep->processed += transferred;
    if ((ep->processed >= ep->total) || (transferred < ep->ep_size))
    {
        if(USB_EP_NUM(num) == 0)
        {
            if (num & USB_EP_IN)
                usbd_write_complete(system);
            else
            {
//                system->usbd.data_size = ep->processed;
                usbd_read_complete(system);
            }
        }
        else
        {
            if (num & USB_EP_IN)
                class_write_complete(system);
            else
            {
                system->hidd.size = ep->processed;
                class_read_complete(system);
            }
        }
    }
    else
    {
        if(USB_EP_NUM(num) == 0)
            ep_transfer(system, num, false, system->usbd.buf + ep->processed, ep->total - ep->processed);
        else
        {
            ep_transfer(system, num, false, system->hidd.report + ep->processed, ep->total - ep->processed);
        }
    }

}

static inline void otg_reset(SYSTEM* system)
{
    system->usbh.suspended = false;
    system->usbh.dtog = 0;
    //reset device address
    system->usbh.addr_pending = false;
    USB0->ADDR &= ~USB_ADDR_ADDR_MASK;

    usbd_reset(system);
}

#if (KINETIS_USB_WUP_SUP_ENABLE)
static inline void otg_suspend(SYSTEM* system)
{
    system->usbh.suspended = true;
    //enable only resume and reset
    usbd_suspend(system);
}

static inline void otg_wakeup(SYSTEM* system)
{
    system->usbh.suspended = false;
    //enable data processing, sleep
    usbd_wakeup(system);
}
#endif // KINETIS_USB_WUP_SUP_ENABLE

void board_usb_request(SYSTEM* system)
{
    if (USB0->ISTAT & USB_ISTAT_TOKDNE_MASK)
    {
        USB0->CTL |= USB_CTL_ODDRST_MASK;
        otg_data(system, ((USB0->STAT & USB_STAT_ENDP_MASK) >> USB_STAT_ENDP_SHIFT) | (USB_EP_IN * ((USB0->STAT & USB_STAT_TX_MASK) >> USB_STAT_TX_SHIFT)),
                (USB0->STAT & USB_STAT_ODD_MASK) >> USB_STAT_ODD_SHIFT);
        USB0->ISTAT = USB_ISTAT_TOKDNE_MASK;
        return;
    }

    if (USB0->ISTAT & USB_ISTAT_USBRST_MASK)
    {
        otg_reset(system);
        USB0->ISTAT = USB_ISTAT_USBRST_MASK;
        return;
    }

#if (KINETIS_USB_WUP_SUP_ENABLE)
    if (!system->usbh.suspended && (USB0->ISTAT & USB_ISTAT_SLEEP_MASK))
    {
        otg_suspend(system);
        USB0->ISTAT = USB_ISTAT_SLEEP_MASK;
        return;
    }

    if (system->usbh.suspended && (USB0->ISTAT & USB_ISTAT_RESUME_MASK))
    {
        otg_wakeup(system);
        USB0->ISTAT = USB_ISTAT_RESUME_MASK;
        return;
    }
#endif // KINETIS_USB_WUP_SUP_ENABLE
}

void board_usb_set_address(SYSTEM* system, unsigned int address)
{
    //address will be set after STATUS IN packet
    if (address)
    {
        system->usbh.addr = address;
        system->usbh.addr_pending = true;
    }
}
