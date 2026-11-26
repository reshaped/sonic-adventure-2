/* the head of BOSS_FDOG.PRS, the Flying Dog stage */
#include "boss_fdog/stage.h"
#include "boss_fdog/object_kinds.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay boss_fdog_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"FLYINGD ", 0, fdog_stage_init, fdog_stage_exit},
    fdog_stage_step,
};
struct object_list fdog_object_list = {
#include "boss_fdog/head/fdog_object_list.inc"
};
