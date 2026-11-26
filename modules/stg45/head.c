/* the head of STG45.PRS, Sand Ocean's two-player stage */
#include "object/set.h"
#include "stg45/object_kinds.h"
#include "stg45/sand_ocean_2p.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg45_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG45   ", 0, sand_ocean_2p_init, sand_ocean_2p_exit},
    sand_ocean_2p_step,
};
struct object_list sand_ocean_2p_objects = {
#include "stg45/head/sand_ocean_2p_objects.inc"
};
