/* the head of STG28.PRS, Pyramid Cave */
#include "object/set.h"
#include "stg28/object_kinds.h"
#include "stg28/pyramid_cave.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg28_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG28   ", 0, pyramid_cave_init, pyramid_cave_exit},
    pyramid_cave_step,
};
struct object_list pyramid_cave_objects = {
#include "stg28/head/pyramid_cave_objects.inc"
};
