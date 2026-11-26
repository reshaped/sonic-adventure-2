/* the head of STG17.PRS, Mission Street */
#include "object/set.h"
#include "stg17/object_kinds.h"
#include "stg17/mission_street.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg17_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG17   ", 0, mission_street_init, mission_street_exit},
    mission_street_step,
};
struct object_list mission_street_objects = {
#include "stg17/head/mission_street_objects.inc"
};
