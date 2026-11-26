/* the head of STG35.PRS, Eggman's route through Cannon's Core */
#include "object/set.h"
#include "shared/core_objects.h"
#include "stg35/core_eggman.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg35_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG35   ", 0, core_eggman_init, core_eggman_exit},
    core_eggman_step,
};
struct object_list core_eggman_objects = {
#include "stg35/head/core_eggman_objects.inc"
};
