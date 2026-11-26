/* the head of CHAOSTGLOBBY.PRS, a Chao area */
#include "chaostglobby/lobby.h"
#include "system/loader.h"

#pragma section HEAD
static struct chao_area_head chaostglobby_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    "ALSTLOBY",
    chaostglobby_step,
};
