/* the head of STG03.PRS, Green Forest */
#include "object/set.h"
#include "stg03/green_forest.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg03_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG03   ", 0, green_forest_init, green_forest_exit},
    green_forest_step,
};
struct object_list green_forest_objects = {
#include "stg03/head/green_forest_objects.inc"
};
