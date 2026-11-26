/* the GCC build's reads of a downloaded kart pack, whose words are
 * little-endian at any alignment */
#ifndef CART_DOWNLOAD_FORMAT_IMPL_H
#define CART_DOWNLOAD_FORMAT_IMPL_H
#include <sg_xpt.h>

/* the most bytes a kart pack decodes to: the size of download.c's buffer */
#define CART_PACK_CAPACITY 0x19000U

struct cart_region {
    Uint32 offset;
    Uint32 size;
};

static Uint32 cart_word(const Uint8 *p)
{
    return (Uint32)p[0] | (Uint32)p[1] << 8 | (Uint32)p[2] << 16 |
           (Uint32)p[3] << 24;
}

/* whether size bytes from offset lie within limit bytes */
static Bool cart_range(Uint32 offset, Uint32 size, Uint32 limit)
{
    return offset <= limit && size <= limit - offset;
}

/* whether each of the count parts starts at header_bytes or later, ends by
 * limit, and overlaps no other. a part of size 0 takes no bytes, and passes */
static Bool cart_regions(const struct cart_region *parts, unsigned count,
                         Uint32 header_bytes, Uint32 limit)
{
    unsigned i, j;
    for (i = 0; i < count; ++i) {
        if (!parts[i].size) {
            continue;
        }
        if (parts[i].offset < header_bytes ||
            !cart_range(parts[i].offset, parts[i].size, limit)) {
            return FALSE;
        }
        for (j = 0; j < i; ++j) {
            if (parts[j].size &&
                parts[i].offset < parts[j].offset + parts[j].size &&
                parts[j].offset < parts[i].offset + parts[i].size) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/* whether the size bytes at p hold a NUL */
static Bool cart_string(const Uint8 *p, Uint32 size)
{
    while (size--) {
        if (*p++ == 0) {
            return TRUE;
        }
    }
    return FALSE;
}
#endif
