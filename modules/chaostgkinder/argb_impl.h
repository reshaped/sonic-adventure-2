/* expand_argb4444 widens an ARGB4444 color to ARGB8888. each 4-bit channel
 * becomes a byte that holds it in both nibbles */
#ifndef CHAOSTGKINDER_ARGB_IMPL_H
#define CHAOSTGKINDER_ARGB_IMPL_H
#include <sg_xpt.h>

static Uint32 expand_argb4444(Uint16 color)
{
    return (((color & 0xf000UL) * 0x11) << 12) |
           (((color & 0x0f00UL) * 0x11) << 8) |
           (((color & 0x00f0UL) * 0x11) << 4) | ((color & 0x000fUL) * 0x11);
}

#endif
