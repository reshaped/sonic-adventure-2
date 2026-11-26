/* a macroblock's blocks, decoded into coefficients and transformed into
 * samples */
#include <sg_xpt.h>
#include <sofdec.h>

#include "../sofdec/mpv_section.h"

/* mpv->cbp shifted left by DCT_CBP_SHIFT is the DCT's pattern */
#define DCT_CBP_SHIFT 2

#pragma section SFD00
MPV_CODE(PSGSFD00)
Sint32 MPVCDEC_IntraBlocks(struct mpv_obj *mpv)
{
    struct mpvbdec_pa *bpa;
    struct dctfz_pa *dct;
    Uint8 *last;

    MPVABDEC_EraseBlock6(mpv->coef[0]);
    dct = &mpv->dct;
    bpa = &mpv->bpa;
    last = dct->last;
    bpa->qscale = mpv->qscale;
    bpa->qm = mpv->intra_qm;
    bpa->level_add = 0;
    bpa->dcsiz = mpvvlc_y_dcsiz;
    bpa->dc_pred = &mpv->dc_pred.y;
    bpa->coef = mpv->coef[0];
    last[0] = MPVABDEC_IntraBlock(mpv, bpa);
    bpa->coef = mpv->coef[1];
    last[1] = MPVABDEC_IntraBlock(mpv, bpa);
    bpa->coef = mpv->coef[2];
    last[2] = MPVABDEC_IntraBlock(mpv, bpa);
    bpa->coef = mpv->coef[3];
    last[3] = MPVABDEC_IntraBlock(mpv, bpa);
    bpa->dcsiz = mpvvlc_c_dcsiz;
    bpa->dc_pred = &mpv->dc_pred.cb;
    bpa->coef = mpv->coef[4];
    last[4] = MPVABDEC_IntraBlock(mpv, bpa);
    bpa->dc_pred = &mpv->dc_pred.cr;
    bpa->coef = mpv->coef[5];
    last[5] = MPVABDEC_IntraBlock(mpv, bpa);
    DCT_FzTrans6Blk(dct);
    return 0;
}

MPV_CODE(PSGSFD00)
Sint32 MPVCDEC_NintraBlocks(struct mpv_obj *mpv)
{
    struct mpvbdec_pa *bpa;
    struct dctfz_pa *dct;
    Uint8 *last;
    Sint32 cbp;
    Sint32 i;

    dct = &mpv->dct;
    last = dct->last;
    bpa = &mpv->bpa;
    bpa->qscale = mpv->qscale;
    bpa->qm = mpv->nintra_qm;
    bpa->level_add = 1;
    cbp = mpv->cbp << DCT_CBP_SHIFT;
    dct->cbp = cbp;
    for (i = 0; i < MPV_BLOCKS; i++) {
        if (cbp < 0) {
            bpa->coef = mpv->coef[i];
            last[i] = MPVABDEC_NintraBlock(mpv, bpa);
        }
        cbp <<= 1;
    }
    DCT_FzTransCbp(dct);
    return 0;
}
#pragma section
