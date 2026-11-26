/* the head of STG44.PRS, Mad Space */
#include "object/set.h"
#include "stg44/object_kinds.h"
#include "stg44/mad_space.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg44_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG44   ", 0, mad_space_init, mad_space_exit},
    mad_space_step,
};
struct object_list mad_space_objects = {
#include "stg44/head/mad_space_objects.inc"
};
