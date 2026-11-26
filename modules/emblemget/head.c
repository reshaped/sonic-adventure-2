/* the head of EMBLEMGET.PRS, the emblem screen */
#include "emblemget/emblemget.h"
#include "system/loader.h"

#pragma section HEAD
static struct program_overlay emblemget_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"EMBLEM_G", 5, emblemget_init, emblemget_exit},
    emblemget_step,
};
