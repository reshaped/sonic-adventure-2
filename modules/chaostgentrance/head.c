/* the head of CHAOSTGENTRANCE.PRS, a Chao area */
#include "chaostgentrance/entrance.h"
#include "system/loader.h"

#pragma section HEAD
static struct chao_area_head chaostgentrance_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL. CHAOSTGRACE.PRS's head holds
     * the same name */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    "ALSTRACE",
    chaostgentrance_step,
};
