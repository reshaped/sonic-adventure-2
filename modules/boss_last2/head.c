/* the head of BOSS_LAST2.PRS, Finalhazard's orbit */
#include "boss_last2/stage.h"
#include "boss_last2/object_kinds.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay boss_last2_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"LASTBS2 ", 0, last2_stage_init, last2_stage_exit},
    last2_stage_step,
};
/* no code reads the list: last2_stage_init starts no set objects */
static struct object_list last2_object_list = {
#include "boss_last2/head/last2_object_list.inc"
};
