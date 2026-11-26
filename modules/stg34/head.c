/* the head of STG34.PRS, Sonic's route through Cannon's Core */
#include "object/set.h"
#include "shared/core_objects.h"
#include "stg34/core_sonic.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg34_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG34   ", 0, core_sonic_init, core_sonic_exit},
    core_sonic_step,
};
struct object_list core_sonic_objects = {
#include "stg34/head/core_sonic_objects.inc"
};
