/* the export's text: a character takes one byte, the slot that lists its
 * code in book_text.c's alphabet. a mark, the code BOOK_TEXT_MARK << 8 | n,
 * takes the two bytes BOOK_TEXT_MARK and n */
#ifndef CHAOSTGKINDER_BOOK_TEXT_H
#define CHAOSTGKINDER_BOOK_TEXT_H
#include <sg_xpt.h>

#define BOOK_TEXT_MARK 0xff

/* write src, up to its 0, into dst as the export's text, then a 0. dst
 * needs limit + 1 bytes. return how many bytes precede the 0: at most limit.
 * each code read takes one of limit, and a mark two. a code no slot lists
 * is dropped, as is a mark that finds one of limit left */
Sint32 book_encode_text(Uint8 *dst, const Uint16 *src, Sint32 limit);
#endif
