/*
 * kinetis_otg.h
 *
 *  Created on: 6 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#ifndef KINETIS_OTG_H
#define KINETIS_OTG_H

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
  char setup[USB_MAX_EP0_SIZE];
  uint32_t dtog;
  uint8_t status_direction, addr;
  bool suspended, addr_pending;
} USBH;

#endif /* KINETIS_OTG_H */
