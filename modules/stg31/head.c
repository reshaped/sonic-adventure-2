/* the head of STG31.PRS, Green Hill */
#include "object/set.h"
#include "stg31/green_hill.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg31_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG31   ", 0, green_hill_init, green_hill_exit},
    green_hill_step,
};
struct object_list green_hill_objects = {
#include "stg31/head/green_hill_objects.inc"
};
