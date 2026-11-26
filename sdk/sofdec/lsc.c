/* CRI's linking stream controller (LSC): the calls on its handles, and its
 * status function. a handle reads a queue of files into one stream joint.
 * lsc_ini.c defines the handles */
#include <adx_stmc.h>
#include <cri_lsc.h>
#include <lsc_crs.h>
#include <lsc_err.h>
#include <lsc_ini.h>
#include <lsc_svr.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

/* the member exports lsc_Alloc, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
LSC lsc_Alloc(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* the greatest stream ID. the ID after it wraps to 0 */
#define SID_MAX 0x7fffffff
/* the flow limit a new handle takes from its bsize: eight tenths of it,
 * truncated */
#define FLOW_SHARE(nbyte) ((nbyte) * 8 / 10)

LSC_STATFN lsc_stat_func = NULL;
void *lsc_stat_obj1 = NULL;
void *lsc_stat_obj2 = NULL;

/* the first handle not in use, or null. lsc_Alloc doesn't mark it used */
LSC lsc_Alloc(void)
{
    /* @hack spelling: lsc, a local that starts null and takes the handle the
     * loop finds, where returning the handle from inside the loop, and null
     * after it, would do */
    LSC lsc = NULL;
    Sint32 i;

    for (i = 0; i < LSC_OBJ_MAX; i++) {
        if (lsc_obj[i].used == FALSE) {
            lsc = &lsc_obj[i];
            break;
        }
    }
    return lsc;
}

LSC LSC_Create(SJ sj)
{
    Sint32 mask;
    LSC lsc;
    LSC_SINFO *sinfo;

    if (sj == NULL) {
        LSC_CallErrFunc("E0001: Illigal parameter=sj (LSC_Create)");
        return NULL;
    }
    LSC_LockCrs(&mask);
    lsc = lsc_Alloc();
    if (lsc == NULL) {
        LSC_CallErrFunc("E0002: Not enough instance (LSC_Create)");
    } else {
        lsc->sj = sj;
        lsc->stat = LSC_STAT_STOP;
        lsc->bsize =
            SJ_GetNumData(sj, SJ_LIN_FREE) + SJ_GetNumData(sj, SJ_LIN_DATA);
        lsc->bufmin = FLOW_SHARE(lsc->bsize);
        /* @hack spelling: a pointer stepped through sinfo, where an index i
         * and lsc->sinfo[i] would do */
        for (sinfo = lsc->sinfo; sinfo < lsc->sinfo + LSC_STM_MAX; sinfo++) {
            sinfo->stat = LSC_STM_STAT_WAIT;
        }
        lsc->used = TRUE;
    }
    LSC_UnlockCrs(&mask);
    return lsc;
}

void LSC_Destroy(LSC lsc)
{
    Sint32 mask;

    if (lsc == NULL) {
        return;
    }
    LSC_LockCrs(&mask);
    LSC_Stop(lsc);
    /* @hack redundant: the store to used, which the memset on the next line
     * clears again */
    lsc->used = FALSE;
    memset(lsc, 0, sizeof(LSC_OBJ));
    LSC_UnlockCrs(&mask);
}

Sint32 LSC_EntryFname(LSC lsc, char *fname)
{
    ADXSTM stm;
    Sint32 nsct;

    stm = ADXSTM_OpenFname(fname, lsc->sj);
    if (stm == NULL) {
        LSC_CallErrFunc("E0004: Can not open '%s'", fname);
    }
    /* @bug memory: reads the stream's size through a null pointer when
     * ADXSTM_OpenFname returns null, because the function goes on after its
     * report (JSR to ADXSTM_GetFileLen at 0x8c15897e). the file is then
     * entered with the size that read finds, which isn't the file's */
    nsct = ADXSTM_SCT_COUNT(ADXSTM_GetFileLen(stm));
    ADXSTM_Close(stm);
    return LSC_EntryFileRange(lsc, fname, NULL, 0, nsct);
}

Sint32 LSC_EntryFileRange(LSC lsc, char *fname, void *dir, Sint32 ofst,
                          Sint32 nsct)
{
    Sint32 pos;
    Sint32 prev;
    Sint32 sid;
    LSC_SINFO *sinfo;

    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return -1;
    }
    if (lsc->nstm >= LSC_STM_MAX) {
        return -1;
    }
    if (fname == NULL) {
        /* @bug memory: reads through the null fname it reports. the format's
         * %s has vsprintf take the length of the string at address 0 (JSR to
         * LSC_CallErrFunc at 0x8c1589ea) */
        LSC_CallErrFunc("E0011: Illigal parameter fname=%s", fname);
        return -1;
    }
    /* @hack spelling: pos, a local for the index of the slot before wpos,
     * where the expression written as the subscript would do */
    pos = (lsc->wpos + LSC_STM_MAX - 1) % LSC_STM_MAX;
    prev = lsc->sinfo[pos].sid;
    if (prev == SID_MAX) {
        sid = 0;
    } else {
        sid = prev + 1;
    }
    sinfo = &lsc->sinfo[lsc->wpos];
    sinfo->sid = sid;
    strncpy(sinfo->fname, fname, LSC_FNAME_MAX);
    sinfo->ofst = ofst;
    sinfo->fsct = nsct;
    sinfo->dir = dir;
    sinfo->stat = LSC_STM_STAT_WAIT;
    sinfo->rdsct = 0;
    lsc->nstm++;
    lsc->wpos = (lsc->wpos + 1) % LSC_STM_MAX;
    if (lsc->stat == LSC_STAT_WAIT) {
        lsc->stat = LSC_STAT_EXEC;
    }
    return sid;
}

void LSC_Start(LSC lsc)
{
    Sint32 mask;

    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return;
    }
    LSC_LockCrs(&mask);
    if (lsc->stat != LSC_STAT_STOP) {
        LSC_Stop(lsc);
    }
    if (lsc->nstm > 0) {
        lsc->stat = LSC_STAT_EXEC;
    } else {
        lsc->stat = LSC_STAT_WAIT;
    }
    LSC_UnlockCrs(&mask);
}

void LSC_Stop(LSC lsc)
{
    Sint32 mask;

    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return;
    }
    LSC_LockCrs(&mask);
    /* @hack redundant: a second test of lsc, which is never null here */
    if (lsc == NULL) {
        return;
    }
    if (lsc->fp != NULL) {
        if (lsc->rdflg == TRUE) {
            ADXSTM_Stop(lsc->fp);
            lsc->rdflg = FALSE;
        }
        ADXSTM_Close(lsc->fp);
        lsc->fp = NULL;
    }
    lsc->fsct = 0;
    lsc->wpos = 0;
    lsc->rpos = 0;
    lsc->nstm = 0;
    lsc->errcnt = 0;
    lsc->stat = LSC_STAT_STOP;
    LSC_UnlockCrs(&mask);
}

void LSC_Pause(LSC lsc, Sint32 sw)
{
    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return;
    }
    lsc->pause = (sw == TRUE);
}

void LSC_ExecServer(void)
{
    Sint32 mask;
    Sint32 i;

    LSC_LockCrs(&mask);
    for (i = 0; i < LSC_OBJ_MAX; i++) {
        if (lsc_obj[i].used == TRUE) {
            lsc_ExecHndl(&lsc_obj[i]);
        }
    }
    LSC_UnlockCrs(&mask);
}

Sint32 LSC_GetStat(LSC lsc)
{
    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return -1;
    }
    return lsc->stat;
}

Sint32 LSC_GetNumStm(LSC lsc)
{
    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return -1;
    }
    return lsc->nstm;
}

Sint32 LSC_GetStmId(LSC lsc, Sint32 no)
{
    Sint32 pos;

    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return -1;
    }
    if (no < 0 || no >= lsc->nstm) {
        LSC_CallErrFunc("E0009: Illigal parameter no=%d", no);
        return -1;
    }
    /* @hack spelling: pos, a local for the index of file no's slot, where
     * the expression written as the subscript would do */
    pos = (lsc->rpos + no) % LSC_STM_MAX;
    return lsc->sinfo[pos].sid;
}

char *LSC_GetStmFname(LSC lsc, Sint32 sid)
{
    Sint32 i;

    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return NULL;
    }
    for (i = 0; i < LSC_STM_MAX; i++) {
        if (lsc->sinfo[i].sid == sid) {
            break;
        }
    }
    if (i == LSC_STM_MAX) {
        LSC_CallErrFunc("E0012: Can not find stream ID =%d", sid);
        return NULL;
    }
    return lsc->sinfo[i].fname;
}

Sint32 LSC_GetStmStat(LSC lsc, Sint32 sid)
{
    Sint32 i;

    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return -1;
    }
    for (i = 0; i < LSC_STM_MAX; i++) {
        if (lsc->sinfo[i].sid == sid) {
            break;
        }
    }
    if (i == LSC_STM_MAX) {
        LSC_CallErrFunc("E0012: Can not find stream ID =%d", sid);
        return -1;
    }
    return lsc->sinfo[i].stat;
}

Sint32 LSC_GetStmRdSct(LSC lsc, Sint32 sid)
{
    Sint32 i;

    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return 0;
    }
    for (i = 0; i < LSC_STM_MAX; i++) {
        if (lsc->sinfo[i].sid == sid) {
            break;
        }
    }
    if (i == LSC_STM_MAX) {
        LSC_CallErrFunc("E0012: Can not find stream ID =%d", sid);
        return 0;
    }
    return lsc->sinfo[i].rdsct;
}

void LSC_SetFlowLimit(LSC lsc, Sint32 min)
{
    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return;
    }
    if (min < 0 || min > lsc->bsize) {
        LSC_CallErrFunc("E0010: Illigal parameter min=%d", min);
        return;
    }
    lsc->bufmin = min;
}

Sint32 LSC_GetFlowLimit(LSC lsc)
{
    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return -1;
    }
    return lsc->bufmin;
}

void LSC_EntryChgStatFunc(LSC_STATFN func, void *obj1, void *obj2)
{
    if (func == NULL) {
        lsc_stat_func = NULL;
        lsc_stat_obj1 = NULL;
        lsc_stat_obj2 = NULL;
        return;
    }
    lsc_stat_func = func;
    lsc_stat_obj1 = obj1;
    lsc_stat_obj2 = obj2;
}

void LSC_CallStatFunc(void)
{
    if (lsc_stat_func != NULL) {
        lsc_stat_func(lsc_stat_obj1, lsc_stat_obj2);
    }
}

void LSC_SetLpFlg(LSC lsc, Sint32 sw)
{
    if (lsc == NULL) {
        LSC_CallErrFunc("E0003: Illigal parameter lsc=NULL");
        return;
    }
    lsc->lpflg = sw;
}
