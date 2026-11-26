/* the head of STG42.PRS, the second fight of Sonic and Shadow */
#include "object/set.h"
#include "stg42/versus_sonic_2.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg42_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG42   ", 0, versus_sonic_2_init, versus_sonic_2_exit},
    versus_sonic_2_step,
};
struct object_list versus_sonic_2_objects = {
#include "stg42/head/versus_sonic_2_objects.inc"
};
