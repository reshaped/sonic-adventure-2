/* the head of EVENT.PRS, the event player */
#include "system/loader.h"

#include "event/scene.h"

#pragma section HEAD
static struct program_overlay event_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"EVENT   ", 2, event_init, event_exit},
    event_step,
};
