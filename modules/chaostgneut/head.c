/* the head of CHAOSTGNEUT.PRS, a Chao area */
#include "chaostgneut/garden.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct chao_area_head chaostgneut_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    "ALSTNEUT",
    chaostgneut_step,
};
struct object_list chaostgneut_objects = {
#include "chaostgneut/head/chaostgneut_objects.inc"
};
