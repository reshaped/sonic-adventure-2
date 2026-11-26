/* the head of STG22.PRS, Crazy Gadget */
#include "object/set.h"
#include "stg22/crazy_gadget.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg22_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG22   ", 0, crazy_gadget_init, crazy_gadget_exit},
    crazy_gadget_step,
};
struct object_list crazy_gadget_objects = {
#include "stg22/head/crazy_gadget_objects.inc"
};
