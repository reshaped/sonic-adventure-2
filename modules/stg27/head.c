/* the head of STG27.PRS, Lost Colony */
#include "object/set.h"
#include "stg27/object_kinds.h"
#include "stg27/lost_colony.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg27_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG27   ", 0, lost_colony_init, lost_colony_exit},
    lost_colony_step,
};
/* the stage's object list, which lost_colony_init gives to set_start */
struct object_list lost_colony_objects = {
#include "stg27/head/lost_colony_objects.inc"
};
