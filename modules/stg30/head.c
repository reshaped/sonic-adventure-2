/* the head of STG30.PRS, Final Rush */
#include "object/set.h"
#include "stg30/final_rush.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg30_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG30   ", 0, stg30_init, stg30_exit},
    stg30_step,
};
struct object_list stg30_objects = {
#include "stg30/head/stg30_objects.inc"
};
