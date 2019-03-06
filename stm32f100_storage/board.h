/*
 * board.h
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdbool.h>
#include "config.h"

#include "board/stm32/stm32.h"

#if defined(STM32)
#include "board/stm32/stm32_pin.h"
#include "board/stm32/stm32_power.h"
#if (USB_ENABLE)
#include "board/stm32/stm32_usb.h"
#endif // USB_ENABLE
#endif

#include "system_private.h"

#if (USB_ENABLE)
#include "usb.h"
#endif // USB_ENABLE

//main
extern void board_init();
extern void board_reset();

//dbg
extern void board_dbg_init();
extern void board_dbg(const char *const buf, unsigned int size);

//delay
extern void delay_us(unsigned int us);
extern void delay_ms(unsigned int ms);

#if (USB_ENABLE)
//USB device
extern void board_usb_init(SYSTEM* system);
extern bool board_usb_start(SYSTEM* system);
extern void board_usb_stop(SYSTEM* system);
extern void board_usb_request(SYSTEM* system);
extern void board_usb_set_address(SYSTEM* system, unsigned int address);

//USB endpoint
extern void board_usb_open_ep(SYSTEM* system, int num, USB_EP_TYPE type, int size);
extern void board_usb_close_ep(SYSTEM* system, int num);
extern void board_usb_flush_ep(SYSTEM* system, int num);
extern void board_usb_tx(SYSTEM* system, int num, char *data, int size);
extern void board_usb_rx(SYSTEM* system, int num, char* data, int size);
extern void board_usb_set_stall(SYSTEM* system, int num);
extern void board_usb_clear_stall(SYSTEM* system, int num);
extern bool board_usb_is_stall(SYSTEM* system, int num);
#endif // USB_ENABLE

#endif /* BOARD_H_ */
