/* the head of STG08.PRS, Security Hall */
#include "object/set.h"
#include "stg08/security_hall.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg08_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG08   ", 0, security_hall_init, security_hall_exit},
    security_hall_step,
};
struct object_list security_hall_objects = {
#include "stg08/head/security_hall_objects.inc"
};
