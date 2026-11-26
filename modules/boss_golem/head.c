/* the head of BOSS_GOLEM.PRS, the Egg Golem stage of the hero story */
#include "boss_golem/stage.h"
#include "object/set.h"
#include "shared/golem_object_kinds.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay boss_golem_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"GOLEM   ", 0, golem_stage_init, golem_stage_exit},
    golem_stage_step,
};
struct object_list golem_object_list = {
#include "boss_golem/head/golem_object_list.inc"
};
