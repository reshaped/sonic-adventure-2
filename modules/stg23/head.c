/* the head of STG23.PRS, Hidden Base */
#include "object/set.h"
#include "stg23/hidden_base.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg23_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG23   ", 0, stg23_init, stg23_exit},
    stg23_step,
};
struct object_list stg23_objects = {
#include "stg23/head/stg23_objects.inc"
};
