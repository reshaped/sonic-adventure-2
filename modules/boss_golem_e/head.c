/* the head of BOSS_GOLEM_E.PRS, the Egg Golem stage of the dark story */
#include "boss_golem_e/stage.h"
#include "object/set.h"
#include "shared/golem_object_kinds.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay boss_golem_e_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"GOLEME  ", 0, golem_e_stage_init, golem_e_stage_exit},
    golem_e_stage_step,
};
struct object_list golem_e_object_list = {
#include "boss_golem_e/head/golem_e_object_list.inc"
};
