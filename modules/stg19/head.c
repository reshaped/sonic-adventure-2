/* the head of STG19.PRS, STAGE_VERSUS_SONIC_1, the first fight of Sonic and
 * Shadow */
#include "object/set.h"
#include "stg19/object_kinds.h"
#include "stg19/versus_sonic_1.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg19_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG19   ", 0, versus_sonic_1_init, versus_sonic_1_exit},
    versus_sonic_1_step,
};
struct object_list versus_sonic_1_objects = {
#include "stg19/head/versus_sonic_1_objects.inc"
};
