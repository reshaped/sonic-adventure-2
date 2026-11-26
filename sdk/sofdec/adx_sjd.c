/* ADX's stream decoders (ADXSJD). each runs a basic decoder (ADXB) from an
 * input joint into a joint for each channel */
#include <adx_bsc.h>
#include <adx_crs.h>
#include <adx_dcd.h>
#include <adx_sjd.h>
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

#include "../sofdec/adx_bendian.h"

/* the most decoders in use at once */
#define ADXSJD_OBJ_MAX 8
/* the most input adxsjd_decode_prep reads a header from, in bytes */
#define MAX_HDLEN 0x1000
/* the bytes of an SPSD header kept for the sound output */
#define SPSD_LEN 64
/* the most bytes a step takes from each output joint's free line */
#define MAX_OUTLEN 0x4000
/* the largest Sint32, a limit that never binds. adxsjd_clear sets
 * maxdecsmpl to it, and a step asks sji for that many bytes */
#define NO_LIMIT 0x7fffffff
/* the trapnsmpl adxsjd_clear sets, for no trap */
#define NO_TRAP (-1)
/* the *nleft adxsjd_get_wr gives while no trap is set */
#define NO_TRAP_LEFT 0x1fffffff

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* a decoder, which runs adxb from sji into sjo's joints, one for each
 * channel */
typedef struct _adxsjd_obj {
    Sint8 used;
    Sint8 stat; /* an ADXSJD_STAT_ value */
    Sint8 maxnch;
    /* TRUE for AU data, whose decode ends once sji holds no data */
    Sint8 endonempty;
    ADXB adxb;
    SJ sji;
    SJ sjo[ADXT_MAX_NCH];
    SJCK ick; /* the input chunk the step in progress decodes */
    SJCK ock[ADXT_MAX_NCH]; /* each channel's free chunk the step fills */
    /* samples of each channel put out since ADXSJD_Start */
    Sint32 decnsmpl;
    Sint32 decdtlen; /* input bytes decoded since ADXSJD_Start */
    Sint32 decpos; /* the stream's sample the next step decodes first */
    Sint32 maxdecsmpl; /* the most samples of each channel a step decodes */
    /* the trap. trapfunc, if set, runs with trapobj after each step that
     * leaves trapcnt at trapnsmpl or past it. a negative trapnsmpl sets no
     * trap */
    Sint32 trapnsmpl;
    Sint32 trapcnt; /* each step adds the samples it puts out */
    Sint32 trapdtlen; /* each step adds the input bytes it takes */
    void (*trapfunc)(void *obj);
    void *trapobj;
    /* called with fltobj for each channel's samples a step puts out */
    void (*fltfunc)(void *obj, Sint32 ch, void *data, Sint32 dtlen);
    void *fltobj;
    char spsd[SPSD_LEN]; /* the start of an SPSD header */
    Sint32 hdrlen; /* the header's bytes, or 0 before it is read */
} ADXSJD_OBJ;

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void adxsjd_clear(ADXSJD sjd);
void ADXSJD_SetOutSj(ADXSJD sjd, Sint32 ch, SJ sjo);
void adxsjd_decode_prep(ADXSJD sjd);
Sint16 *adxsjd_get_wr(void *obj, Sint32 *wpos, Sint32 *nroom, Sint32 *nleft);
void adxsjd_decexec_start(ADXSJD sjd);
void adxsjd_decexec_end(ADXSJD sjd);
void adxsjd_decode_exec(ADXSJD sjd);
Sint32 ADXSJD_GetDecDtLen(ADXSJD sjd);
Sint32 ADXSJD_GetDecPos(ADXSJD sjd);
Sint32 ADXSJD_GetTrapNumSmpl(ADXSJD sjd);
Sint32 ADXSJD_GetTrapCnt(ADXSJD sjd);
Sint32 ADXSJD_GetTrapDtLen(ADXSJD sjd);
/* each returns the basic decoder's call of its name */
Sint32 ADXSJD_GetBlkLen(ADXSJD sjd);
Sint32 ADXSJD_GetCof(ADXSJD sjd);
Sint32 ADXSJD_GetLpInsNsmpl(ADXSJD sjd);

/* every decoder, in use or free */
ADXSJD_OBJ adxsjd_obj[ADXSJD_OBJ_MAX];
/* NOLINTEND(misc-use-internal-linkage) */

void ADXSJD_Init(void)
{
    ADXB_Init();
    memset(adxsjd_obj, 0, sizeof(adxsjd_obj));
}

void ADXSJD_Finish(void)
{
    memset(adxsjd_obj, 0, sizeof(adxsjd_obj));
}

void adxsjd_clear(ADXSJD sjd)
{
    sjd->hdrlen = 0;
    sjd->decnsmpl = 0;
    sjd->decdtlen = 0;
    sjd->decpos = 0;
    sjd->maxdecsmpl = NO_LIMIT;
    sjd->trapnsmpl = NO_TRAP;
    sjd->trapcnt = 0;
    sjd->trapdtlen = 0;
    sjd->endonempty = FALSE;
}

ADXSJD ADXSJD_Create(SJ sji, Sint32 maxnch, SJ *sjo)
{
    ADXSJD sjd;
    SJ sj;
    Sint32 i;
    Sint16 *buf;
    Sint32 bsize;

    sj = sjo[0];
    for (i = 0; i < ADXSJD_OBJ_MAX; i++) {
        if (adxsjd_obj[i].used == FALSE) {
            break;
        }
    }
    if (i == ADXSJD_OBJ_MAX) {
        return NULL;
    }
    sjd = &adxsjd_obj[i];
    buf = (Sint16 *)SJRBF_GetBufPtr(sj);
    bsize = SJRBF_GetBufSize(sj) / sizeof(Sint16);
    sjd->adxb = ADXB_Create(maxnch, buf, bsize,
                            bsize + (SJRBF_GetXtrSize(sj) / sizeof(Sint16)));
    /* @bug memory: ADXB_EntryGetWrFunc runs before the null test (JSR at
     * 0x8c156028, TST at 0x8c15602e). where ADXB_Create fails, it stores
     * adxsjd_get_wr and sjd through a null adxb */
    ADXB_EntryGetWrFunc(sjd->adxb, adxsjd_get_wr, sjd);
    if (sjd->adxb == NULL) {
        return NULL;
    }
    sjd->sji = sji;
    sjd->maxnch = maxnch;
    for (i = 0; i < maxnch; i++) {
        sjd->sjo[i] = sjo[i];
    }
    sjd->stat = ADXSJD_STAT_STOP;
    adxsjd_clear(sjd);
    sjd->trapfunc = NULL;
    sjd->trapobj = NULL;
    sjd->fltfunc = NULL;
    sjd->fltobj = NULL;
    sjd->used = TRUE;
    return sjd;
}

void ADXSJD_Destroy(ADXSJD sjd)
{
    if (sjd != NULL) {
        if (sjd->adxb != NULL) {
            ADXB_Destroy(sjd->adxb);
        }
        ADXCRS_Lock();
        memset(sjd, 0, sizeof(ADXSJD_OBJ));
        ADXCRS_Unlock();
    }
}

Sint32 ADXSJD_GetStat(ADXSJD sjd)
{
    return sjd->stat;
}

void ADXSJD_SetInSj(ADXSJD sjd, SJ sji)
{
    sjd->sji = sji;
}

void ADXSJD_SetOutSj(ADXSJD sjd, Sint32 ch, SJ sjo)
{
    sjd->sjo[ch] = sjo;
}

void ADXSJD_SetMaxDecSmpl(ADXSJD sjd, Sint32 nsmpl)
{
    sjd->maxdecsmpl = nsmpl;
}

void ADXSJD_Start(ADXSJD sjd)
{
    adxsjd_clear(sjd);
    sjd->stat = ADXSJD_STAT_DECINFO;
}

void ADXSJD_Stop(ADXSJD sjd)
{
    ADXB_Stop(sjd->adxb);
    sjd->stat = ADXSJD_STAT_STOP;
}

/* read the data's header from one chunk of sji, free the header's bytes,
 * and start decoding. the chunk goes back for the next pass while it holds
 * fewer than ADX_HDR_INFO_LEN bytes. it goes back too while it holds no
 * header that ADXB_DecodeHeader reads whole */
void adxsjd_decode_prep(ADXSJD sjd)
{
    ADXB adxb;
    SJ sji;
    SJCK ck1;
    SJCK ck2;
    Sint32 hdlen;

    adxb = sjd->adxb;
    sji = sjd->sji;
    SJ_GetChunk(sji, SJ_LIN_DATA, MAX_HDLEN, &ck1);
    if (ck1.len < ADX_HDR_INFO_LEN) {
        SJ_UngetChunk(sji, SJ_LIN_DATA, &ck1);
        return;
    }
    hdlen = ADXB_DecodeHeader(adxb, (Sint8 *)ck1.data, ck1.len);
    if (hdlen == 0 || hdlen > ck1.len) {
        SJ_UngetChunk(sji, SJ_LIN_DATA, &ck1);
        return;
    }
    sjd->hdrlen = hdlen;
    if (ADXB_GetFormat(adxb) == ADXB_FORMAT_AU) {
        sjd->endonempty = TRUE;
    }
    if (ADXB_GetFormat(adxb) == ADXB_FORMAT_SPSD) {
        memcpy(sjd->spsd, ck1.data, MIN(ck1.len, SPSD_LEN));
    }
    SJ_SplitChunk(&ck1, hdlen, &ck1, &ck2);
    SJ_PutChunk(sji, SJ_LIN_FREE, &ck1);
    SJ_UngetChunk(sji, SJ_LIN_DATA, &ck2);
    sjd->stat = ADXSJD_STAT_DECODE;
}

/* the basic decoder's ADXB_GETWR: take a free chunk of up to MAX_OUTLEN
 * bytes from each channel's output joint. *wpos gets the first chunk's
 * offset in its ring, in samples. *nroom gets its samples, at most
 * maxdecsmpl. *nleft gets the samples left before the trap, or NO_TRAP_LEFT
 * with no trap. it returns the basic decoder's ring */
Sint16 *adxsjd_get_wr(void *obj, Sint32 *wpos, Sint32 *nroom, Sint32 *nleft)
{
    ADXSJD sjd = obj;
    SJ sjo;
    char *buf;
    Sint32 i;
    Sint32 n;

    sjo = sjd->sjo[0];
    for (i = 0; i < ADXB_GetNumChan(sjd->adxb); i++) {
        SJ_GetChunk(sjd->sjo[i], SJ_LIN_FREE, MAX_OUTLEN, &sjd->ock[i]);
    }
    buf = SJRBF_GetBufPtr(sjo);
    *wpos = (unsigned long)(sjd->ock[0].data - buf) / sizeof(Sint16);
    n = sjd->ock[0].len / sizeof(Sint16);
    *nroom = MIN(n, sjd->maxdecsmpl);
    if (sjd->trapnsmpl >= 0) {
        *nleft = sjd->trapnsmpl - sjd->trapcnt;
    } else {
        *nleft = NO_TRAP_LEFT;
    }
    return ADXB_GetPcmBuf(sjd->adxb);
}

/* start a step on one chunk of sji's data line. the step waits until the
 * first output joint has room for a block. the decoder becomes
 * ADXSJD_STAT_DECEND instead where its data is done. an ADX footer stands
 * where the next block's scale would */
void adxsjd_decexec_start(ADXSJD sjd)
{
    ADXB adxb;
    SJ sji;
    Sint32 nsmpl;
    Sint32 n;
    Uint16 mark;

    adxb = sjd->adxb;
    sji = sjd->sji;
    if (sjd->endonempty == TRUE && SJ_GetNumData(sji, SJ_LIN_DATA) == 0) {
        sjd->stat = ADXSJD_STAT_DECEND;
        return;
    }
    SJ_GetChunk(sji, SJ_LIN_DATA, NO_LIMIT, &sjd->ick);
    /* @hack spelling: the length test nested inside the format test, where
     * one test with && would do */
    if (ADXB_GetFormat(adxb) == ADXB_FORMAT_ADX) {
        if (sjd->ick.len >= ADX_MARK_LEN) {
            mark = ADX_SWAP16(*(Sint16 *)sjd->ick.data);
            if (mark == ADX_END_MARK) {
                sjd->stat = ADXSJD_STAT_DECEND;
                SJ_UngetChunk(sji, SJ_LIN_DATA, &sjd->ick);
                return;
            }
        }
    }
    if (sjd->decpos >= ADXSJD_GetTotalNumSmpl(sjd)) {
        sjd->stat = ADXSJD_STAT_DECEND;
        SJ_UngetChunk(sji, SJ_LIN_DATA, &sjd->ick);
        return;
    }
    nsmpl = ADXSJD_GetBlkSmpl(sjd);
    n = SJ_GetNumData(sjd->sjo[0], SJ_LIN_FREE) / sizeof(Sint16);
    if (n < nsmpl) {
        SJ_UngetChunk(sji, SJ_LIN_DATA, &sjd->ick);
        return;
    }
    ADXB_EntryData(adxb, (Sint8 *)sjd->ick.data, sjd->ick.len);
    ADXB_Start(adxb);
}

/* end a step. the samples it decoded past the data's last one never reach
 * an output joint's data line */
void adxsjd_decexec_end(ADXSJD sjd)
{
    ADXB adxb;
    SJ sji;
    SJCK ck1;
    SJCK ck2;
    Sint32 total;
    Sint32 dtlen;
    Sint32 decn;
    Sint32 nsmpl;
    Sint32 i;

    adxb = sjd->adxb;
    sji = sjd->sji;
    total = ADXB_GetTotalNumSmpl(adxb);
    dtlen = ADXB_GetDecDtLen(adxb);
    decn = ADXB_GetDecNumSmpl(adxb);
    nsmpl = MIN(decn, total - sjd->decpos);
    SJ_SplitChunk(&sjd->ick, dtlen, &ck1, &ck2);
    SJ_PutChunk(sji, SJ_LIN_FREE, &ck1);
    SJ_UngetChunk(sji, SJ_LIN_DATA, &ck2);
    for (i = 0; i < ADXB_GetNumChan(sjd->adxb); i++) {
        SJ_SplitChunk(&sjd->ock[i], nsmpl * sizeof(Sint16), &ck1, &ck2);
        if (sjd->fltfunc != NULL) {
            sjd->fltfunc(sjd->fltobj, i, ck1.data, ck1.len);
        }
        SJ_PutChunk(sjd->sjo[i], SJ_LIN_DATA, &ck1);
        SJ_UngetChunk(sjd->sjo[i], SJ_LIN_FREE, &ck2);
    }
    sjd->decnsmpl += nsmpl;
    sjd->decdtlen += dtlen;
    sjd->decpos += nsmpl;
    sjd->trapcnt += nsmpl;
    sjd->trapdtlen += dtlen;
    /* @hack spelling: the test of trapcnt nested inside the test of
     * trapnsmpl, where one test with && would do */
    if (sjd->trapnsmpl >= 0) {
        if (sjd->trapcnt >= sjd->trapnsmpl && sjd->trapfunc != NULL) {
            sjd->trapfunc(sjd->trapobj);
        }
    }
    ADXB_Reset(adxb);
}

void adxsjd_decode_exec(ADXSJD sjd)
{
    ADXB adxb;

    adxb = sjd->adxb;
    if (ADXB_GetStat(adxb) == ADXB_STAT_STOP) {
        adxsjd_decexec_start(sjd);
    }
    ADXB_ExecHndl(adxb);
    if (ADXB_GetStat(adxb) == ADXB_STAT_END) {
        adxsjd_decexec_end(sjd);
    }
}

void ADXSJD_ExecHndl(ADXSJD sjd)
{
    if (sjd->stat == ADXSJD_STAT_DECODE) {
        adxsjd_decode_exec(sjd);
    } else if (sjd->stat == ADXSJD_STAT_DECINFO) {
        adxsjd_decode_prep(sjd);
    }
}

void ADXSJD_ExecServer(void)
{
    ADXSJD sjd;
    Sint32 i;

    for (i = 0; i < ADXSJD_OBJ_MAX; i++) {
        sjd = &adxsjd_obj[i];
        if (sjd->used == TRUE) {
            ADXSJD_ExecHndl(sjd);
        }
    }
}

Sint32 ADXSJD_GetDecDtLen(ADXSJD sjd)
{
    return sjd->decdtlen;
}

Sint32 ADXSJD_GetDecNumSmpl(ADXSJD sjd)
{
    return sjd->decnsmpl;
}

void ADXSJD_SetDecPos(ADXSJD sjd, Sint32 pos)
{
    sjd->decpos = pos;
}

Sint32 ADXSJD_GetDecPos(ADXSJD sjd)
{
    return sjd->decpos;
}

void ADXSJD_EntryFltFunc(ADXSJD sjd,
                         void (*f)(void *obj, Sint32 ch, void *data,
                                   Sint32 dtlen),
                         void *obj)
{
    sjd->fltfunc = f;
    sjd->fltobj = obj;
}

void ADXSJD_EntryTrapFunc(ADXSJD sjd, void (*func)(void *obj), void *obj)
{
    sjd->trapfunc = func;
    sjd->trapobj = obj;
}

void ADXSJD_SetTrapNumSmpl(ADXSJD sjd, Sint32 nsmpl)
{
    sjd->trapnsmpl = nsmpl;
}

Sint32 ADXSJD_GetTrapNumSmpl(ADXSJD sjd)
{
    return sjd->trapnsmpl;
}

void ADXSJD_SetTrapCnt(ADXSJD sjd, Sint32 cnt)
{
    sjd->trapcnt = cnt;
}

Sint32 ADXSJD_GetTrapCnt(ADXSJD sjd)
{
    return sjd->trapcnt;
}

void ADXSJD_SetTrapDtLen(ADXSJD sjd, Sint32 len)
{
    sjd->trapdtlen = len;
}

Sint32 ADXSJD_GetTrapDtLen(ADXSJD sjd)
{
    return sjd->trapdtlen;
}

Sint32 ADXSJD_GetFormat(ADXSJD sjd)
{
    return ADXB_GetFormat(sjd->adxb);
}

Sint32 ADXSJD_GetSfreq(ADXSJD sjd)
{
    return ADXB_GetSfreq(sjd->adxb);
}

Sint32 ADXSJD_GetNumChan(ADXSJD sjd)
{
    return ADXB_GetNumChan(sjd->adxb);
}

Sint32 ADXSJD_GetOutBps(ADXSJD sjd)
{
    return ADXB_GetOutBps(sjd->adxb);
}

Sint32 ADXSJD_GetBlkSmpl(ADXSJD sjd)
{
    return ADXB_GetBlkSmpl(sjd->adxb);
}

Sint32 ADXSJD_GetBlkLen(ADXSJD sjd)
{
    return ADXB_GetBlkLen(sjd->adxb);
}

Sint32 ADXSJD_GetTotalNumSmpl(ADXSJD sjd)
{
    return ADXB_GetTotalNumSmpl(sjd->adxb);
}

Sint32 ADXSJD_GetCof(ADXSJD sjd)
{
    return ADXB_GetCof(sjd->adxb);
}

Sint32 ADXSJD_GetNumLoop(ADXSJD sjd)
{
    return ADXB_GetNumLoop(sjd->adxb);
}

Sint32 ADXSJD_GetLpInsNsmpl(ADXSJD sjd)
{
    return ADXB_GetLpInsNsmpl(sjd->adxb);
}

Sint32 ADXSJD_GetLpStartPos(ADXSJD sjd)
{
    return ADXB_GetLpStartPos(sjd->adxb);
}

Sint32 ADXSJD_GetLpStartOfst(ADXSJD sjd)
{
    return ADXB_GetLpStartOfst(sjd->adxb);
}

Sint32 ADXSJD_GetLpEndPos(ADXSJD sjd)
{
    return ADXB_GetLpEndPos(sjd->adxb);
}

Sint32 ADXSJD_GetLpEndOfst(ADXSJD sjd)
{
    return ADXB_GetLpEndOfst(sjd->adxb);
}

Sint32 ADXSJD_GetHdrLen(ADXSJD sjd)
{
    return sjd->hdrlen;
}

Sint32 ADXSJD_GetFmtBps(ADXSJD sjd)
{
    return ADXB_GetFmtBps(sjd->adxb);
}

char *ADXSJD_GetSpsdInfo(ADXSJD sjd)
{
    return sjd->spsd;
}
