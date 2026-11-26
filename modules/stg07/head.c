/* the head of STG07.PRS, Aquatic Mine */
#include "object/set.h"
#include "stg07/object_kinds.h"
#include "stg07/aquatic_mine.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg07_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG07   ", 0, aquatic_mine_init, aquatic_mine_exit},
    aquatic_mine_step,
};
struct object_list aquatic_mine_objects = {
#include "stg07/head/aquatic_mine_objects.inc"
};
