/* MPEG video's inverse DCT: its matrices and parameters, and the drivers
 * that hand each block to a transform */
#include <sg_xpt.h>
#include <sofdec.h>

#include "../sofdec/mpv_section.h"

/* the floats of a transform matrix, 4 by 4, and the matrices, the even
 * one and the odd */
#define MTX_SIZE 16
#define NMTX 2
/* a cache line's bytes, the alignment DCT_FzInit gives the matrices in
 * mtx_area */
#define MTX_ALIGN 32

/* COSk is cos(k pi / 16) / 2, the matrices' entries */
#define COS1 0.49039264F
#define COS2 0.461939766F
#define COS3 0.41573481F
#define COS4 0.35355339F
#define COS5 0.27778512F
#define COS6 0.19134172F
#define COS7 0.097545161F

/* The archive also exports this scan-table initializer. */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void DCT_FzInitScanTbl(const Sint8 *scan, Sint8 *tbl);
/* NOLINTEND(misc-use-internal-linkage) */

static void copy_words(Uint32 *dst, const Uint32 *src, Uint32 nword);

/* a matrix's row of four holds one coefficient's weights. the even matrix's
 * rows are coefficients 0, 4, 2 and 6. the odd matrix's are 1, 3, 5 and 7.
 * a row's entry r is the weight at point 2r of the 8-point line. point
 * 7 - 2r takes the same weight, negated for an odd coefficient */
/* clang-format off */
static const Float mtx_even_init[MTX_SIZE] = {
    COS4,  COS4,  COS4,  COS4,
    COS4, -COS4,  COS4, -COS4,
    COS2, -COS6, -COS2,  COS6,
    COS6,  COS2, -COS6, -COS2,
};
static const Float mtx_odd_init[MTX_SIZE] = {
    COS1,  COS5, -COS7, -COS3,
    COS3, -COS1,  COS5,  COS7,
    COS5,  COS7, -COS3,  COS1,
    COS7,  COS3,  COS1,  COS5,
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

/* the version string, as DCT_FzInit last read it */
static const char *version;
/* the matrices' room where DCT_FzInit's work is null, with MTX_ALIGN bytes
 * to spare for their alignment */
static Uint8 mtx_area[(NMTX * MTX_SIZE * sizeof(Float)) + MTX_ALIGN];
/* NOLINTBEGIN(misc-use-internal-linkage) */
Float *dctfz_p_mtx_even;
Float *dctfz_p_mtx_odd;
/* NOLINTEND(misc-use-internal-linkage) */

const char *DCT_FzGetVerStr(void)
{
    return dct_version;
}

/* copy the matrices into work, or into mtx_area where work is null.
 * dctfz_p_mtx_even and dctfz_p_mtx_odd then point at them. work must hold
 * NMTX * MTX_SIZE floats, on the 8-byte boundary DCTAFZ_DenseCbp needs */
void DCT_FzInit(void *work)
{
    /* @hack spelling: mtx, a local the aligned room is stepped in, where the
     * sum stored would do */
    Float *mtx;

    version = DCT_FzGetVerStr();
    if (work != NULL) {
        dctfz_p_mtx_even = work;
        dctfz_p_mtx_odd = (Float *)work + MTX_SIZE;
    } else {
        mtx = (Float *)(((Uint32)mtx_area + MTX_ALIGN - 1) & ~(MTX_ALIGN - 1));
        dctfz_p_mtx_even = mtx;
        mtx += MTX_SIZE;
        dctfz_p_mtx_odd = mtx;
    }
    /* @hack type: the matrices copied as Uint32 words, where Floats would
     * do */
    copy_words((Uint32 *)dctfz_p_mtx_even, (const Uint32 *)mtx_even_init,
               MTX_SIZE);
    copy_words((Uint32 *)dctfz_p_mtx_odd, (const Uint32 *)mtx_odd_init,
               MTX_SIZE);
}

/* tbl[i] gets the place, in the DCT's order, of the coefficient whose raster
 * place is scan[i] */
void DCT_FzInitScanTbl(const Sint8 *scan, Sint8 *tbl)
{
    Sint32 i;

    for (i = 0; i < MPV_COEFS; i++) {
        tbl[i] = dct_order[scan[i]];
    }
}

void DCT_FzInitPa(struct dctfz_pa *pa)
{
    /* @hack spelling: the eight stores to last written out, where a loop
     * over last would do */
    pa->last[0] = 0;
    pa->last[1] = 0;
    pa->last[2] = 0;
    pa->last[3] = 0;
    pa->last[4] = 0;
    pa->last[5] = 0;
    pa->last[6] = 0;
    pa->last[7] = 0;
    pa->sparse = DCTAFZ_SparseDc;
    pa->dense = DCTAFZ_DenseCbp;
    pa->first_bit = DCTFZ_CBP_FIRST;
    pa->first_keep = ~DCTFZ_CBP_FIRST;
    pa->count[0] = 0;
    pa->count[1] = 0;
    pa->round = DCTFZ_BIAS + 0.5F;
    pa->coef_stride = MPV_COEFS * sizeof(Float);
    pa->cbp = 0;
    pa->coef = NULL;
    pa->blk = NULL;
    pa->next_blk = NULL;
    pa->bias = DCTFZ_BIAS;
    pa->unk_3c = MPV_COEFS * sizeof(Sint16);
    pa->mtx_even = dctfz_p_mtx_even;
    pa->mtx_odd = dctfz_p_mtx_odd;
    pa->work = NULL;
    pa->nleft = 0;
    pa->work_at = NULL;
}

static void copy_words(Uint32 *dst, const Uint32 *src, Uint32 nword)
{
    /* @hack spelling: nword raised by one and tested after each decrement,
     * where a test before it would do */
    nword++;
    while (--nword != 0) {
        *dst++ = *src++;
    }
}

#pragma section SFD00
MPV_CODE(PSGSFD00)
void DCT_FzTrans6Blk(struct dctfz_pa *pa)
{
    Float *coef = pa->coef;
    Uint32 bit = pa->first_bit;
    Sint16 **blk = pa->blk;
    Uint32 cbp = 0;
    Uint8 *last = pa->last;
    Sint32 i;

    for (i = 0; i < MPV_BLOCKS; i++) {
        if (*last == 0) {
            pa->sparse(coef, *blk);
        } else {
            cbp |= bit;
        }
        coef = (Float *)((Uint8 *)coef + pa->coef_stride);
        blk++;
        bit >>= 1;
        last++;
    }
    if (cbp != 0) {
        pa->cbp = cbp;
        pa->next_blk = pa->blk;
        pa->dense(pa);
    }
}

MPV_CODE(PSGSFD00)
void DCT_FzTransCbp(struct dctfz_pa *pa)
{
    Sint16 **blk = pa->blk;
    Float *coef = pa->coef;
    Sint32 cbp = pa->cbp;
    Uint32 keep = pa->first_keep;
    Uint8 *last = pa->last;
    Sint32 i;

    for (i = 0; i < MPV_BLOCKS; i++) {
        if (cbp < 0) {
            if (*last == 0) {
                pa->sparse(coef, *blk);
                pa->cbp &= keep;
                /* @hack redundant: an empty else */
            } else {
            }
        }
        cbp <<= 1;
        coef = (Float *)((Uint8 *)coef + pa->coef_stride);
        blk++;
        last++;
        keep = (keep >> 1) | pa->first_bit;
    }
    if (pa->cbp != 0) {
        pa->next_blk = pa->blk;
        pa->dense(pa);
    }
}
#pragma section
