/* the head of OTHERPRINT.PRS, the logos or the picture whose textures are
 * named tento_ending */
#include "otherprint/otherprint.h"
#include "system/loader.h"

#pragma section HEAD
static struct program_overlay otherprint_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"OTHERPRI", 4, otherprint_init, otherprint_exit},
    otherprint_step,
};
