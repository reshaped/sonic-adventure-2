/* the head of STG20.PRS, STAGE_VERSUS_MECH_1, the first fight of Tails's and
 * Eggman's walkers */
#define CARRIER_DUEL
#include "object/set.h"
#include "shared/carrier_objects.h"
#include "shared/carrier_stage.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg20_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG20   ", 0, carrier_stage_init, carrier_stage_exit},
    carrier_stage_step,
};
struct object_list carrier_objects = {
#include "stg20/head/carrier_objects.inc"
};
