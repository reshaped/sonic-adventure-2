/* the head of CHAOSTGKINDER.PRS, a Chao area */
#include "chaostgkinder/rooms.h"
#include "system/loader.h"

#pragma section HEAD
static struct chao_area_head chaostgkinder_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    "ALSTKNDR",
    chaostgkinder_step,
};
