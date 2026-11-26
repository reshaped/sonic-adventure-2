/* the head of BOSS_LAST1.PRS, the Biolizard's chamber */
#include "boss_last1/stage.h"
#include "boss_last1/object_kinds.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay boss_last1_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"LASTBS1 ", 0, last1_stage_init, last1_stage_exit},
    last1_stage_step,
};
struct object_list last1_object_list = {
#include "boss_last1/head/last1_object_list.inc"
};
