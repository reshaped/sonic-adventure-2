/* cart_select_layout over the stub build's layouts, where each course's
 * pieces are its datum's count of piece 0: the course's layout holds them
 * all, and LAYOUT_END in every byte past them. argv gives each course's
 * count, in cart_course_mode's order */
#include <assert.h>
#include <stdlib.h>

#include "cart/layout.c"

struct cart_setup cart_setup;

/* the layout the card's course gives here */
enum { DOWNLOAD_PIECES = 7, DOWNLOAD_PIECE = 3 };

#ifdef NONMATCHING
void cart_download_copy_layout(void *to, Uint32 capacity)
{
    assert(capacity == CART_SECTIONS);
    memset(to, DOWNLOAD_PIECE, DOWNLOAD_PIECES);
}
#else
void cart_download_copy_layout(void *to)
{
    memset(to, DOWNLOAD_PIECE, DOWNLOAD_PIECES);
}
#endif

static void selects(int mode, int count, int piece)
{
    int i;

    cart_select_layout(mode);
    assert(cart_setup.piece_count == count);
    for (i = 0; i < CART_SECTIONS; i++) {
        assert(cart_setup.pieces[i] == (i < count ? piece : LAYOUT_END));
    }
}

int main(int argc, char **argv)
{
    int mode;

    assert(argc == 1 + CART_DOWNLOAD);
    for (mode = CART_TAILS; mode < CART_DOWNLOAD; mode++) {
        selects(mode, atoi(argv[1 + mode]), 0);
    }
    selects(CART_DOWNLOAD, DOWNLOAD_PIECES, DOWNLOAD_PIECE);
    return 0;
}
