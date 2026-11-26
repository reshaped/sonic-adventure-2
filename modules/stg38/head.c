/* the head of STG38.PRS, Knuckles's route through Cannon's Core */
#include "object/set.h"
#include "shared/core_objects.h"
#include "stg38/core_knuckles.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay core_knuckles_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG38   ", 0, core_knuckles_init, core_knuckles_exit},
    core_knuckles_step,
};
/* the stage's init gives the list to set_start, by its name */
struct object_list core_knuckles_objects = {
#include "stg38/head/core_knuckles_objects.inc"
};
