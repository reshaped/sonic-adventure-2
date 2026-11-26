/* the head of STG21.PRS, Sand Ocean */
#include "object/set.h"
#include "stg21/sand_ocean.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg21_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG21   ", 0, sand_ocean_init, sand_ocean_exit},
    sand_ocean_step,
};
struct object_list sand_ocean_objects = {
#include "stg21/head/sand_ocean_objects.inc"
};
