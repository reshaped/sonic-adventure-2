/* the head of STG36.PRS, Tails's route through Cannon's Core */
#include "object/set.h"
#include "shared/core_objects.h"
#include "stg36/core_tails.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg36_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG36   ", 0, core_tails_init, core_tails_exit},
    core_tails_step,
};
struct object_list core_tails_objects = {
#include "stg36/head/core_tails_objects.inc"
};
