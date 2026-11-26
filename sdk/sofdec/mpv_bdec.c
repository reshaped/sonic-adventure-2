/* the block decoder's zigzag scan and default intra matrix, in the DCT's
 * order, and its masks */
#include <sg_xpt.h>
#include <sofdec.h>

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mpvbdec_SetupIxa(struct mpv_ixa *ixa);

/* mpvbdec_zigzag[n] is the place, in the DCT's order, of the zigzag scan's
 * coefficient n */
Sint8 mpvbdec_zigzag[MPV_COEFS];
/* MPEG-1's default intra quantizer matrix, in the DCT's order */
Uint8 mpvbdec_dfl_iqm[MPV_COEFS];
/* NOLINTEND(misc-use-internal-linkage) */

/* low_mask[n] keeps a halfword's low 16 - n bits */
static const Uint16 low_mask[16] = {
    0xffff, 0x7fff, 0x3fff, 0x1fff, 0x0fff, 0x07ff, 0x03ff, 0x01ff,
    0x00ff, 0x007f, 0x003f, 0x001f, 0x000f, 0x0007, 0x0003, 0x0001,
};
/* zigzag_scan[n] is the raster place, 8 times the row plus the column, of the
 * zigzag scan's coefficient n */
/* clang-format off */
static const Sint8 zigzag_scan[MPV_COEFS] = {
     0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
};
/* clang-format on */
/* dct_order[n] is the place, in the DCT's order, of the coefficient at
 * raster place n. a row here is a row of the block */
/* clang-format off */
static const Sint8 dct_order[MPV_COEFS] = {
     0, 32,  2, 33,  1, 34,  3, 35,
    16, 48, 18, 49, 17, 50, 19, 51,
     8, 40, 10, 41,  9, 42, 11, 43,
    20, 52, 22, 53, 21, 54, 23, 55,
     4, 36,  6, 37,  5, 38,  7, 39,
    24, 56, 26, 57, 25, 58, 27, 59,
    12, 44, 14, 45, 13, 46, 15, 47,
    28, 60, 30, 61, 29, 62, 31, 63,
};
/* clang-format on */
/* MPEG-1's default intra quantizer matrix, in raster order. a row here is a
 * row of the matrix */
/* clang-format off */
static const Uint8 default_intra_matrix[MPV_COEFS] = {
     8, 16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83,
};
/* clang-format on */

void MPVBDEC_Init(struct mpv_ixa *ixa)
{
    Sint32 i;

    for (i = 0; i < MPV_COEFS; i++) {
        mpvbdec_zigzag[i] = dct_order[zigzag_scan[i]];
        mpvbdec_dfl_iqm[dct_order[i]] = default_intra_matrix[i];
    }
    mpvbdec_SetupIxa(ixa);
    /* @hack redundant: a return that ends the function anyway */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* the copies take each table as words, so the four must lie on 4-byte
 * boundaries. their places in the data and in struct mpv_ixa put them there,
 * not their types */
void mpvbdec_SetupIxa(struct mpv_ixa *ixa)
{
    /* @hack spelling: to, a local for each copy's destination, where the
     * destination itself would do */
    Uint32 *to;

    to = (Uint32 *)ixa->zigzag;
    if (to != NULL) {
        UTY_MemcpyDword(to, (const Uint32 *)mpvbdec_zigzag,
                        UTY_DWORDS(mpvbdec_zigzag));
    }
    to = (Uint32 *)ixa->mask;
    if (to != NULL) {
        UTY_MemcpyDword(to, (const Uint32 *)low_mask, UTY_DWORDS(low_mask));
    }
}
