/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: Alexey E. Kramarenko (alexeyk13@yandex.ru)
*/

#ifndef CONFIG_H
#define CONFIG_H

#define LED_PIN                                     P28

#define BUTTON_PIN                                  P9
#define BUTTON_FLAGS                                (PINBOARD_FLAG_DOWN_EVENT | \
                                                     PINBOARD_FLAG_UP_EVENT |   \
                                                     PINBOARD_FLAG_PULL)

#define B_PRESS_MS                                  150
#define B_PRESS_LONG_MS                             1000
#endif // CONFIG_H
