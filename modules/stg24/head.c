/* the head of STG24.PRS, Eternal Engine */
#include "object/set.h"
#include "stg24/object_kinds.h"
#include "system/loader.h"
#include "stg24/eternal_engine.h"

#pragma section HEAD
static struct stage_overlay stg24_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG24   ", 0, stg24_init, stg24_exit},
    stg24_step,
};
struct object_list stg24_objects = {
#include "stg24/head/stg24_objects.inc"
};
