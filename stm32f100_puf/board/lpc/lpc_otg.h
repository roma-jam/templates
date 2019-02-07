/*
 * lpc_otg.h
 *
 *  Created on: 14 рту. 2017 у.
 *      Author: RLeonov
 */

#ifndef LPC_OTG_H_
#define LPC_OTG_H_

#include <stdint.h>
#include <stdbool.h>
#include "usb.h"


//0 control, 1 for CCID class
#define USB_EP_COUNT_MAX            2

typedef struct {
    void* ptr;
    unsigned int ep_size, processed, total;
    uint16_t mps;
} EP;

typedef struct _USB {
  EP out[USB_EP_COUNT_MAX];
  EP in[USB_EP_COUNT_MAX];
  USB_SPEED speed;
  char setup[USB_MAX_EP0_SIZE];
  uint8_t addr;

  bool suspended;
  bool addr_pending;
} USBH;

#endif /* LPC_OTG_H_ */
