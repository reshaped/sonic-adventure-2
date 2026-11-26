/* the head of STG13.PRS, City Escape */
#include "object/set.h"
#include "stg13/city_escape.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay stg13_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"STG13   ", 0, city_escape_init, city_escape_exit},
    city_escape_step,
};
struct object_list city_escape_objects = {
#include "stg13/head/city_escape_objects.inc"
};
