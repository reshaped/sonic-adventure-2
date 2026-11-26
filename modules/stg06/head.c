/* the head of STG06.PRS, Sky Rail */
#include "object/set.h"
#include "stg06/object_kinds.h"
#include "stg06/sky_rail.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg06_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG06   ", 0, sky_rail_init, sky_rail_exit},
    sky_rail_step,
};
struct object_list sky_rail_objects = {
#include "stg06/head/sky_rail_objects.inc"
};
