/* the head of STG26.PRS, Egg Quarters */
#include "object/set.h"
#include "stg26/egg_quarters.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg26_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG26   ", 0, egg_quarters_init, egg_quarters_exit},
    egg_quarters_step,
};
/* the stage's object list, which egg_quarters_init gives to set_start */
struct object_list egg_quarters_objects = {
#include "stg26/head/egg_quarters_objects.inc"
};
