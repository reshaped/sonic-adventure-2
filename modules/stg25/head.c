/* the head of STG25.PRS, Death Chamber */
#include "object/set.h"
#include "stg25/object_kinds.h"
#include "stg25/death_chamber.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg25_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG25   ", 0, death_chamber_init, death_chamber_exit},
    death_chamber_step,
};
struct object_list death_chamber_objects = {
#include "stg25/head/death_chamber_objects.inc"
};
