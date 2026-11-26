/* the head of STG33.PRS, STAGE_VERSUS_KNUCKLES, the fight of Knuckles and
 * Rouge */
#include "object/set.h"
#include "stg33/object_kinds.h"
#include "stg33/knuckles_rouge.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg33_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG33   ", 0, knuckles_rouge_init, knuckles_rouge_exit},
    knuckles_rouge_step,
};
struct object_list knuckles_rouge_objects = {
#include "stg33/head/knuckles_rouge_objects.inc"
};
