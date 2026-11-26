/* CRI ADX's streams (ADXT), and the library's server pass. adx_tlk2.c
 * holds the starts that open their own input */
#include <adx_amp.h>
#include <adx_crs.h>
#include <adxahx.h>
#include <adx_dcd.h>
#include <adx_errs.h>
#include <adx_inis.h>
#include <adx_rnas.h>
#include <adx_sjd.h>
#include <adx_stmc.h>
#include <adx_tlk.h>
#include <cri_adxf.h>
#include <cri_adxt.h>
#include <cri_lsc.h>
#include <mw_rna.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>
#include <string.h>
#ifdef NONMATCHING
/* this side's division sets errno to EDIV for a zero divisor, as SHC's runtime
 * does */
#include <errno.h>
#endif

#include "../sofdec/adx_bendian.h"

/* the AHX joints ADXT_Create makes: one, for the first channel */
#define AHX_NCH 1
/* stm's read limit from a file's start, in sectors. the server sets another
 * once it has read the data's header */
#define PREP_RDSCT 25
/* ADX data's block: BLK_SIZE bytes a channel for BLK_SMPL samples */
#define BLK_SIZE 18
#define BLK_SMPL 32
#define PCM_BPS 16

/* ADXT_GetTime's *tscale in NTSC's and PAL's time modes, its counts in a
 * second. a V-blank counts VSYNC_UNITS, and a second holds 59.94 V-blanks
 * in NTSC and 50 in PAL */
#define TUNIT_NTSC 5994
#define TUNIT_PAL 5000
#define VSYNC_UNITS 100
/* the drift, in milliseconds, past which ADXT_GetTime sets its count of
 * V-blanks from the sound output's time again */
#define DRIFT_MAX 60.0F
/* the milliseconds in a second */
#define MSEC 1000.0F
/* ADXT_GetTimeReal's units in a second */
#define REAL_UNITS 100.0F
/* the empty passes of the loop ADXT_Pause spins after it turns the sound
 * output's play off */
#define PAUSE_WAIT 500000

/* the lesct and trpnsmpl adxt_start_sj sets: a loop's end that never comes,
 * and no trap */
#define NO_LIMIT 0x7fffffff
#define NO_TRAP (-1)
/* the *tscale ADXT_GetTimeSfreq gives, with *ncount 0, for a stream in no
 * state of ADXT_STAT_PLAYING, ADXT_STAT_DECEND and ADXT_STAT_PLAYEND */
#define IDLE_TSCALE 1
/* the count ADXT_GetTime adds to a finished stream's time, past the
 * truncated time of its samples */
#define END_COUNT 1
/* ADXT_SetPauseBuf's result. the call sets no buffer */
#define PAUSE_BUF_RESULT 0x4000

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void ADXT_StopWithoutLsc(ADXT adxt);
void ADXT_GetTimeSfreq(ADXT adxt, Sint32 *ncount, Sint32 *tscale);
Sint32 ADXT_SetPauseBuf(void);

/* ADXT_SetTimeMode's mode */
Sint32 adxt_time_mode = ADXT_TMODE_SFREQ;
/* the time ADXT_ExecServer's last pass took, in syTmrGetCount's counts.
 * adxt_svrcnt holds the whole pass's. adxt_svrcnt_hndl holds the time of the
 * streams' handlers and LSC's server. each other adxt_svrcnt_ holds the time
 * of the server its suffix names: ADXSJD's, ADXRNA's, ADXF's or ADXSTM's */
Sint32 adxt_svrcnt = 0;
Sint32 adxt_svrcnt_sjd = 0;
Sint32 adxt_svrcnt_rna = 0;
Sint32 adxt_svrcnt_adxf = 0;
Sint32 adxt_svrcnt_adxstm = 0;
Sint32 adxt_svrcnt_hndl = 0;
/* *tscale in a mode of V-blanks: TUNIT_NTSC or TUNIT_PAL, or 0 before
 * ADXT_SetTimeMode sets it */
Sint32 adxt_time_unit;
/* the drift that ADXT_GetTime last found in a mode of V-blanks, in
 * milliseconds. it is the sound output's time less the time counted in
 * V-blanks. a call on a stream in neither ADXT_STAT_PLAYING nor
 * ADXT_STAT_DECEND leaves it 0 */
Float adxt_diff_av;
/* NOLINTEND(misc-use-internal-linkage) */

ADXT ADXT_Create(Sint32 maxnch, void *work, Sint32 worksize)
{
    ADXT adxt;
    char *buf;
    Sint32 size;
    Sint32 i;

    buf =
        (char *)(((Uint32)work + ADXT_WORK_ALIGN - 1) & ~(ADXT_WORK_ALIGN - 1));
    size = worksize - (buf - (char *)work);
    for (i = 0; i < ADXT_MAX_OBJ; i++) {
        if (adxt_obj[i].used == FALSE) {
            break;
        }
    }
    if (i == ADXT_MAX_OBJ) {
        return NULL;
    }
    adxt = &adxt_obj[i];
    memset(adxt, 0, sizeof(ADX_TALK));
    adxt->maxnch = maxnch;
    adxt->ibuf = buf + (ADXT_OBUF_WORK * maxnch * sizeof(Sint16));
    adxt->ibuflen =
        (size - (ADXT_OBUF_WORK * maxnch * sizeof(Sint16)) - ADXT_IBUF_XLEN) /
        ADXSTM_SCT_SIZE * ADXSTM_SCT_SIZE;
    adxt->ibufxlen = ADXT_IBUF_XLEN;
    adxt->obuf = (Sint16 *)buf;
    adxt->obufsize = ADXT_OBUF_SIZE;
    adxt->obufdist = ADXT_OBUF_DIST;
    /* @hack redundant: the store to sji, which the memset has cleared */
    adxt->sji = NULL;
    adxt->sjf = SJRBF_Create(adxt->ibuf, adxt->ibuflen, adxt->ibufxlen);
    if (adxt->sjf == NULL) {
        ADXT_Destroy(adxt);
        return NULL;
    }
    for (i = 0; i < maxnch; i++) {
        adxt->sjo[i] =
            SJRBF_Create((char *)(adxt->obuf + (adxt->obufdist * i)),
                         adxt->obufsize * sizeof(Sint16),
                         (adxt->obufdist - adxt->obufsize) * sizeof(Sint16));
        if (adxt->sjo[i] == NULL) {
            ADXT_Destroy(adxt);
            return NULL;
        }
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((adxt->sjd = ADXSJD_Create(adxt->sjf, maxnch, adxt->sjo)) == NULL) {
        ADXT_Destroy(adxt);
        return NULL;
    }
    /* @hack spelling: the loop over AHX_NCH channels, where one create would
     * do */
    for (i = 0; i < AHX_NCH; i++) {
        adxt->ahxsjo[i] =
            SJRBF_Create((char *)(adxt->obuf + (adxt->obufdist * i)),
                         adxt->obufsize * sizeof(Sint16),
                         (ADXT_OBUF_WORK - ADXT_OBUF_SIZE) * sizeof(Sint16));
        if (adxt->ahxsjo[i] == NULL) {
            ADXT_Destroy(adxt);
            return NULL;
        }
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((adxt->ahx = adxahx_create(adxt->sjf, maxnch, adxt->ahxsjo)) == NULL) {
        /* @hack redundant: the store to ahxflg, which the memset has
         * cleared */
        adxt->ahxflg = FALSE;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((adxt->rna = ADXRNA_Create(adxt->sjo, maxnch)) == NULL) {
        ADXT_Destroy(adxt);
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((adxt->lsc = LSC_Create(adxt->sjf)) == NULL) {
        ADXT_Destroy(adxt);
        return NULL;
    }
    adxt->svrfreq = ADXT_DEF_SVRFREQ;
    adxt->maxsct = adxt->ibuflen / ADXSTM_SCT_SIZE;
    adxt->minsct = adxt->maxsct * ADXT_RELOAD_SHARE;
    /* @hack redundant: the store to outvol, which the memset has cleared */
    adxt->outvol = 0;
    for (i = 0; i < maxnch; i++) {
        adxt->outpan[i] = ADXT_PAN_AUTO;
    }
    adxt->lpflg = TRUE;
    /* @hack redundant: the stores of 0 to trp, wpos, mofst, ercode, edecpos,
     * edeccnt and eshrtcnt, which the memset has made */
    adxt->trp = 0;
    adxt->wpos = 0;
    adxt->mofst = 0;
    adxt->ercode = 0;
    adxt->edecpos = 0;
    adxt->edeccnt = 0;
    adxt->eshrtcnt = 0;
    adxt->autorcvr = ADXT_RMODE_STOP;
    /* @hack redundant: the store to pause_flag, which the memset has
     * cleared */
    adxt->pause_flag = FALSE;
    /* @hack redundant: the store to time_ofst, which the memset has
     * cleared */
    adxt->time_ofst = 0;
    adxt->lnkflg = TRUE;
    adxt->used = TRUE;
    return adxt;
}

void ADXT_Destroy(ADXT adxt)
{
    Sint32 i;

    if (adxt == NULL) {
        return;
    }
    if (adxt->used == TRUE) {
        ADXT_Stop(adxt);
    }
    if (adxt->rna != NULL) {
        ADXRNA_Destroy(adxt->rna);
    }
    if (adxt->sjd != NULL) {
        ADXSJD_Destroy(adxt->sjd);
    }
    if (adxt->ahx != NULL) {
        adxahx_destroy(adxt->ahx);
    }
    if (adxt->stm != NULL) {
        ADXSTM_Close(adxt->stm);
    }
    ADXCRS_Lock();
    if (adxt->lsc != NULL) {
        LSC_Destroy(adxt->lsc);
    }
    if (adxt->sjf != NULL) {
        SJ_Destroy(adxt->sjf);
    }
    for (i = 0; i < adxt->maxnch; i++) {
        if (adxt->sjo[i] != NULL) {
            SJ_Destroy(adxt->sjo[i]);
        }
        if (adxt->ampsji[i] != NULL) {
            SJ_Destroy(adxt->ampsji[i]);
        }
        if (adxt->ampsjo[i] != NULL) {
            SJ_Destroy(adxt->ampsjo[i]);
        }
        if (adxt->ahxsjo[i] != NULL) {
            SJ_Destroy(adxt->ahxsjo[i]);
        }
    }
    if (adxt->amp != NULL) {
        ADXAMP_Destroy(adxt->amp);
    }
    memset(adxt, 0, sizeof(ADX_TALK));
    /* @hack redundant: the store to used, which the memset has just
     * cleared */
    adxt->used = FALSE;
    ADXCRS_Unlock();
}

void ADXT_DestroyAll(void)
{
    ADXT adxt;
    Sint32 i;

    for (i = 0; i < ADXT_MAX_OBJ; i++) {
        adxt = &adxt_obj[i];
        if (adxt->used == TRUE) {
            ADXT_Destroy(adxt);
        }
    }
}

void ADXT_CloseAllHandles(void)
{
    ADXT_DestroyAll();
}

void adxt_start_sj(ADXT adxt, SJ sj)
{
    Sint32 i;

    for (i = 0; i < adxt->maxnch; i++) {
        SJ_Reset(adxt->sjo[i]);
    }
    ADXSJD_SetInSj(adxt->sjd, sj);
    adxt->sji = sj;
    adxt->stat = ADXT_STAT_DECINFO;
    adxt->lpcnt = 0;
    adxt->pstready_flag = FALSE;
    adxt->lesct = NO_LIMIT;
    adxt->trpnsmpl = NO_TRAP;
    adxt->tvofst = 0;
    adxt->svcnt = adxt_vsync_cnt;
    adxt->ahxflg = TRUE;
    if (adxt->ahx != NULL) {
        /* @hack spelling: the loop over AHX_NCH channels, where one reset
         * would do */
        for (i = 0; i < AHX_NCH; i++) {
            SJ_Reset(adxt->ahxsjo[i]);
        }
        adxahx_set_input(adxt->ahx, sj);
    }
}

void adxt_start_stm(ADXT adxt)
{
    ADXSTM_SetBufSize(adxt->stm, adxt->minsct * ADXSTM_SCT_SIZE,
                      adxt->maxsct * ADXSTM_SCT_SIZE);
    ADXSTM_SetEos(adxt->stm, PREP_RDSCT);
    ADXSTM_Start(adxt->stm);
    adxt_start_sj(adxt, adxt->sjf);
    adxt->pmode = ADXT_PMODE_FNAME;
}

void ADXT_StartSj(ADXT adxt, SJ sj)
{
    Sint32 i;

    ADXT_Stop(adxt);
    ADXCRS_Lock();
    for (i = 0; i < adxt->maxnch; i++) {
        SJ_Reset(adxt->sjo[i]);
    }
    ADXSJD_SetInSj(adxt->sjd, sj);
    adxt->sji = sj;
    ADXSJD_Start(adxt->sjd);
    adxt->stat = ADXT_STAT_DECINFO;
    adxt->pmode = ADXT_PMODE_SJ;
    adxt->lpcnt = 0;
    adxt->lnkflg = TRUE;
    adxt->tvofst = 0;
    adxt->svcnt = adxt_vsync_cnt;
    ADXCRS_Unlock();
}

/* stop the stream as ADXT_Stop does, but leave its LSC as it is */
void ADXT_StopWithoutLsc(ADXT adxt)
{
    ADXCRS_Lock();
    ADXRNA_SetTransSw(adxt->rna, FALSE);
    ADXRNA_SetPlaySw(adxt->rna, FALSE);
    ADXSJD_Stop(adxt->sjd);
    if (adxt->stm != NULL) {
        ADXSTM_Stop(adxt->stm);
        ADXSTM_Close(adxt->stm);
        if (adxt->sji != NULL) {
            SJ_Reset(adxt->sji);
        }
        adxt->stm = NULL;
    }
    if (adxt->pmode == ADXT_PMODE_MEM && adxt->sji != NULL) {
        SJ_Destroy(adxt->sji);
    }
    if (adxt->ahxflg == TRUE && adxt->ahx != NULL) {
        adxahx_stop(adxt->ahx);
        adxt->maxdecsmpl = 0;
    }
    adxt->sji = NULL;
    adxt->stat = ADXT_STAT_STOP;
    ADXCRS_Unlock();
}

void ADXT_Stop(ADXT adxt)
{
    if (adxt->pmode == ADXT_PMODE_SLFILE) {
        LSC_Stop(adxt->lsc);
        if (adxt->sji != NULL) {
            SJ_Reset(adxt->sji);
        }
    }
    ADXT_StopWithoutLsc(adxt);
}

Sint32 ADXT_GetStat(ADXT adxt)
{
    return adxt->stat;
}

void ADXT_SetTimeMode(Sint32 mode)
{
    if (mode == ADXT_TMODE_NTSC) {
        adxt_time_unit = TUNIT_NTSC;
    } else if (mode == ADXT_TMODE_PAL) {
        adxt_time_unit = TUNIT_PAL;
    }
    adxt_time_mode = mode;
}

/* the time played, as ADXT_GetTime gives it in ADXT_TMODE_SFREQ, whatever
 * the mode */
void ADXT_GetTimeSfreq(ADXT adxt, Sint32 *ncount, Sint32 *tscale)
{
    if (adxt->stat == ADXT_STAT_PLAYING || adxt->stat == ADXT_STAT_DECEND) {
        ADXRNA_GetTime(adxt->rna, ncount, tscale);
    } else if (adxt->stat == ADXT_STAT_PLAYEND) {
        if (adxt->ahxflg == FALSE) {
            *ncount = ADXSJD_GetTotalNumSmpl(adxt->sjd);
            *tscale = ADXSJD_GetSfreq(adxt->sjd);
        } else {
            *ncount = adxahx_total_samples(adxt->ahx);
            *tscale = adxahx_sample_rate(adxt->ahx);
        }
        *ncount *= PCM_BPS / ADXSJD_GetOutBps(adxt->sjd);
    } else {
        *ncount = 0;
        *tscale = IDLE_TSCALE;
    }
    *ncount += adxt->time_ofst;
}

void ADXT_GetTime(ADXT adxt, Sint32 *ncount, Sint32 *tscale)
{
    Sint32 n;
    Sint32 t;

    if (adxt_time_mode == ADXT_TMODE_SFREQ) {
        ADXT_GetTimeSfreq(adxt, ncount, tscale);
        return;
    }
    adxt_diff_av = 0.0F;
    if (adxt->stat == ADXT_STAT_PLAYING || adxt->stat == ADXT_STAT_DECEND) {
        if (adxt->pause_flag == FALSE) {
            *ncount =
                ((adxt_vsync_cnt - adxt->svcnt) * VSYNC_UNITS) + adxt->tvofst;
        } else {
            *ncount = adxt->tvofst;
        }
        ADXT_GetTimeSfreq(adxt, &n, &t);
        adxt_diff_av =
            (((Float)n / (Float)t) - ((Float)*ncount / (Float)adxt_time_unit)) *
            MSEC;
        if (adxt_diff_av > DRIFT_MAX || adxt_diff_av < -DRIFT_MAX) {
            ADXRNA_GetTime(adxt->rna, &n, &t);
            adxt->tvofst = (Float)n / (Float)t * (Float)adxt_time_unit;
            adxt->svcnt = adxt_vsync_cnt;
        }
    } else if (adxt->stat == ADXT_STAT_PLAYEND) {
        if (adxt->ahxflg == FALSE) {
            n = ADXSJD_GetTotalNumSmpl(adxt->sjd);
            t = ADXSJD_GetSfreq(adxt->sjd);
        } else {
            n = adxahx_total_samples(adxt->ahx);
            t = adxahx_sample_rate(adxt->ahx);
        }
        n *= PCM_BPS / ADXSJD_GetOutBps(adxt->sjd);
        *ncount = (Float)n / (Float)t * (Float)adxt_time_unit;
        *ncount += adxt->tvofst + END_COUNT;
    } else {
        *ncount = 0;
    }
    *ncount += adxt->time_ofst;
    *tscale = adxt_time_unit;
}

Sint32 ADXT_GetTimeReal(ADXT adxt)
{
    Sint32 ncount;
    Sint32 tscale;

    ADXT_GetTime(adxt, &ncount, &tscale);
    return (Float)ncount / (Float)tscale * REAL_UNITS;
}

Sint32 ADXT_GetNumSmpl(ADXT adxt)
{
    if (adxt->stat >= ADXT_STAT_PREP) {
        /* @hack spelling: the else after a return, where the second return
         * alone would do */
        if (adxt->ahxflg == TRUE) {
            return adxahx_total_samples(adxt->ahx);
        } else {
            return ADXSJD_GetTotalNumSmpl(adxt->sjd);
        }
    }
    return 0;
}

Sint32 ADXT_GetSfreq(ADXT adxt)
{
    if (adxt->stat >= ADXT_STAT_PREP) {
        /* @hack spelling: the else after a return, where the second return
         * alone would do */
        if (adxt->ahxflg == TRUE) {
            return adxahx_sample_rate(adxt->ahx);
        } else {
            return ADXSJD_GetSfreq(adxt->sjd);
        }
    }
    return 0;
}

Sint32 ADXT_GetNumChan(ADXT adxt)
{
    if (adxt->stat >= ADXT_STAT_PREP) {
        /* @hack spelling: the else after a return, where the second return
         * alone would do */
        if (adxt->ahxflg == TRUE) {
            return adxahx_channels(adxt->ahx);
        } else {
            return ADXSJD_GetNumChan(adxt->sjd);
        }
    }
    return 0;
}

Sint32 ADXT_GetHdrLen(ADXT adxt)
{
    if (adxt->stat >= ADXT_STAT_PREP) {
        return ADXSJD_GetHdrLen(adxt->sjd);
    }
    return 0;
}

Sint32 ADXT_GetFmtBps(ADXT adxt)
{
    if (adxt->stat >= ADXT_STAT_PREP) {
        /* @hack spelling: the else after a return, where the second return
         * alone would do */
        if (adxt->ahxflg == TRUE) {
            return adxahx_sample_bits(adxt->ahx);
        } else {
            return ADXSJD_GetFmtBps(adxt->sjd);
        }
    }
    return 0;
}

void ADXT_SetOutPan(ADXT adxt, Sint32 ch, Sint32 pan)
{
    /* @bug memory: stores pan before it tests ch, so a ch of ADXT_MAX_NCH or
     * more writes past outpan (MOV.W at 0x8c130510, the test at
     * 0x8c130514) */
    adxt->outpan[ch] = pan;
    if (ch < adxt->maxnch) {
        ADXRNA_SetOutPan(adxt->rna, ch, pan);
    } else {
        ADXERR_CallErrFunc1("E8101208 ADXT_SetOutPan: parameter error");
    }
}

Sint32 ADXT_GetOutPan(ADXT adxt, Sint32 ch)
{
    return adxt->outpan[ch];
}

void ADXT_SetOutVol(ADXT adxt, Sint32 vol)
{
    adxt->outvol = vol;
    ADXRNA_SetOutVol(adxt->rna, vol);
}

Sint32 ADXT_GetOutVol(ADXT adxt)
{
    return adxt->outvol;
}

void ADXT_SetSvrFreq(ADXT adxt, Sint32 freq)
{
    adxt->svrfreq = freq;
}

void ADXT_SetReloadTime(ADXT adxt, Float time, Sint32 nch, Sint32 sfreq)
{
    Sint32 nbyte;
    Sint32 nsct;
    Sint32 maxsct;

    nbyte = (Sint32)(sfreq * time) / BLK_SMPL * nch * BLK_SIZE;
    nsct = nbyte / ADXSTM_SCT_SIZE;
    /* @hack spelling: maxsct, a local for the stream's maxsct, where the
     * field read in the test and the store would do */
    maxsct = adxt->maxsct;
    if (nsct >= maxsct) {
        nsct = maxsct;
    }
    adxt->minsct = nsct;
}

void ADXT_SetReloadSct(ADXT adxt, Sint32 minsct)
{
    adxt->minsct = minsct;
}

Sint32 ADXT_GetNumSctIbuf(ADXT adxt)
{
    if (adxt->sji != NULL) {
        return SJ_GetNumData(adxt->sji, SJ_LIN_DATA) / ADXSTM_SCT_SIZE;
    }
    return 0;
}

Float ADXT_GetIbufRemainTime(ADXT adxt)
{
    Float time;
    Sint32 nbyte;
    Sint32 nch;
    Float sfreq;

    /* @hack spelling: time, a local for the result, where returns would do */
    time = 0.0F;
    if (ADXT_GetStat(adxt) >= ADXT_STAT_PREP && adxt->sji != NULL) {
        nbyte = SJ_GetNumData(adxt->sji, SJ_LIN_DATA);
        nch = ADXT_GetNumChan(adxt);
        sfreq = ADXT_GetSfreq(adxt);
#ifdef NONMATCHING
        /* this side keeps the shipped result for a stream of 0 channels. SHC's
         * runtime gives 0 for an integer division by 0, and sets errno to
         * EDIV. C leaves the division undefined */
        if (nch == 0) {
            errno = EDIV;
            time = 0.0F / sfreq;
        } else {
            time = (Float)(nbyte / (nch * BLK_SIZE) * BLK_SMPL) / sfreq;
        }
#else
        /* the integer division counts whole blocks */
        /* NOLINTNEXTLINE(bugprone-integer-division) */
        time = (Float)(nbyte / (nch * BLK_SIZE) * BLK_SMPL) / sfreq;
#endif
    }
    return time;
}

Sint32 ADXT_IsIbufSafety(ADXT adxt)
{
    Sint32 nbyte;

    if (adxt->sji != NULL) {
        /* @hack spelling: nbyte, a local for the bytes sji holds, where the
         * call in the test would do */
        nbyte = SJ_GetNumData(adxt->sji, SJ_LIN_DATA);
        if (nbyte >= adxt->minsct * ADXSTM_SCT_SIZE) {
            return TRUE;
        }
    }
    return FALSE;
}

void ADXT_SetAutoRcvr(ADXT adxt, Sint32 rmode)
{
    adxt->autorcvr = rmode;
}

Sint32 ADXT_IsCompleted(ADXT adxt)
{
    /* @hack spelling: the test and two returns, where the comparison
     * returned would do */
    if (adxt->stat == ADXT_STAT_PLAYEND) {
        return TRUE;
    }
    return FALSE;
}

void ADXT_ExecServer(void)
{
    /* @hack redundant: register on t0, where a plain local would do */
    register Uint32 t0;
    /* @hack redundant: register on t1, where a plain local would do */
    register Uint32 t1;
    Uint32 t2;
    Uint32 t3;
    /* @hack redundant: register on t4, where a plain local would do */
    register Uint32 t4;
    Uint32 t5;
    Uint32 now;
    ADXT adxt;
    Sint32 i;

    /* @hack spelling: now, a copy of the first count, where t0 set by the
     * call would do */
    now = syTmrGetCount();
    t0 = now;
    ADXSJD_ExecServer();
    t1 = syTmrGetCount();
    adxt_svrcnt_sjd = syTmrDiffCount(t0, t1);
    ADXRNA_ExecServer();
    t2 = syTmrGetCount();
    adxt_svrcnt_rna = syTmrDiffCount(t1, t2);
    ADXF_ExecServer();
    t3 = syTmrGetCount();
    adxt_svrcnt_adxf = syTmrDiffCount(t2, t3);
    ADXSTM_ExecServer();
    t4 = syTmrGetCount();
    adxt_svrcnt_adxstm = syTmrDiffCount(t3, t4);
    for (i = 0; i < ADXT_MAX_OBJ; i++) {
        adxt = &adxt_obj[i];
        if (adxt->used == TRUE) {
            ADXT_ExecHndl(adxt);
        }
    }
    LSC_ExecServer();
    t5 = syTmrGetCount();
    adxt_svrcnt_hndl = syTmrDiffCount(t4, t5);
    adxt_svrcnt = syTmrDiffCount(t0, t5);
}

Sint32 ADXT_GetErrCode(ADXT adxt)
{
    return adxt->ercode;
}

void ADXT_ClearErrCode(ADXT adxt)
{
    adxt->ercode = 0;
    adxt->edecpos = 0;
    adxt->edeccnt = 0;
    adxt->eshrtcnt = 0;
}

Sint32 ADXT_GetLpCnt(ADXT adxt)
{
    return adxt->lpcnt;
}

void ADXT_SetLpFlg(ADXT adxt, Sint32 flg)
{
    adxt->lpflg = flg;
}

SJ ADXT_GetInputSj(ADXT adxt)
{
    return adxt->sji;
}

void ADXT_SetWaitPlayStart(ADXT adxt, Sint32 flg)
{
    adxt->pstwait_flag = flg;
}

Sint32 ADXT_IsReadyPlayStart(ADXT adxt)
{
    return adxt->pstready_flag;
}

void ADXT_Pause(ADXT adxt, Sint32 sw)
{
    Sint32 stat;
    Sint32 ncount;
    Sint32 tscale;
    Sint32 i;

    stat = ADXT_GetStat(adxt);
    adxt->pause_flag = sw;
    if (stat == ADXT_STAT_PLAYING || stat == ADXT_STAT_DECEND) {
        if (sw == TRUE) {
            ADXRNA_SetPlaySw(adxt->rna, FALSE);
            for (i = 0; i < PAUSE_WAIT; i++) {
            }
            mwRnaAdjustBuf(adxt->rna);
            ADXRNA_GetTime(adxt->rna, &ncount, &tscale);
            adxt->tvofst =
                (Float)ncount / (Float)tscale * (Float)adxt_time_unit;
        } else {
            ADXRNA_SetPlaySw(adxt->rna, TRUE);
            ADXRNA_GetTime(adxt->rna, &ncount, &tscale);
            adxt->tvofst =
                (Float)ncount / (Float)tscale * (Float)adxt_time_unit;
            adxt->svcnt = adxt_vsync_cnt;
        }
    }
}

void ADXT_SetDrctLvl(ADXT adxt, Sint32 drctlvl)
{
    mwRnaSetDrctLvl(adxt->rna, drctlvl);
}

Sint32 ADXT_GetDrctLvl(ADXT adxt)
{
    return mwRnaGetDrctLvl(adxt->rna);
}

void ADXT_SetFx(ADXT adxt, Sint32 fxch, Sint32 fxlvl)
{
    mwRnaSetFx(adxt->rna, fxch, fxlvl);
}

void ADXT_GetFx(ADXT adxt, Sint32 *fxch, Sint32 *fxlvl)
{
    mwRnaGetFx(adxt->rna, fxch, fxlvl);
}

void ADXT_SetFilter(ADXT adxt, Sint32 coff, Sint32 q)
{
    mwRnaSetFilter(adxt->rna, coff, q);
}

void ADXT_GetFilter(ADXT adxt, Sint32 *coff, Sint32 *q)
{
    mwRnaGetFilter(adxt->rna, coff, q);
}

void ADXT_SetTranspose(ADXT adxt, Sint32 transps, Sint32 detune)
{
    mwRnaSetPitch(adxt->rna, transps, detune);
}

void ADXT_GetTranspose(ADXT adxt, Sint32 *transps, Sint32 *detune)
{
    mwRnaGetPitch(adxt->rna, transps, detune);
}

void ADXT_EntryErrFunc(void (*func)(void *obj, char *msg), void *obj)
{
    ADXERR_EntryErrFunc(func, obj);
}

Sint32 ADXT_SetPauseBuf(void)
{
    return PAUSE_BUF_RESULT;
}

Sint32 ADXT_DiscardSmpl(ADXT adxt, Sint32 nsmpl)
{
    Sint32 ncount;
    Sint32 tscale;
    Sint32 ndiscard;

    if (adxt->pause_flag == FALSE) {
        return 0;
    }
    ndiscard = ADXRNA_DiscardData(adxt->rna, nsmpl);
    ADXT_ExecServer();
    ADXRNA_GetTime(adxt->rna, &ncount, &tscale);
    adxt->tvofst = (Float)ncount / (Float)tscale * (Float)adxt_time_unit;
    adxt->svcnt = adxt_vsync_cnt;
    return ndiscard;
}

void ADXT_SetTimeOfst(ADXT adxt, Sint32 ofst)
{
    adxt->time_ofst = ofst;
}

void ADXT_SetLnkSw(ADXT adxt, Sint32 sw)
{
    adxt->lnkflg = sw;
}

Sint32 ADXT_GetLnkSw(ADXT adxt)
{
    return adxt->lnkflg;
}

void ADXT_EntryFltFunc(ADXT adxt,
                       void (*f)(void *obj, Sint32 ch, void *data,
                                 Sint32 dtlen),
                       void *obj)
{
    ADXSJD_EntryFltFunc(adxt->sjd, f, obj);
}

Sint32 ADXT_IsHeader(char *adr, Sint32 siz, Sint32 *hdrsiz)
{
    Sint16 dtofs;
    Sint8 enc;
    Sint8 bps;
    Sint8 blklen;
    Sint8 nch;
    Sint32 sfreq;
    Sint32 total;
    Sint32 blksmpl;
    Uint16 mark;
    Sint32 ret;

    if (siz < ADX_MARK_LEN) {
        return FALSE;
    }
    /* @hack spelling: mark, a local for the first halfword, where the
     * halfword in the test would do */
    mark = ADX_SWAP16(*(Uint16 *)adr);
    if (mark != ADX_MARK) {
        return FALSE;
    }
    /* @hack spelling: ret, a local for ADX_DecodeInfo's result, where the
     * call in the test would do */
    ret = ADX_DecodeInfo((Sint8 *)adr, siz, &dtofs, &enc, &bps, &blklen, &nch,
                         &sfreq, &total, &blksmpl);
    if (ret < 0) {
        return FALSE;
    }
    *hdrsiz = dtofs;
    return TRUE;
}

/* adr is a char *, as cri_adxt.h declares it, though the call only reads
 * it */
/* NOLINTNEXTLINE(readability-non-const-parameter) */
Sint32 ADXT_IsEndcode(char *adr, Sint32 siz, Sint32 *endsiz)
{
    Uint16 mark;

    if (siz < ADX_MARK_LEN) {
        return FALSE;
    }
    /* @hack spelling: mark, a local for the first halfword, where the
     * halfword in the test would do */
    mark = ADX_SWAP16(*(Uint16 *)adr);
    if (mark != ADX_END_MARK) {
        return FALSE;
    }
    *endsiz = siz;
    return TRUE;
}

Sint32 ADXT_InsertSilence(ADXT adxt, Sint32 nch, Sint32 nsmpl)
{
    SJ sj;
    SJCK ck;
    SJCK rest;
    Sint32 nbyte;
    Sint32 len;
    Sint32 total;
    /* @hack spelling: nblk declared ahead of blksize, where the order of
     * their first uses would do */
    Sint32 nblk;
    Sint32 blksize;

    sj = adxt->sji;
    if (sj == NULL) {
        return 0;
    }
    nblk = nsmpl / BLK_SMPL;
    blksize = nch * BLK_SIZE;
    nbyte = blksize * nblk;
    total = 0;
    SJ_GetChunk(sj, SJ_LIN_FREE, nbyte, &ck);
    len = ck.len / blksize * blksize;
    memset(ck.data, 0, len);
    SJ_SplitChunk(&ck, len, &ck, &rest);
    SJ_PutChunk(sj, SJ_LIN_DATA, &ck);
    SJ_UngetChunk(sj, SJ_LIN_FREE, &rest);
    /* @hack spelling: nbyte = nbyte - len, where nbyte -= len would do */
    nbyte = nbyte - len;
    total += len;
    SJ_GetChunk(sj, SJ_LIN_FREE, nbyte, &ck);
    len = ck.len / blksize * blksize;
    memset(ck.data, 0, len);
    SJ_SplitChunk(&ck, len, &ck, &rest);
    SJ_PutChunk(sj, SJ_LIN_DATA, &ck);
    SJ_UngetChunk(sj, SJ_LIN_FREE, &rest);
    total += len;
    return total / blksize * BLK_SMPL;
}
