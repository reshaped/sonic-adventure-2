/* the head of STG10.PRS, Metal Harbor */
#include "object/set.h"
#include "stg10/metal_harbor.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg10_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG10   ", 0, metal_harbor_init, metal_harbor_exit},
    metal_harbor_step,
};
struct object_list metal_harbor_objects = {
#include "stg10/head/metal_harbor_objects.inc"
};
