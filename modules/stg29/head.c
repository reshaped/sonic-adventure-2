/* the head of STG29.PRS, STAGE_VERSUS_MECH_2, the second fight of Tails's and
 * Eggman's walkers */
#include "object/set.h"
#include "stg29/object_kinds.h"
#include "stg29/versus_mech_2.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg29_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG29   ", 0, versus_mech_2_init, versus_mech_2_exit},
    versus_mech_2_step,
};
struct object_list versus_mech_2_objects = {
#include "stg29/head/versus_mech_2_objects.inc"
};
