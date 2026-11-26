/* texture sizes in bytes, indexed by a level: the log2 of the texture's
 * shorter side */
#include <nj_texlist.h>
#include <sg_xpt.h>

/* a codebook entry is eight bytes, a 2-by-2 block of 16-bit texels */
Uint32 _nj_SVQCodeBookSize[7] = {0, 0, 0, 128, 128, 256, 1024};
Uint32 _nj_SVQCodeBookSizeMM[7] = {0, 0, 0, 128, 128, 512, 2048};

/* an index byte picks each 2-by-2 block. a level of side s takes s * s / 4
 * bytes, and the 1-by-1 level one. a mipmapped texture's indices add up its
 * levels down to 1-by-1 */
Uint32 _nj_SVQIndexSize[7] = {1, 1, 4, 16, 64, 256, 1024};
Uint32 _nj_SVQIndexSizeMM[7] = {1, 2, 6, 22, 86, 342, 1366};

/* a texture of side s holds (4 * s * s - 1) / 3 texels with its mipmaps.
 * each texel takes two bytes, and the size counts two more */
#define MIP_TEXELS(side) (((4UL * (side) * (side)) - 1UL) / 3UL)
#define IMAGE_BYTES(side) (2UL + (2UL * MIP_TEXELS(side)))
Uint32 _nj_ImageByteSize[11] = {
    IMAGE_BYTES(1),   IMAGE_BYTES(2),   IMAGE_BYTES(4),   IMAGE_BYTES(8),
    IMAGE_BYTES(16),  IMAGE_BYTES(32),  IMAGE_BYTES(64),  IMAGE_BYTES(128),
    IMAGE_BYTES(256), IMAGE_BYTES(512), IMAGE_BYTES(1024)};
