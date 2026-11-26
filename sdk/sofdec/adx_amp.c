/* CRI ADX's amplitude monitor (ADXAMP). a monitor reads each channel's
 * 16-bit samples from an input joint. it puts each frame's peak on the
 * channel's output joint */
#include <adx_amp.h>
#include <adx_crs.h>
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>
#ifdef NONMATCHING
/* this side's division sets errno to EDIV for a zero divisor, as SHC's runtime
 * does */
#include <errno.h>
#endif

/* how many monitors there are */
#define ADXAMP_OBJ_MAX 8
/* a monitor's states: stopped, and measuring */
#define ADXAMP_STAT_STOP 0
#define ADXAMP_STAT_EXEC 2
/* a new monitor's sampling rate in hertz, and its frames' length and period
 * in seconds */
#define DEF_SFREQ 44100
#define DEF_FRMLEN 0.1F
#define DEF_FRMPRD 0.1F

typedef struct _adxamp_obj {
    Sint8 used; /* TRUE while the monitor is in use */
    Sint8 stat; /* an ADXAMP_STAT_ value */
    Sint8 maxnch;
    SJ sji[ADXT_MAX_NCH];
    SJ sjo[ADXT_MAX_NCH];
    /* each channel's samples measured since ADXAMP_Start */
    Sint32 nsmpl[ADXT_MAX_NCH];
    /* the channels measured. ADXAMP_Create sets it to maxnch, and nothing
     * changes it */
    Sint32 nch;
    Sint32 sfreq;
    Float frmlen; /* in seconds */
    Float frmprd; /* in seconds. only ADXAMP_GetFrmPrd reads it */
    Sint32 nfrm; /* the frames measured, summed over the channels */
} ADXAMP_OBJ;

/* what a monitor puts on a channel's output joint for each frame */
typedef struct {
    Sint32 peak; /* the largest magnitude of the frame's samples */
    Sint32 pos; /* the frame's first sample, counted from ADXAMP_Start */
    Sint32 sfreq;
    Sint32 nsmpl; /* the frame's length in samples */
} AMP_FRAME;

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* ADXAMP_Init and ADXAMP_Finish nest in pairs. the outermost call of either
 * frees every monitor */
void ADXAMP_Init(void);
void ADXAMP_Finish(void);
/* a free monitor of maxnch channels, or null while every monitor is in use.
 * maxnch must be at most ADXT_MAX_NCH. the monitor reads channel ch from
 * sji[ch], and puts its frames on sjo[ch]. it starts stopped, at DEF_SFREQ,
 * in frames of DEF_FRMLEN */
ADXAMP ADXAMP_Create(Sint32 maxnch, SJ *sji, SJ *sjo);
/* the monitor's ADXAMP_STAT_ state */
Sint32 ADXAMP_GetStat(ADXAMP amp);
/* ADXAMP_ExecServer measures a monitor from its ADXAMP_Start to its
 * ADXAMP_Stop. ADXAMP_Start clears the monitor's counts first, and
 * ADXAMP_Stop keeps them */
void ADXAMP_Start(ADXAMP amp);
void ADXAMP_Stop(ADXAMP amp);
/* measure each whole frame that sji[ch] holds, while sjo[ch] has room for
 * its AMP_FRAME. the frame's samples go back to sji[ch]'s free line */
void adxamp_extract(ADXAMP amp);
/* ADXAMP_ExecHndl measures the monitor while it is ADXAMP_STAT_EXEC.
 * ADXAMP_ExecServer runs ADXAMP_ExecHndl on each monitor in use */
void ADXAMP_ExecHndl(ADXAMP amp);
void ADXAMP_ExecServer(void);
/* the samples of channel 0 measured since ADXAMP_Start */
Sint32 ADXAMP_GetExtractNumSmpl(ADXAMP amp);
/* the sampling rate, in hertz, that turns a frame's length into samples */
Sint32 ADXAMP_GetSfreq(ADXAMP amp);
/* set and get the frames' length and period, in seconds */
void ADXAMP_SetFrmLen(ADXAMP amp, Float frmlen);
Float ADXAMP_GetFrmLen(ADXAMP amp);
void ADXAMP_SetFrmPrd(ADXAMP amp, Float frmprd);
Float ADXAMP_GetFrmPrd(ADXAMP amp);

/* the ADXAMP_Init calls that no ADXAMP_Finish has answered */
Sint32 adxsmp_init_cnt = 0;
/* every monitor, in use or free */
ADXAMP_OBJ adxamp_obj[ADXAMP_OBJ_MAX];
/* NOLINTEND(misc-use-internal-linkage) */

void ADXAMP_Init(void)
{
    if (adxsmp_init_cnt == 0) {
        memset(adxamp_obj, 0, sizeof(adxamp_obj));
    }
    adxsmp_init_cnt++;
}

void ADXAMP_Finish(void)
{
    adxsmp_init_cnt--;
    if (adxsmp_init_cnt == 0) {
        memset(adxamp_obj, 0, sizeof(adxamp_obj));
    }
}

ADXAMP ADXAMP_Create(Sint32 maxnch, SJ *sji, SJ *sjo)
{
    ADXAMP amp;
    Sint32 i;

    for (i = 0; i < ADXAMP_OBJ_MAX; i++) {
        if (adxamp_obj[i].used == FALSE) {
            break;
        }
    }
    if (i == ADXAMP_OBJ_MAX) {
        return NULL;
    }
    ADXCRS_Lock();
    amp = &adxamp_obj[i];
    amp->maxnch = maxnch;
    for (i = 0; i < maxnch; i++) {
        amp->sji[i] = sji[i];
        amp->sjo[i] = sjo[i];
        amp->nsmpl[i] = 0;
    }
    amp->stat = ADXAMP_STAT_STOP;
    amp->nfrm = 0;
    amp->nch = maxnch;
    amp->sfreq = DEF_SFREQ;
    amp->frmlen = DEF_FRMLEN;
    amp->frmprd = DEF_FRMPRD;
    amp->used = TRUE;
    ADXCRS_Unlock();
    return amp;
}

void ADXAMP_Destroy(ADXAMP amp)
{
    if (amp != NULL) {
        ADXCRS_Lock();
        memset(amp, 0, sizeof(ADXAMP_OBJ));
        ADXCRS_Unlock();
    }
}

Sint32 ADXAMP_GetStat(ADXAMP amp)
{
    return amp->stat;
}

void ADXAMP_Start(ADXAMP amp)
{
    Sint32 i;

    for (i = 0; i < amp->maxnch; i++) {
        amp->nsmpl[i] = 0;
    }
    amp->nfrm = 0;
    amp->stat = ADXAMP_STAT_EXEC;
}

void ADXAMP_Stop(ADXAMP amp)
{
    amp->stat = ADXAMP_STAT_STOP;
}

void adxamp_extract(ADXAMP amp)
{
    Sint32 frmsmpl;
    Sint32 nsmpl;
    Sint32 nfrm;
    Sint32 nroom;
    Sint32 nbyte;
    Sint32 n;
    Sint32 peak;
    /* @hack redundant: register on ch, where a plain local would do */
    register Sint32 ch;
    /* @hack redundant: register on j, where a plain local would do */
    register Sint32 j;
    /* @hack redundant: register on k, where a plain local would do */
    register Sint32 k;
    /* @hack redundant: register on i, where a plain local would do */
    register Sint32 i;
    /* @hack redundant: register on p, where a plain local would do */
    register Sint16 *p;
    /* @hack redundant: register on v, where a plain local would do */
    register Sint32 v;
    SJCK ick;
    SJCK ock;
    AMP_FRAME *frm;

    frmsmpl = (Sint32)(amp->sfreq * amp->frmlen);
    for (ch = 0; ch < amp->nch; ch++) {
        nsmpl = SJ_GetNumData(amp->sji[ch], SJ_LIN_DATA) / sizeof(Sint16);
#ifdef NONMATCHING
        /* this side keeps the shipped result for a frame under one sample
         * long. for a zero divisor, SHC's runtime returns 0 and sets errno to
         * EDIV */
        if (frmsmpl == 0) {
            errno = EDIV;
            nfrm = 0;
        } else {
            nfrm = nsmpl / frmsmpl;
        }
#else
        nfrm = nsmpl / frmsmpl;
#endif
        nroom = SJ_GetNumData(amp->sjo[ch], SJ_LIN_FREE) / sizeof(AMP_FRAME);
        /* @hack spelling: the least of nfrm and nroom by a conditional, where
         * an if would do */
        nfrm = (nfrm < nroom) ? nfrm : nroom;
        for (j = 0; j < nfrm; j++) {
            peak = 0;
            for (k = 0; k < frmsmpl; k += n) {
                /* @hack spelling: nbyte, a local for the bytes asked for,
                 * where the product in the call would do */
                nbyte = (frmsmpl - k) * sizeof(Sint16);
                SJ_GetChunk(amp->sji[ch], SJ_LIN_DATA, nbyte, &ick);
                n = ick.len / sizeof(Sint16);
                p = (Sint16 *)ick.data;
                for (i = 0; i < n; i++) {
                    v = *p;
                    if (v < 0) {
                        v = -v;
                    }
                    if (v > peak) {
                        peak = v;
                    }
                    p++;
                }
                SJ_PutChunk(amp->sji[ch], SJ_LIN_FREE, &ick);
            }
            SJ_GetChunk(amp->sjo[ch], SJ_LIN_FREE, sizeof(AMP_FRAME), &ock);
            /* nroom counted room for the frame, so the chunk holds it. on an
             * empty chunk, adxamp_extract hangs here */
            if (ock.len == 0) {
                for (;;) {
                }
            }
            frm = (AMP_FRAME *)ock.data;
            frm->peak = peak;
            frm->pos = amp->nsmpl[ch];
            frm->sfreq = amp->sfreq;
            frm->nsmpl = frmsmpl;
            SJ_PutChunk(amp->sjo[ch], SJ_LIN_DATA, &ock);
            amp->nsmpl[ch] += frmsmpl;
            amp->nfrm++;
        }
    }
}

void ADXAMP_ExecHndl(ADXAMP amp)
{
    if (amp->stat == ADXAMP_STAT_EXEC) {
        adxamp_extract(amp);
    }
}

void ADXAMP_ExecServer(void)
{
    ADXAMP amp;
    Sint32 i;

    for (i = 0; i < ADXAMP_OBJ_MAX; i++) {
        amp = &adxamp_obj[i];
        if (amp->used == TRUE) {
            ADXAMP_ExecHndl(amp);
        }
    }
}

Sint32 ADXAMP_GetExtractNumSmpl(ADXAMP amp)
{
    return amp->nsmpl[0];
}

void ADXAMP_SetSfreq(ADXAMP amp, Sint32 sfreq)
{
    amp->sfreq = sfreq;
}

Sint32 ADXAMP_GetSfreq(ADXAMP amp)
{
    return amp->sfreq;
}

void ADXAMP_SetFrmLen(ADXAMP amp, Float frmlen)
{
    amp->frmlen = frmlen;
}

Float ADXAMP_GetFrmLen(ADXAMP amp)
{
    return amp->frmlen;
}

void ADXAMP_SetFrmPrd(ADXAMP amp, Float frmprd)
{
    amp->frmprd = frmprd;
}

Float ADXAMP_GetFrmPrd(ADXAMP amp)
{
    return amp->frmprd;
}
