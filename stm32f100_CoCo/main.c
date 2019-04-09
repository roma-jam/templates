/*
 * main.c
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include <string.h>
#include "board.h"
#include "system.h"

#include "userspace/stdlib.h"
#include "ucore.h"

#if (DFU_DEBUG)
#include "dbg.h"
#endif // DFU_DEBUG

// I do not use IDE setting, so later I turn on this setting from makefile during build
#ifdef __COVERAGESCANNER__

//__coveragescanner_save();

//__coveragescanner_clear();

#endif

// here I setup communication link
#if defined( __COVERAGESCANNER__ )
static int csfputs(const char *s, void *stream)
{
    printf("%s", s);
    return 0;
}

static void *csfopenappend(const char *path)
{
    return (void*)1;
}

static int csfclose(void *fp)
{
    return 1;
}

#endif

//unsigned int t = 12;

int test_function(int a, int b, int c)
{
    if(a > b)
        return c;

    if(a < b)
        return c + 1;

    if (a == b)
        return 0;
}

void main()
{
    board_init();

#if (DFU_DEBUG)
    board_dbg_init();
//    printf("CoCo Template, v%d.%d, %s\n", VERSION >> 8, VERSION & 0xff, __BUILD_TIME);
//    printf("%s core, %d MHz\n", __MCU, power_get_core_clock() / 1000000);
#endif // DFU_DEBUG

#ifdef __COVERAGESCANNER__
  __coveragescanner_set_custom_io(
          NULL,
          csfputs,
          csfopenappend,
          NULL,
          NULL,
          csfclose,
          NULL );
#endif


      test_function(10, 5, 3);
      test_function(1, 102, 2);
      test_function(17, 17, 3);

//  uint8_t* data = malloc(221);
//    printf("malloc ");
//    if(data == NULL)
//        printf("failure\n");
//    else
//    {
//        printf("ok, addr %#X\n", data);
//        free(data);
//    }
//
//    ucore_stat();
//    t += 12;
//    printf("t=%d\n", t);

#ifdef __COVERAGESCANNER__
  /* Saves the execution report */
  __coveragescanner_save();
#endif

    for(;;) { }
}

