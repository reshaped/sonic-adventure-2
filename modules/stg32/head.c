/* the head of STG32.PRS, Meteor Herd */
#include "object/set.h"
#include "stg32/object_kinds.h"
#include "stg32/meteor_herd.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg32_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG32   ", 0, meteor_herd_init, meteor_herd_exit},
    meteor_herd_step,
};
struct object_list meteor_herd_objects = {
#include "stg32/head/meteor_herd_objects.inc"
};
