/* the head of CART.PRS, the overlay of STAGE_CART_1 and STAGE_CART_2 */
#include "cart/stage.h"
#include "object/set.h"
#include "system/loader.h"

#pragma section HEAD
static struct stage_overlay cart_head HEAD_SECTION = {
    /* the name takes all 8 bytes, with no NUL */
    /* NOLINTNEXTLINE(clang-diagnostic-unterminated-string-initialization) */
    {"CART    ", 0, cart_init, cart_exit},
    cart_step,
};
struct object_list cart_objects = {
#include "cart/head/cart_objects.inc"
};
