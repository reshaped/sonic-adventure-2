/* the colors of a chunk model's NJD_CV_D8 vertices, and the midpoint of two
 * points */
#ifndef SHARED_VERTEX_COLORS_H
#define SHARED_VERTEX_COLORS_H
#include <ninja.h>
#include <sg_xpt.h>

/* vlist's first chunk must be an NJD_CV_D8 chunk of at least one vertex,
 * unchecked. the three vlist functions return 0.
 *
 * vlist_set_color sets every vertex's color to color. vlist_shade multiplies
 * each byte of every vertex's color by the same byte of color. it divides each
 * product by 256, rounding down. vlist_copy_shaded writes the chunk to dst, its
 * colors shaded as vlist_shade shades them. it then copies the word that
 * follows the chunk. it leaves vlist unchanged and doesn't read mask.
 *
 * vlist_shade and vlist_copy_shaded never return unless the word that follows
 * the chunk is NJD_CE */
Sint32 vlist_set_color(Sint32 *vlist, Uint32 color);
Sint32 vlist_shade(Sint32 *vlist, Uint32 color);
Sint32 vlist_copy_shaded(Sint32 *dst, Sint32 *vlist, Uint32 color, Uint32 mask);
/* the mask vlist_copy_shaded's callers pass, which it doesn't read */
#define VLIST_UNREAD_MASK 0xffff
/* set mid to the point halfway between from and to */
void point_midpoint(NJS_POINT3 *from, NJS_POINT3 *to, NJS_POINT3 *mid);

#endif
