/* the head of STG41.PRS, Wild Canyon's two-player stage */
#include "object/set.h"
#include "stg41/object_kinds.h"
#include "stg41/wild_canyon_2p.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg41_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG41   ", 0, wild_canyon_2p_init, wild_canyon_2p_exit},
    wild_canyon_2p_step,
};
struct object_list wild_canyon_2p_objects = {
#include "stg41/head/wild_canyon_2p_objects.inc"
};
