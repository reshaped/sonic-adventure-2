/* the head of STG16.PRS, Wild Canyon */
#include "object/set.h"
#include "stg16/object_kinds.h"
#include "stg16/wild_canyon.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg16_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG16   ", 0, wild_canyon_init, wild_canyon_exit},
    wild_canyon_step,
};
struct object_list wild_canyon_objects = {
#include "stg16/head/wild_canyon_objects.inc"
};
