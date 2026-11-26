/* the head of STG09.PRS, Prison Lane */
#include "object/set.h"
#include "stg09/prison_lane.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg09_head HEAD_SECTION = {
    /* the head names the overlay STG10, as STG10.PRS's does. the name takes
     * all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG10   ", 0, prison_lane_init, prison_lane_exit},
    prison_lane_step,
};
struct object_list prison_lane_objects = {
#include "stg09/head/prison_lane_objects.inc"
};
