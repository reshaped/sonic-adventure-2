/* LSC's server step for one handle, by the state of the file at its queue's
 * head.
 *
 * the source doesn't include lsc_err.h, so LSC_CallErrFunc is undeclared
 * here. SHC passes an undeclared function's arguments in registers, as the
 * image's calls from this source pass them. LSC_CallErrFunc reads them from
 * the stack, where lsc_err.h's declaration would have SHC pass them */
/* NOLINTBEGIN(clang-diagnostic-implicit-function-declaration) */
#include <adx_stmc.h>
#include <cri_lsc.h>
#include <lsc_svr.h>
#include <sg_xpt.h>
#include <string.h>
#ifdef NONMATCHING
/* GCC's side includes the declaration, so each call passes LSC_CallErrFunc
 * its arguments where it reads them */
#include <lsc_err.h>
#endif

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void lsc_StatWait(LSC lsc);
void lsc_StatRead(LSC lsc);
void lsc_StatEnd(LSC lsc);
/* NOLINTEND(misc-use-internal-linkage) */

/* bytes the member reserves that no code reads */
static char dat_8c2df5a8[40];
/* the name of the file lsc_StatEnd enters again. its last byte stays 0, which
 * ends a name the queue keeps without a terminator */
static char lsc_loop_fname[LSC_FNAME_MAX];

/* open the stream of the file at the queue's head unless one is open. start
 * the stream with the handle's flow limit unless it is started. the file
 * becomes LSC_STM_STAT_READ */
void lsc_StatWait(LSC lsc)
{
    LSC_SINFO *sinfo;
    Sint32 nsct;

    sinfo = &lsc->sinfo[lsc->rpos];
    if (lsc->nstm > 0) {
        if (lsc->fp == NULL) {
            lsc->fp = ADXSTM_OpenFileRangeExRt(
                sinfo->fname, sinfo->dir, sinfo->ofst, sinfo->fsct, lsc->sj);
            if (lsc->fp == NULL) {
                /* @bug mismatched-call: passes the format and fname in R4 and
                 * R5, as the head says. the report is formatted from whatever
                 * the stack holds (JSR at 0x8c17043c) */
                LSC_CallErrFunc("E0004: Can not open '%s'", sinfo->fname);
            }
            /* @bug memory: reads and writes through a null fp when
             * ADXSTM_OpenFileRangeExRt returns null, because the function
             * goes on after its report. ADXSTM_GetFileLen reads through it
             * first (JSR at 0x8c170442). ADXSTM_SetBufSize, ADXSTM_Seek and
             * ADXSTM_Start follow */
            /* @hack spelling: nsct, a local that holds the stream's sectors
             * while rdflg is cleared, where lsc->fsct set from the call
             * would do */
            nsct = ADXSTM_SCT_COUNT(ADXSTM_GetFileLen(lsc->fp));
            lsc->rdflg = FALSE;
            lsc->fsct = nsct;
            sinfo->rdsct = 0;
        }
        if (lsc->rdflg == FALSE) {
            ADXSTM_SetBufSize(lsc->fp, lsc->bufmin, lsc->bsize);
            ADXSTM_Seek(lsc->fp, 0);
            ADXSTM_Start(lsc->fp);
            lsc->rdflg = TRUE;
        }
        sinfo->stat = LSC_STM_STAT_READ;
    } else if (lsc->fp != NULL) {
        /* @bug mismatched-call: passes the format, nstm and fp in R4 to R6,
         * as the head says (JMP at 0x8c17049c) */
        LSC_CallErrFunc("E0006: Illigal member nstm='%d' fp='%08x'h", lsc->nstm,
                        lsc->fp);
    }
}

/* follow the stream of the file at the queue's head. while the stream
 * reads, the file's rdsct takes its position. once it has read to its end,
 * rdsct takes its size, and the file becomes LSC_STM_STAT_END. a failed
 * stream makes the handle LSC_STAT_ERR */
void lsc_StatRead(LSC lsc)
{
    LSC_SINFO *sinfo;
    Sint32 stat;

    if (lsc->fp == NULL) {
        /* @bug mismatched-call: passes the format in R4, as the head says
         * (JMP at 0x8c1704be) */
        LSC_CallErrFunc("E0007: lsc->fp=NULL");
        return;
    }
    sinfo = &lsc->sinfo[lsc->rpos];
    stat = ADXSTM_GetStat(lsc->fp);
    if (stat == ADXSTM_STAT_ERROR) {
        lsc->stat = LSC_STAT_ERR;
    } else if (stat == ADXSTM_STAT_READING) {
        sinfo->rdsct = ADXSTM_Tell(lsc->fp);
    } else if (stat == ADXSTM_STAT_READEND) {
        sinfo->rdsct = lsc->fsct;
        sinfo->stat = LSC_STM_STAT_END;
    }
}

/* close the stream of the file at the queue's head, and take the file off
 * the queue. with lpflg TRUE, the call enters the file again at the queue's
 * end. a handle whose fp is null is left as it is */
void lsc_StatEnd(LSC lsc)
{
    LSC_SINFO *sinfo;
    void *dir;
    Sint32 ofst;
    Sint32 nsct;

    if (lsc->fp == NULL) {
        return;
    }
#ifdef NONMATCHING
    /* GCC warns that dir, ofst and nsct may be read unset, as the bug tag
     * below describes. that read needs lpflg set, and the game never sets
     * it */
    dir = NULL;
    ofst = 0;
    nsct = 0;
#endif
    ADXSTM_Close(lsc->fp);
    lsc->fp = NULL;
    if (lsc->lpflg == TRUE) {
        sinfo = &lsc->sinfo[lsc->rpos];
        memcpy(lsc_loop_fname, sinfo->fname, LSC_FNAME_MAX - 1);
        dir = sinfo->dir;
        ofst = sinfo->ofst;
        nsct = sinfo->fsct;
    }
    lsc->nstm--;
    lsc->rpos = (lsc->rpos + 1) % LSC_STM_MAX;
    if (lsc->nstm <= 0) {
        LSC_CallStatFunc();
        lsc->stat = LSC_STAT_WAIT;
    }
    /* @bug uninitialized: passes dir, ofst and nsct unset where lpflg is TRUE
     * here but wasn't at the test above (MOV.L @(8,R15),R2 at 0x8c1705ae).
     * between the two tests, LSC_CallStatFunc runs once the queue is empty,
     * and the function it calls can set lpflg. the game sets neither lpflg
     * nor such a function: no word in any image holds the address of
     * LSC_SetLpFlg or LSC_EntryChgStatFunc */
    if (lsc->lpflg == TRUE) {
        LSC_EntryFileRange(lsc, lsc_loop_fname, dir, ofst, nsct);
    }
}

void lsc_ExecHndl(LSC lsc)
{
    if (lsc->pause == TRUE || lsc->stat != LSC_STAT_EXEC || lsc->nstm <= 0) {
        return;
    }
    if (lsc->sinfo[lsc->rpos].stat == LSC_STM_STAT_READ) {
        lsc_StatRead(lsc);
    }
    if (lsc->sinfo[lsc->rpos].stat == LSC_STM_STAT_END) {
        lsc_StatEnd(lsc);
    }
    if (lsc->sinfo[lsc->rpos].stat == LSC_STM_STAT_WAIT) {
        lsc_StatWait(lsc);
    }
}
/* NOLINTEND(clang-diagnostic-implicit-function-declaration) */
