/* the head of MCWARN.PRS, the memory card warning */
#include "mcwarn/mcwarn.h"
#include "system/loader.h"

#pragma section HEAD
static struct program_overlay mcwarn_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"MCWARN  ", 1, mcwarn_init, mcwarn_exit},
    mcwarn_step,
};
