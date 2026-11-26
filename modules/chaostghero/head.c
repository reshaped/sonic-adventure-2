/* the head of CHAOSTGHERO.PRS, a Chao area */
#include "chaostghero/garden.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct chao_area_head chaostghero_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    "ALSTHERO",
    chaostghero_step,
};
struct object_list chaostghero_objects = {
#include "chaostghero/head/chaostghero_objects.inc"
};
