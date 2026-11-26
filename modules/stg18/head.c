/* the head of STG18.PRS, Dry Lagoon */
#include "object/set.h"
#include "stg18/dry_lagoon.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg18_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG18   ", 0, dry_lagoon_init, dry_lagoon_exit},
    dry_lagoon_step,
};
struct object_list dry_lagoon_objects = {
#include "stg18/head/dry_lagoon_objects.inc"
};
