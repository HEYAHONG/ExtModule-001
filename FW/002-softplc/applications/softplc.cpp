#include "main.h"
#include "hbox.h"

enum
{
    SOFTPLC_LOOP_START=1,
    SOFTPLC_LOOP_EXIT,
};

HSTACKLESSCOROUTINE_BLOCK_START(softplc)
hsoftplc_init();
hstacklesscoroutine_yield_with_label(SOFTPLC_LOOP_START);
hsoftplc_loop();
hstacklesscoroutine_goto_label(SOFTPLC_LOOP_START);
hstacklesscoroutine_yield_with_label(SOFTPLC_LOOP_EXIT);
HSTACKLESSCOROUTINE_BLOCK_END(softplc)

