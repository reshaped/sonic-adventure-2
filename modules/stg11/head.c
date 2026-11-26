/* the head of STG11.PRS, Iron Gate */
#include "object/set.h"
#include "stg11/iron_gate.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg11_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG11   ", 0, iron_gate_init, iron_gate_exit},
    iron_gate_step,
};
struct object_list iron_gate_objects = {
#include "stg11/head/iron_gate_objects.inc"
};
