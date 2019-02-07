/*
 * core.h
 *
 *  Created on: 14 рту. 2017 у.
 *      Author: RLeonov
 */

#ifndef CORE_CORE_H_
#define CORE_CORE_H_

#include "board/kinetis/kinetis.h"
#include "board/lpc/lpc.h"
#include "board/stm32/stm32.h"

#if defined(CORTEX_M3) || defined(CORTEX_M0) || defined(CORTEX_M4)
#ifndef CORTEX_M
#define CORTEX_M
#endif //CORTEX_M
#endif //defined(CORTEX_M3) || defined(CORTEX_M0) || defined(CORTEX_M4)

#ifdef CORTEX_M
#ifndef SRAM_BASE
#define SRAM_BASE                0x20000000
#endif
#endif

#endif /* CORE_CORE_H_ */
