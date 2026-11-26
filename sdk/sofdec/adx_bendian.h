/* ADX_LE16 and ADX_LE32 read a little-endian value from bytes at any
 * address. the AU and AIFF readers read each big-endian field with them,
 * then swap it with ADX_SWAP16 or ADX_SWAP32. they compare a chunk's name as
 * read */
#ifndef ADX_BENDIAN_H
#define ADX_BENDIAN_H
#include <sg_xpt.h>

#define ADX_LE16(p) ((p)[0] | ((p)[1] << 8))
#define ADX_LE32(p)                                                            \
    ((Uint32)(p)[0] | ((Uint32)(p)[1] << 8) | ((Uint32)(p)[2] << 16) |         \
     ((Uint32)(p)[3] << 24))
#define ADX_SWAP16(v) ((((v) >> 8) & 0xff) | (((v) << 8) & 0xff00))
#define ADX_SWAP32(v)                                                          \
    ((((v) >> 24) & 0xff) | (((v) >> 8) & 0xff00) |                            \
     (((Uint32)(v) << 8) & 0xff0000) | ((Uint32)(v) << 24))

#endif
