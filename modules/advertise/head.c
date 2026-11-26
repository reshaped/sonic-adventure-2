/* the head of ADVERTISE.PRS, the menus and the fake title */
#include "advertise/advertise.h"
#include "system/loader.h"

#pragma section HEAD
static struct program_overlay advertise_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"ADVERTIZ", 3, advertise_init, advertise_exit},
    /* advertise_init sets the step */
    NULL,
};
