/*
 * config.h
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//------------------------------ board ------------------------------------------------
#define SP_PTR                                      (SRAM_BASE + SRAM_SIZE)
#define DELAY_COEF_MS                               7000
#define DELAY_COEF_US                               7

//------------------------------ debug ------------------------------------------------
#define SYS_DEBUG                                   1

//------------------------------- USB -------------------------------------------------
#define USB_ENABLE                                  0

#if (SYS_DEBUG)
#define USB_DEBUG_ERRORS                            0
#define USB_DEBUG_REQUESTS                          0
#define USB_DEBUG_SETUP_REQUESTS                    0
#define USB_DEBUG_CLASS_REQUESTS                    0

#define RECOVERY_LAYER_DEBUG                        0
#define MEMORY_DEBUG                                0

#endif // DFU SYS_DEBUG


#endif /* CONFIG_H_ */
