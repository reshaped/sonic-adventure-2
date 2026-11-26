/* CRI ADX's expander (ADXPD). each handle decodes the run of ADX blocks its
 * entry gives it, in one ADXPD_ExecHndl call */
#include <adx_dcd.h>
#include <adx_dcd3.h>
#include <adx_xpnd.h>
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <string.h>

/* the handles the expander holds */
#define ADXPD_OBJ_MAX 8
/* a new handle's mode */
#define DEF_MODE 0
/* the channels of a mono entry and of a stereo one */
#define NCH_MONO 1
#define NCH_STEREO 2
/* the samples a channel's dly holds, as adx_dcd3.h's decoders keep them */
#define DLY_SMPL 2

/* a handle. its entry sets nch, ibuf, reqnblk and the output buffers. a
 * mono entry leaves obufr null */
typedef struct _adxpd_obj {
    Bool used;
    Sint32 no; /* the handle's index in adxpd_obj */
    Sint32 mode; /* ADXPD_SetMode's last mode, or DEF_MODE */
    Sint32 stat; /* an ADXPD_STAT_ value */
    Sint32 nblk; /* the blocks the last decode gave */
    Sint32 nch;
    Sint8 *ibuf;
    Sint32 reqnblk; /* the blocks the entry gives */
    Sint16 *obufl;
    Sint16 *obufr;
    /* each channel's dly. dly[0] is for mono or the left channel, and dly[1]
     * for the right */
    Sint16 dly[NCH_STEREO][DLY_SMPL];
    /* the prediction coefficients, k0 then k1 as ADX_GetCoefficient gives
     * them */
    Sint16 coef[2];
} ADXPD_OBJ;

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void ADXPD_SetMode(ADXPD xpd, Sint32 mode);
void adxpd_error(void);
void ADXPD_ExecServer(void);

/* every handle, in use or free */
ADXPD_OBJ adxpd_obj[ADXPD_OBJ_MAX];
/* NOLINTEND(misc-use-internal-linkage) */

Bool adxpd_internal_error = FALSE;

void ADXPD_Init(void)
{
    memset(adxpd_obj, 0, sizeof(adxpd_obj));
}

ADXPD ADXPD_Create(void)
{
    Sint32 i;
    ADXPD xpd;

    for (i = 0; i < ADXPD_OBJ_MAX; i++) {
        if (adxpd_obj[i].used == FALSE) {
            break;
        }
    }
    if (i == ADXPD_OBJ_MAX) {
        return NULL;
    }
    xpd = &adxpd_obj[i];
    memset(xpd, 0, sizeof(ADXPD_OBJ));
    xpd->used = TRUE;
    xpd->no = i;
    xpd->mode = DEF_MODE;
    xpd->stat = ADXPD_STAT_STOP;
    ADX_GetCoefficient(ADXPD_DEF_COF, ADXPD_DEF_SFREQ, &xpd->coef[0],
                       &xpd->coef[1]);
    memset(xpd->dly, 0, sizeof(xpd->dly));
    return xpd;
}

void ADXPD_SetCoef(ADXPD xpd, Sint32 sfreq, Sint32 cof)
{
    ADX_GetCoefficient(cof, sfreq, &xpd->coef[0], &xpd->coef[1]);
}

void ADXPD_Destroy(ADXPD xpd)
{
    if (xpd != NULL) {
        xpd->used = FALSE;
        memset(xpd, 0, sizeof(ADXPD_OBJ));
    }
}

void ADXPD_SetMode(ADXPD xpd, Sint32 mode)
{
    xpd->mode = mode;
}

Sint32 ADXPD_GetStat(ADXPD xpd)
{
    return xpd->stat;
}

Sint32 ADXPD_EntryMono(ADXPD xpd, Sint8 *ibuf, Sint32 nblk, Sint16 *obuf)
{
    if (xpd->stat == ADXPD_STAT_STOP) {
        xpd->nch = NCH_MONO;
        xpd->ibuf = ibuf;
        xpd->reqnblk = nblk;
        xpd->obufl = obuf;
        xpd->obufr = NULL;
        return 1;
    }
    return 0;
}

Sint32 ADXPD_EntrySte(ADXPD xpd, Sint8 *ibuf, Sint32 nblk, Sint16 *obufl,
                      Sint16 *obufr)
{
    if (xpd->stat == ADXPD_STAT_STOP) {
        xpd->nch = NCH_STEREO;
        xpd->ibuf = ibuf;
        xpd->reqnblk = nblk;
        xpd->obufl = obufl;
        xpd->obufr = obufr;
        return 1;
    }
    return 0;
}

void ADXPD_Start(ADXPD xpd)
{
    if (xpd->stat == ADXPD_STAT_STOP) {
        xpd->nblk = 0;
        xpd->stat = ADXPD_STAT_START;
    }
}

void ADXPD_Stop(ADXPD xpd)
{
    xpd->stat = ADXPD_STAT_STOP;
    memset(xpd->dly, 0, sizeof(xpd->dly));
}

void ADXPD_Reset(ADXPD xpd)
{
    if (xpd->stat == ADXPD_STAT_END) {
        xpd->stat = ADXPD_STAT_STOP;
    }
}

Sint32 ADXPD_GetNumBlk(ADXPD xpd)
{
    return xpd->nblk;
}

void adxpd_error(void)
{
    adxpd_internal_error = TRUE;
}

void ADXPD_ExecHndl(ADXPD xpd)
{
    if (xpd->stat == ADXPD_STAT_START) {
        xpd->stat = ADXPD_STAT_EXEC;
    }
    if (xpd->stat == ADXPD_STAT_EXEC) {
        if (xpd->nch == NCH_MONO) {
            xpd->nblk =
                ADX_DecodeMonoFloat(xpd->ibuf, xpd->reqnblk, xpd->obufl,
                                    xpd->dly[0], xpd->coef[0], xpd->coef[1]);
        } else {
            xpd->nblk = ADX_DecodeSteFloat(xpd->ibuf, xpd->reqnblk, xpd->obufl,
                                           xpd->dly[0], xpd->obufr, xpd->dly[1],
                                           xpd->coef[0], xpd->coef[1]);
            if (xpd->nblk % NCH_STEREO == 1) {
                adxpd_error();
            }
        }
        xpd->stat = ADXPD_STAT_END;
    }
}

void ADXPD_ExecServer(void)
{
    Sint32 i;
    ADXPD xpd;

    for (i = 0; i < ADXPD_OBJ_MAX; i++) {
        xpd = &adxpd_obj[i];
        if (xpd->used == TRUE) {
            ADXPD_ExecHndl(xpd);
        }
    }
}
