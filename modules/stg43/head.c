/* the head of STG43.PRS, Cosmic Wall */
#include "object/set.h"
#include "stg43/object_kinds.h"
#include "stg43/cosmic_wall.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg43_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG43   ", 0, cosmic_wall_init, cosmic_wall_exit},
    cosmic_wall_step,
};
struct object_list cosmic_wall_objects = {
#include "stg43/head/cosmic_wall_objects.inc"
};
