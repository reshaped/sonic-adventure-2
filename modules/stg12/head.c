/* the head of STG12.PRS, Weapons Bed */
#include "object/set.h"
#include "shared/carrier_objects.h"
#include "shared/carrier_stage.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg12_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG12   ", 0, carrier_stage_init, carrier_stage_exit},
    carrier_stage_step,
};
struct object_list carrier_objects = {
#include "stg12/head/carrier_objects.inc"
};
