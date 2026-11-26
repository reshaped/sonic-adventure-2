/* the head of BOSS_BIGBOGY.PRS, King Boom Boo's arena */
#include "boss_bigbogy/stage.h"
#include "boss_bigbogy/object_kinds.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay boss_bigbogy_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"BIGBOGY ", 0, bigbogy_stage_init, bigbogy_stage_exit},
    bigbogy_stage_step,
};
struct object_list bigbogy_object_list = {
#include "boss_bigbogy/head/bigbogy_object_list.inc"
};
