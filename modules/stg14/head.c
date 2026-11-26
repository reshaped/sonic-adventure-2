/* the head of STG14.PRS, Radical Highway */
#include "object/set.h"
#include "stg14/radical_highway.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg14_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG14   ", 0, radical_highway_init, radical_highway_exit},
    radical_highway_step,
};
struct object_list radical_highway_objects = {
#include "stg14/head/radical_highway_objects.inc"
};
