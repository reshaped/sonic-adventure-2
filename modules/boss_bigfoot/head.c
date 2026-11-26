/* the head of BOSS_BIGFOOT.PRS, the Big Foot stage */
#include "boss_bigfoot/stage.h"
#include "boss_bigfoot/object_kinds.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay boss_bigfoot_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"BIGFOOT ", 0, bigfoot_stage_init, bigfoot_stage_exit},
    bigfoot_stage_step,
};
struct object_list bigfoot_object_list = {
#include "boss_bigfoot/head/bigfoot_object_list.inc"
};
