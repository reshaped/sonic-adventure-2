/* the head of CHAOSTGDARK.PRS, a Chao area */
#include "chaostgdark/garden.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct chao_area_head chaostgdark_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    "ALSTDARK",
    chaostgdark_step,
};
struct object_list chaostgdark_objects = {
#include "chaostgdark/head/chaostgdark_objects.inc"
};
