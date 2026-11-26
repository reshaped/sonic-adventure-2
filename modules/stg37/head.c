/* the head of STG37.PRS, Rouge's route through Cannon's Core */
#include "object/set.h"
#include "shared/core_objects.h"
#include "stg37/core_rouge.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay core_rouge_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG37   ", 0, core_rouge_init, core_rouge_exit},
    core_rouge_step,
};
/* the stage's init gives the list to set_start, by its name */
struct object_list core_rouge_objects = {
#include "stg37/head/core_rouge_objects.inc"
};
