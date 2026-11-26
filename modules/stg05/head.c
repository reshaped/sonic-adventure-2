/* the head of STG05.PRS, Pumpkin Hill */
#include "object/set.h"
#include "stg05/object_kinds.h"
#include "stg05/pumpkin_hill.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg05_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG05   ", 0, pumpkin_hill_init, pumpkin_hill_exit},
    pumpkin_hill_step,
};
struct object_list pumpkin_hill_objects = {
#include "stg05/head/pumpkin_hill_objects.inc"
};
struct object_list pumpkin_hill_2p_objects = {
#include "stg05/head/pumpkin_hill_2p_objects.inc"
};
