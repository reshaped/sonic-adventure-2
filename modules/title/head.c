/* the head of TITLE.PRS, the title screen, or the trial's title and the screens
 * before it */
#include "system/loader.h"
#include "title/title.h"

#pragma section HEAD
static struct program_overlay title_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"TITLE   ", 1, title_init, title_exit},
    title_step,
};
