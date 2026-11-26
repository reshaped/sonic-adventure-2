/* the head of STG04.PRS, White Jungle */
#include "object/set.h"
#include "stg04/white_jungle.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg04_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG04   ", 0, white_jungle_init, white_jungle_exit},
    white_jungle_step,
};
struct object_list white_jungle_objects = {
#include "stg04/head/white_jungle_objects.inc"
};
