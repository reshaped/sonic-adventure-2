/* the head of ENDING.PRS, the staff roll of the hero or the dark story */
#include "ending/ending.h"
#include "system/loader.h"

#pragma section HEAD
static struct program_overlay ending_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"ENDING  ", 4, ending_init, ending_exit},
    ending_step,
};
