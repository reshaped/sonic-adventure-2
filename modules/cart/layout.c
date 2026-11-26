/* each course's layout of pieces, and cart_select_layout, which copies one
 * for cart_setup.pieces */
#include <sg_xpt.h>
#include <string.h>

#include "cart/course.h"
#include "cart/track.h"

/* the byte past a layout's last piece */
#define LAYOUT_END (-1)

/* each course's pieces, by their index in cart_pieces, in driving order from
 * the start */
static Sint8 tails_pieces[321] = {
#include "cart/layout/tails_pieces.inc"
};
static Sint8 rouge_pieces[369] = {
#include "cart/layout/rouge_pieces.inc"
};
static Sint8 beginner_pieces[88] = {
#include "cart/layout/beginner_pieces.inc"
};
static Sint8 standard_pieces[112] = {
#include "cart/layout/standard_pieces.inc"
};
static Sint8 expert_pieces[123] = {
#include "cart/layout/expert_pieces.inc"
};

struct route {
    const Sint8 *pieces;
    Sint32 count;
};
#define ROUTE(pieces) {pieces, sizeof(pieces)}
static struct route routes[] = {ROUTE(tails_pieces), ROUTE(rouge_pieces),
                                ROUTE(beginner_pieces), ROUTE(standard_pieces),
                                ROUTE(expert_pieces)};
static Sint8 layout[CART_SECTIONS];

void cart_select_layout(int mode)
{
    Sint8 *piece;
    cart_setup.pieces = NULL;
    memset(layout, LAYOUT_END, sizeof(layout));
    if (mode < CART_DOWNLOAD) {
        memcpy(layout, routes[mode].pieces, routes[mode].count);
    } else {
#ifdef NONMATCHING
        /* this side bounds the copy by layout's size */
        cart_download_copy_layout(layout, sizeof(layout));
#else
        cart_download_copy_layout(layout);
#endif
    }
    cart_setup.piece_count = 0;
    for (piece = layout; piece < layout + sizeof(layout); piece++) {
        if (*piece != LAYOUT_END) {
            cart_setup.piece_count++;
        } else {
            break;
        }
    }
    cart_setup.pieces = layout;
}
