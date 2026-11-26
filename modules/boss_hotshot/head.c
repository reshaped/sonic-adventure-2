/* the head of BOSS_HOTSHOT.PRS, the Hot Shot stage */
#include "boss_hotshot/stage.h"
#include "boss_hotshot/object_kinds.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay boss_hotshot_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"BOSS0   ", 0, hotshot_stage_init, hotshot_stage_exit},
    hotshot_stage_step,
};
struct object_list hotshot_object_list = {
#include "boss_hotshot/head/hotshot_object_list.inc"
};
