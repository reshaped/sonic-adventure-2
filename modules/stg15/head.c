/* the head of STG15.PRS */
#define CARRIER_2P
#include "object/set.h"
#include "shared/carrier_objects.h"
#include "shared/carrier_stage.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg15_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG15   ", 0, carrier_stage_init, carrier_stage_exit},
    carrier_stage_step,
};
struct object_list carrier_objects = {
#include "stg15/head/carrier_objects.inc"
};
