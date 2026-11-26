/* CRI ADX's file readers (ADXSTM), which read cvfs files into stream joints */
#include <adx_crs.h>
#include <adx_errs.h>
#include <adx_stmc.h>
#include <gdci.h>
#include <sg_cvfs.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

/* a new reader's maxsct */
#define DEF_REQ_RD_SCT 512
/* the readers adxstmf_obj holds, for both pools */
#define ADXSTM_OBJ_MAX 32
/* the real-time pool's count until ADXT_SetupRtimeNumStm sets it */
#define DEF_RTIM_NUM 8
/* a reader retries RETRY_MAX + 1 failed reads in a row. the next failure,
 * with nretry past RETRY_MAX, makes it ADXSTM_STAT_ERROR */
#define RETRY_MAX 10
/* the rw cvFsOpen passes on to the disc's Open, which takes only 0 */
#define OPEN_RW 0

/* a reader over the fsize bytes of file from its sector ofst. the reader's
 * read position is cvFsTell(file) less ofst, in sectors */
typedef struct _adxstm_obj {
    Sint8 used;
    Sint8 stat; /* an ADXSTM_STAT_ value */
    Sint8 reading; /* TRUE while a read is under way */
    Sint8 nretry; /* the reads that have failed in a row */
    SJ sj;
    struct cvfs_file *file;
    Sint32 stsct; /* the read position at ADXSTM_Start */
    Sint32 ofst;
    Sint32 fsize;
    Sint32 rdsize; /* the bytes of room a read asks sj for */
    /* a read starts while sj's data line holds fewer than bufsize bytes */
    Sint32 bufsize;
    Sint32 rdsct; /* the sectors of the read under way */
    SJCK ck; /* the room the read under way took from sj */
    Sint32 maxsct; /* the most sectors a read asks for */
    Sint32 eossct; /* the read limit, the sector no read passes */
    Sint32 rdbyte; /* the bytes read since ADXSTM_Start */
    /* eosfunc, if set, runs with eosobj after a read that leaves the read
     * position at eossct */
    void (*eosfunc)(void *obj);
    void *eosobj;
    char fname[ADXSTM_FNAME_MAX];
} ADXSTM_OBJ;

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void ADXT_SetupRtimeNumStm(Sint32 num);
void ADXT_SetupNrmlNumStm(Sint32 num);
void ADXSTM_Reset(void);
ADXSTM ADXSTM_OpenFileRange(char *fname, Sint32 ofst, Sint32 nsct, SJ sj);
Sint32 ADXSTM_GetBufSize(ADXSTM stm, Sint32 *bufsize, Sint32 *rdsize);
SJ ADXSTM_GetSj(ADXSTM stm);
void ADXSTM_EntryErrFunc(void);
void ADXSTM_GetCvdfsStat(ADXSTM stm, Sint32 *stat);
Sint32 ADXSTM_GetFad(void *unused, Sint32 *fad);
void ADXSTMF_SetupHandleMember(ADXSTM stm, struct cvfs_file *file, Sint32 ofst,
                               Sint32 fsize, SJ sj);
ADXSTM ADXSTMF_CreateCvfsRt(struct cvfs_file *file, Sint32 ofst, Sint32 fsize,
                            SJ sj);
ADXSTM ADXSTMF_CreateCvfs(struct cvfs_file *file, Sint32 ofst, Sint32 fsize,
                          SJ sj);
void ADXSTMF_Destroy(ADXSTM stm);
void adxstm_sj_internal_error(void);
void adxstmf_stat_exec(ADXSTM stm);
void ADXSTMF_ExecHndl(ADXSTM stm);

/* adxstmf_obj holds the library's readers in two pools. the real-time pool
 * is the adxstmf_rtim_num readers from index adxstmf_rtim_ofst. the normal
 * pool is the adxstmf_nrml_num readers from index adxstmf_nrml_ofst. pools
 * whose counts add past ADXSTM_OBJ_MAX share readers */
Sint32 adxstmf_rtim_ofst = 0;
Sint32 adxstmf_rtim_num = DEF_RTIM_NUM;
Sint32 adxstmf_nrml_ofst = DEF_RTIM_NUM;
Sint32 adxstmf_nrml_num = ADXSTM_OBJ_MAX - DEF_RTIM_NUM;
/* the times adxstmf_stat_exec found a reader's sj, or sj->vtbl, null as it
 * came to start a read */
Sint32 adxstm_sj_internal_error_cnt = 0;

ADXSTM_OBJ adxstmf_obj[ADXSTM_OBJ_MAX];
/* NOLINTEND(misc-use-internal-linkage) */

/* the real-time pool becomes the first num readers. num must be from 0 to
 * ADXSTM_OBJ_MAX */
void ADXT_SetupRtimeNumStm(Sint32 num)
{
    adxstmf_rtim_num = num;
}

/* the normal pool becomes the last num readers. num must be from 0 to
 * ADXSTM_OBJ_MAX */
void ADXT_SetupNrmlNumStm(Sint32 num)
{
    adxstmf_nrml_ofst = ADXSTM_OBJ_MAX - num;
    adxstmf_nrml_num = num;
}

Sint32 ADXSTM_Init(void)
{
    char *devname;

    cvFsInit();
    /* @hack spelling: devname set inside the call, where a statement of its
     * own before the call would do */
    cvFsAddDev(devname = "GD", gdCiGetInterface, NULL);
    cvFsSetDefDev(devname);
    memset(adxstmf_obj, 0, sizeof(adxstmf_obj));
    return 1;
}

void ADXSTM_Reset(void)
{
}

void ADXSTM_Finish(void)
{
    cvFsFinish();
}

void ADXSTMF_SetupHandleMember(ADXSTM stm, struct cvfs_file *file, Sint32 ofst,
                               Sint32 fsize, SJ sj)
{
    stm->stat = ADXSTM_STAT_STOP;
    stm->reading = FALSE;
    stm->sj = sj;
    stm->file = file;
    stm->stsct = 0;
    stm->ofst = ofst;
    stm->fsize = fsize;
    stm->maxsct = DEF_REQ_RD_SCT;
    /* @hack spelling: stm->fsize, which holds fsize here, where
     * ADXSTM_SCT_COUNT(fsize) would do */
    stm->eossct = ADXSTM_SCT_COUNT(stm->fsize);
    /* @hack spelling: bufsize and rdsize set in one statement, where
     * stm->rdsize = ... and then stm->bufsize = stm->rdsize would do */
    stm->bufsize = stm->rdsize = SJ_GetNumData(sj, SJ_LIN_FREE);
    stm->used = TRUE;
}

/* set up a free reader of the real-time pool. the reader covers the fsize
 * bytes of file from its sector ofst. it returns the reader, or null where
 * the pool has none free */
ADXSTM ADXSTMF_CreateCvfsRt(struct cvfs_file *file, Sint32 ofst, Sint32 fsize,
                            SJ sj)
{
    ADXSTM stm;
    Sint32 i;

#ifdef NONMATCHING
    /* GCC can't see that stm is set wherever it is read. it is, for any pool
     * count of 0 or more. a pool of no readers leaves i at its count, and the
     * call returns null */
    stm = NULL;
#endif
    for (i = 0; i < adxstmf_rtim_num; i++) {
        stm = &adxstmf_obj[adxstmf_rtim_ofst + i];
        if (stm->used == FALSE) {
            break;
        }
    }
    if (i == adxstmf_rtim_num) {
        return NULL;
    }
    ADXSTMF_SetupHandleMember(stm, file, ofst, fsize, sj);
    return stm;
}

/* set up a reader as ADXSTMF_CreateCvfsRt does, from the normal pool */
ADXSTM ADXSTMF_CreateCvfs(struct cvfs_file *file, Sint32 ofst, Sint32 fsize,
                          SJ sj)
{
    ADXSTM stm;
    Sint32 i;

#ifdef NONMATCHING
    /* GCC can't see that stm is set wherever it is read. it is, for any pool
     * count of 0 or more. a pool of no readers leaves i at its count, and the
     * call returns null */
    stm = NULL;
#endif
    for (i = 0; i < adxstmf_nrml_num; i++) {
        stm = &adxstmf_obj[adxstmf_nrml_ofst + i];
        if (stm->used == FALSE) {
            break;
        }
    }
    if (i == adxstmf_nrml_num) {
        return NULL;
    }
    ADXSTMF_SetupHandleMember(stm, file, ofst, fsize, sj);
    return stm;
}

void ADXSTMF_Destroy(ADXSTM stm)
{
    memset(stm, 0, sizeof(ADXSTM_OBJ));
}

ADXSTM ADXSTM_OpenFileRangeEx(char *fname, void *dir, Sint32 ofst, Sint32 nsct,
                              SJ sj)
{
    struct cvfs_file *file;
    ADXSTM stm;

    if (strlen(fname) >= ADXSTM_FNAME_MAX) {
        ADXERR_CallErrFunc2(
            "E0110122 ADXSTM_OpenFnameRangEx: too long file name", fname);
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((file = cvFsOpen(fname, dir, OPEN_RW)) == NULL) {
        ADXERR_CallErrFunc2("E0041206 ADXSTM_OpenFnameRangEx: can't open ",
                            fname);
        return NULL;
    }
    /* @bug logic: leaves the file open, as adx_stmc.h says. the branch on
     * the null reader at 0x8c1568dc returns with no cvFsClose */
    stm = ADXSTMF_CreateCvfs(file, ofst, nsct * ADXSTM_SCT_SIZE, sj);
    if (stm != NULL) {
        strncpy(stm->fname, fname, ADXSTM_FNAME_MAX - 1);
        ADXSTM_Seek(stm, 0);
    }
    return stm;
}

ADXSTM ADXSTM_OpenFileRangeExRt(char *fname, void *dir, Sint32 ofst,
                                Sint32 nsct, SJ sj)
{
    struct cvfs_file *file;
    ADXSTM stm;

    if (strlen(fname) >= ADXSTM_FNAME_MAX) {
        ADXERR_CallErrFunc2(
            "E0110123 ADXSTM_OpenFnameRangExRt: too long file name", fname);
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((file = cvFsOpen(fname, dir, OPEN_RW)) == NULL) {
        ADXERR_CallErrFunc2("E00041205 ADXSTM_OpenFnameRangeExRt: can't open ",
                            fname);
        return NULL;
    }
    /* @bug logic: leaves the file open, as adx_stmc.h says. the branch on
     * the null reader at 0x8c156952 returns with no cvFsClose */
    stm = ADXSTMF_CreateCvfsRt(file, ofst, nsct * ADXSTM_SCT_SIZE, sj);
    if (stm != NULL) {
        strncpy(stm->fname, fname, ADXSTM_FNAME_MAX - 1);
        ADXSTM_Seek(stm, 0);
    }
    return stm;
}

ADXSTM ADXSTM_OpenFnameEx(char *fname, void *dir, SJ sj)
{
    struct cvfs_file *file;
    Sint32 fsize;
    ADXSTM stm;

    if (dir == NULL) {
        fsize = cvFsGetFileSize(fname);
    } else {
        fsize = cvFsGetFileSizeEx(fname, dir);
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((file = cvFsOpen(fname, dir, OPEN_RW)) == NULL) {
        ADXERR_CallErrFunc2("E00041205 ADXSTM_OpenFnameEx: can't open ", fname);
        return NULL;
    }
    /* @bug logic: leaves the file open, as adx_stmc.h says. the branch on
     * the null reader at 0x8c1569c2 returns with no cvFsClose */
    stm = ADXSTMF_CreateCvfs(file, 0, fsize, sj);
    if (stm != NULL) {
        strncpy(stm->fname, fname, ADXSTM_FNAME_MAX - 1);
        ADXSTM_Seek(stm, 0);
    }
    return stm;
}

ADXSTM ADXSTM_OpenFname(char *fname, SJ sj)
{
    struct cvfs_file *file;
    Sint32 fsize;
    ADXSTM stm;

    if (strlen(fname) >= ADXSTM_FNAME_MAX) {
        ADXERR_CallErrFunc2("E0110125 ADXSTM_OpenFname: too long file name",
                            fname);
        return NULL;
    }
    fsize = cvFsGetFileSize(fname);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((file = cvFsOpen(fname, NULL, OPEN_RW)) == NULL) {
        /* @bug logic: reports the failed open as ADXSTM_OpenFnameEx's, not
         * ADXSTM_OpenFname's. the message is loaded at 0x8c156a22 */
        ADXERR_CallErrFunc2("E00041205 ADXSTM_OpenFnameEx: can't open ", fname);
        return NULL;
    }
    /* @bug logic: leaves the file open, as adx_stmc.h says. the branch on
     * the null reader at 0x8c156a38 returns with no cvFsClose */
    stm = ADXSTMF_CreateCvfsRt(file, 0, fsize, sj);
    if (stm != NULL) {
        strncpy(stm->fname, fname, ADXSTM_FNAME_MAX - 1);
        ADXSTM_Seek(stm, 0);
    }
    return stm;
}

ADXSTM ADXSTM_OpenFileRange(char *fname, Sint32 ofst, Sint32 nsct, SJ sj)
{
    return ADXSTM_OpenFileRangeExRt(fname, NULL, ofst, nsct, sj);
}

void ADXSTM_Close(ADXSTM stm)
{
    /* @hack spelling: obj, a local for the reader, with stm itself passed to
     * ADXSTM_Stop, where stm in obj's place would do */
    ADXSTM_OBJ *obj = stm;

    if (obj != NULL) {
        ADXSTM_Stop(stm);
        if (obj->file != NULL) {
            cvFsClose(obj->file);
        }
        ADXSTMF_Destroy(obj);
    }
}

Sint32 ADXSTM_GetStat(ADXSTM stm)
{
    return stm->stat;
}

Sint32 ADXSTM_Seek(ADXSTM stm, Sint32 sct)
{
    /* @hack spelling: obj, a local for the reader, where stm in its place
     * would do */
    ADXSTM_OBJ *obj = stm;

    cvFsSeek(obj->file, obj->ofst + sct, CVFS_SEEK_SET);
    return cvFsTell(obj->file) - obj->ofst;
}

Sint32 ADXSTM_Tell(ADXSTM stm)
{
    if (stm->file != NULL) {
        return cvFsTell(stm->file) - stm->ofst;
    }
    return 0;
}

Sint32 ADXSTM_Start(ADXSTM stm)
{
    if (stm->fsize == 0) {
        stm->rdbyte = 0;
        stm->stsct = 0;
        stm->stat = ADXSTM_STAT_READEND;
    } else {
        stm->stat = ADXSTM_STAT_READING;
        stm->rdbyte = 0;
        stm->stsct = ADXSTM_Tell(stm);
        stm->nretry = 0;
    }
    return 1;
}

void ADXSTM_Stop(ADXSTM stm)
{
    ADXCRS_Lock();
    if (stm->reading == TRUE && stm->file != NULL) {
        cvFsStopTr(stm->file);
        stm->reading = FALSE;
    }
    ADXCRS_Unlock();
    stm->stat = ADXSTM_STAT_STOP;
}

void ADXSTM_EntryEosFunc(ADXSTM stm, void (*func)(void *obj), void *obj)
{
    stm->eosfunc = func;
    stm->eosobj = obj;
}

void ADXSTM_SetEos(ADXSTM stm, Sint32 eossct)
{
    if (eossct >= 0) {
        stm->eossct = eossct;
    } else {
        stm->eossct = ADXSTM_SCT_COUNT(stm->fsize);
    }
}

void adxstm_sj_internal_error(void)
{
    adxstm_sj_internal_error_cnt++;
}

/* one pass of ADXSTM_ExecServer over stm, a reader in ADXSTM_STAT_READING.
 * the pass that makes stm ADXSTM_STAT_ERROR still tries a read. no later
 * pass collects a read that starts then, since none runs on a reader in
 * ADXSTM_STAT_ERROR. reading stays TRUE until ADXSTM_Stop, and the read's
 * room stays off sj's free line */
void adxstmf_stat_exec(ADXSTM stm)
{
    SJ sj;
    Sint32 stat;
    Sint32 nbyte;
    SJCK read;
    SJCK left;
    Sint32 pos;
    Sint32 end;
    Sint32 ndata;
    Sint32 nsct;
    Sint32 eos;
    Sint32 rest;
    Sint32 req;

    sj = stm->sj;
    if (stm->reading == TRUE) {
        stat = cvFsGetStat(stm->file);
        if (stat == CVFS_STAT_COMPLETE) {
            nbyte = stm->rdsct * ADXSTM_SCT_SIZE;
            SJ_SplitChunk(&stm->ck, nbyte, &read, &left);
            SJ_PutChunk(sj, SJ_LIN_DATA, &read);
            SJ_UngetChunk(sj, SJ_LIN_FREE, &left);
            if (ADXSTM_Tell(stm) == stm->eossct && stm->eosfunc != NULL) {
                stm->eosfunc(stm->eosobj);
            }
            stm->rdbyte += nbyte;
            stm->reading = FALSE;
            /* @hack spelling: the test's two sides in locals, and end's
             * division a statement of its own, where if (ADXSTM_Tell(stm) >=
             * ADXSTM_SCT_COUNT(stm->fsize)) would do */
            pos = ADXSTM_Tell(stm);
            end = stm->fsize + ADXSTM_SCT_SIZE - 1;
            end /= ADXSTM_SCT_SIZE;
            if (pos >= end) {
                stm->stat = ADXSTM_STAT_READEND;
            }
            stm->nretry = 0;
        } else if (stat == CVFS_STAT_ERROR) {
            SJ_UngetChunk(sj, SJ_LIN_FREE, &stm->ck);
            if (stm->nretry > RETRY_MAX) {
                stm->stat = ADXSTM_STAT_ERROR;
            } else {
                stm->nretry++;
            }
            stm->reading = FALSE;
        }
    }
    if (stm->reading == FALSE) {
        if (sj == NULL || sj->vtbl == NULL) {
            adxstm_sj_internal_error();
        } else {
            /* @hack spelling: ndata, a local for the bytes the data line
             * holds, where the call in the test would do */
            ndata = SJ_GetNumData(sj, SJ_LIN_DATA);
            if (ndata < stm->bufsize) {
                SJ_GetChunk(sj, SJ_LIN_FREE, stm->rdsize, &stm->ck);
                nsct = stm->ck.len / ADXSTM_SCT_SIZE;
                /* @hack spelling: eos and pos, locals for the difference's
                 * two sides, and the subtraction a statement of its own,
                 * where rest = stm->eossct - ADXSTM_Tell(stm) would do */
                eos = stm->eossct;
                pos = ADXSTM_Tell(stm);
                rest = eos;
                rest -= pos;
                if (nsct < rest) {
                    rest = nsct;
                }
                req = stm->maxsct;
                if (rest < req) {
                    req = rest;
                }
                if (req == 0) {
                    SJ_UngetChunk(sj, SJ_LIN_FREE, &stm->ck);
                    return;
                }
                stm->rdsct = cvFsReqRd(stm->file, req, stm->ck.data);
                if (stm->rdsct <= 0) {
                    SJ_UngetChunk(sj, SJ_LIN_FREE, &stm->ck);
                } else {
                    stm->reading = TRUE;
                }
            }
        }
    }
}

void ADXSTMF_ExecHndl(ADXSTM stm)
{
    if (stm->stat == ADXSTM_STAT_READING) {
        adxstmf_stat_exec(stm);
    }
}

void ADXSTM_ExecServer(void)
{
    Sint32 i;
    ADXSTM stm;

    for (i = 0; i < ADXSTM_OBJ_MAX; i++) {
        stm = &adxstmf_obj[i];
        if (stm->used == TRUE) {
            ADXSTMF_ExecHndl(stm);
        }
    }
}

Sint32 ADXSTM_GetCurOfst(ADXSTM stm, Sint32 *ofst)
{
    /* @hack spelling: obj, a local for the reader, where stm in its place
     * would do */
    ADXSTM_OBJ *obj = stm;

    *ofst = ADXSTM_Tell(obj) - obj->stsct;
    return 1;
}

Sint32 ADXSTM_GetBufSize(ADXSTM stm, Sint32 *bufsize, Sint32 *rdsize)
{
    *bufsize = stm->bufsize;
    *rdsize = stm->rdsize;
    return 1;
}

SJ ADXSTM_GetSj(ADXSTM stm)
{
    return stm->sj;
}

Sint32 ADXSTM_SetBufSize(ADXSTM stm, Sint32 bufsize, Sint32 rdsize)
{
    stm->bufsize = bufsize;
    stm->rdsize = rdsize;
    return 1;
}

Sint32 ADXSTM_SetReqRdSize(ADXSTM stm, Sint32 nsct)
{
    stm->maxsct = nsct;
    return 1;
}

void ADXSTM_EntryErrFunc(void)
{
}

Sint32 ADXSTM_GetFileLen(ADXSTM stm)
{
    return stm->fsize;
}

void ADXSTM_GetCvdfsStat(ADXSTM stm, Sint32 *stat)
{
    /* @hack spelling: obj, a local for the reader, where stm in its place
     * would do */
    ADXSTM_OBJ *obj = stm;

    *stat = cvFsGetStat(obj->file);
}

Sint32 ADXSTM_GetFad(void *unused, Sint32 *fad)
{
    *fad = 0;
    return 1;
}

Sint32 ADXSTM_GetFsizeSct(char *fname, Sint32 *fnsct)
{
    /* @hack spelling: fsize, a local for the file's bytes, where the call in
     * its place would do */
    Sint32 fsize;

    fsize = cvFsGetFileSize(fname);
    *fnsct = ADXSTM_SCT_COUNT(fsize);
    return 1;
}

Sint32 ADXSTM_GetFsizeByte(char *fname, Sint32 *fsize)
{
    *fsize = cvFsGetFileSize(fname);
    return 1;
}
