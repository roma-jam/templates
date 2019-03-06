/*
 * system.h
 *
 *  Created on: 6 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdbool.h>
#include "config.h"
#include "board.h"

typedef enum {
    SYSTEM_MODE_IDLE = 0x00,
    SYSTEM_MODE_RESET,
    SYSTEM_MODE_GOTO_APP
} SYSTEM_MODE;

typedef struct _SYSTEM {
    SYSTEM_MODE mode;
} SYSTEM;

#endif /* SYSTEM_H_ */
