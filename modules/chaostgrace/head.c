/* the head of CHAOSTGRACE.PRS, a Chao area */
#include "chaostgrace/garden.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct chao_area_head chaostgrace_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    "ALSTRACE",
    chaostgrace_step,
};
struct object_list chaostgrace_objects = {
#include "chaostgrace/head/chaostgrace_objects.inc"
};
