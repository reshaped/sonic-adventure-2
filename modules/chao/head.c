/* the head of CHAO.PRS, the Chao world */
#include "chao/garden.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay chao_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"ALGARDEN", 0, chao_init, chao_exit},
    chao_step,
};
