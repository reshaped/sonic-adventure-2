/* the head of STG39.PRS, Mission Street's two-player stage */
#include "object/set.h"
#include "stg39/object_kinds.h"
#include "stg39/mission_street_2p.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg39_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG39   ", 0, mission_street_2p_init, mission_street_2p_exit},
    mission_street_2p_step,
};
struct object_list mission_street_2p_objects = {
#include "stg39/head/mission_street_2p_objects.inc"
};
