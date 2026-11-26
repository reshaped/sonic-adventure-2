/* the head of STG40.PRS, Final Chase */
#include "object/set.h"
#include "stg40/object_kinds.h"
#include "stg40/final_chase.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg40_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG40   ", 0, final_chase_init, final_chase_exit},
    final_chase_step,
};
struct object_list final_chase_objects = {
#include "stg40/head/final_chase_objects.inc"
};
