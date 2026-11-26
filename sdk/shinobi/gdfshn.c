/* the GD file system's calls on a handle, and the wait for a handle's
 * request */
#include <gdctl.h> /* IWYU pragma: keep -- completes GD_DCF */
#include <gdfs_lif.h> /* IWYU pragma: keep -- completes GDFS_LIF */
#include <machine.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* 20 seconds, in syTmrGetCount's counts of 1.28 microseconds.
 * gdFsWaitCompleteReq gives up on a request that moves no byte for so long.
 * gdFsStopRd gives up after as long on a request whose wait value,
 * gdchn_wait, doesn't reach 1 */
#define WAIT_TOUT 15625000
/* how long gdFsStopRd waits between gdchn_wait reaching 1 and the abort: 2
 * milliseconds, in the same counts */
#define STOP_DELAY 1563
#define READ_TRIES 20000 /* the most tries gdFsRead makes at its request */
#define READ_DELAY 50000 /* the passes of gdFsRead's wait between tries */
/* the boundary, in bytes, that gdFsReqRd32 and gdFsTrans32 need buf on.
 * gdFsTrans32 needs nbytes a multiple of it too */
#define DMA_ALIGN 32

/* the member exports this function, though no header of the SDK declares
 * it. gdFsSetTmode sets the transfer mode of the handle's later gdFsReqRd32
 * requests. tmode is a GDD_FS_TMODE_ value. it returns GDD_ERR_OK, or
 * GDD_ERR_ILLHNDL for a null or a closed handle */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 gdFsSetTmode(GDFS gdfs, Sint32 tmode);
/* NOLINTEND(misc-use-internal-linkage) */

static Sint32 req_rd_cpu(GDFS gdfs, Sint32 nsct, void *buf);

Sint32 gdFsWaitCompleteReq(GDFS gdfs)
{
    Uint32 start;
    Sint32 trnsed;
    Sint32 stat;
    Uint32 elapsed;
    Uint32 now;

    trnsed = gdfs->trnsed;
    start = syTmrGetCount();
    for (;;) {
        gdfs->wk->liftbl->exec_server();
        /* @hack spelling: stat, where the call in the switch would do */
        stat = gdFsGetStat(gdfs);
        switch (stat) {
        case GDD_STAT_IDLE:
            return GDD_ERR_ILLREQ;
        case GDD_STAT_COMPLETE:
            return GDD_ERR_OK;
        case GDD_STAT_READ:
        case GDD_STAT_SEEK:
        case GDD_STAT_BUSY:
            if (trnsed != gdfs->trnsed) {
                start = syTmrGetCount();
                trnsed = gdfs->trnsed;
            }
            break;
        case GDD_STAT_ERR:
        case GDFS_STAT_FATAL:
            return gdfs->err;
        default:
            return GDD_ERR_FATAL;
        }
        /* @hack spelling: now, where syTmrGetCount() in the call would do */
        now = syTmrGetCount();
        /* @hack spelling: elapsed, where the call in the test would do */
        elapsed = syTmrDiffCount(start, now);
        if (elapsed > WAIT_TOUT) {
            return GDD_ERR_TOUT;
        }
    }
}

GDFS gdFsReopenHn(GDFS gdfs, Sint32 fid, Sint32 fad, Sint32 fsize)
{
    if (gdfs == NULL) {
        return NULL;
    }
    if (gdfs->wk->now_work == gdfs) {
        return NULL;
    }
    gdfs->fid = fid;
    gdfs->fad = fad;
    gdfs->fsize = fsize;
    /* @hack spelling: the size read back from gdfs->fsize, where fsize would
     * do */
    gdfs->fsctsize = gdFsCalcSctSize(gdfs->fsize);
    gdfs->ofs = 0;
    gdfs->trnsed = 0;
    gdfs->rsize = 0;
    gdfs->trsize = 0;
    gdfs->gdchn = 0;
    gdfs->ex_errcode = 0;
    gdfs->act = GDFS_ACT_NONE;
    gdfs->trflag = GDD_FS_TRANS_READY;
    gdfs->used = TRUE;
    gdfs->stat = GDD_STAT_IDLE;
    gdfs->err = GDD_ERR_OK;
    return gdfs;
}

GDFS_DIRREC gdFsCreateDirhn(void *dirbuf, Sint32 max_dirent)
{
    GDFS_DIRREC dirrec = dirbuf;

    dirrec->dir_num = 0;
    dirrec->max_ent = max_dirent;
    dirrec->dir_fad = 0;
    return dirrec;
}

Sint32 gdFsChangeDir(const char *dirname)
{
    return gdFsLoadDir(dirname, NULL);
}

void gdFsClose(GDFS gdfs)
{
    if (gdfs == NULL) {
        return;
    }
    if (gdfs->used == FALSE) {
        return;
    }
    if (gdfs->stat == GDD_STAT_READ || gdfs->stat == GDD_STAT_SEEK ||
        gdfs->stat == GDD_STAT_BUSY) {
        gdFsStopRd(gdfs);
    }
    gdfs->used = FALSE;
}

Bool gdFsGetFileSize(GDFS gdfs, Sint32 *fsize)
{
    if (gdfs == NULL) {
        return FALSE;
    }
    if (gdfs->used == FALSE) {
        return FALSE;
    }
    *fsize = gdfs->fsize;
    return TRUE;
}

Bool gdFsGetFileSctSize(GDFS gdfs, Sint32 *fsctsize)
{
    if (gdfs == NULL) {
        return FALSE;
    }
    if (gdfs->used == FALSE) {
        return FALSE;
    }
    *fsctsize = gdfs->fsctsize;
    return TRUE;
}

Bool gdFsGetFileFad(GDFS gdfs, Sint32 *fad)
{
    if (gdfs == NULL) {
        return FALSE;
    }
    if (gdfs->used == FALSE) {
        return FALSE;
    }
    *fad = gdfs->fad;
    return TRUE;
}

Sint32 gdFsSeek(GDFS gdfs, Sint32 sctno, Sint32 type)
{
    Sint32 fsctsize;
    Sint32 ofs;

    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    fsctsize = gdfs->fsctsize;
    switch (type) {
    case GDD_SEEK_SET:
        ofs = sctno;
        break;
    case GDD_SEEK_CUR:
        ofs = gdfs->ofs + sctno;
        break;
    case GDD_SEEK_END:
        ofs = sctno + fsctsize;
        break;
    default:
        return GDD_ERR_SEEK;
    }
    if (ofs < 0 || ofs > fsctsize) {
        return GDD_ERR_OFS;
    }
    gdfs->ofs = ofs;
    return GDD_ERR_OK;
}

Sint32 gdFsTell(GDFS gdfs)
{
    if (gdfs == NULL) {
        return 0;
    }
    if (gdfs->used == FALSE) {
        return 0;
    }
    return gdfs->ofs;
}

Sint32 gdFsCheckEof(GDFS gdfs, Bool *iseof)
{
    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->ofs >= gdfs->fsctsize) {
        *iseof = TRUE;
    } else {
        *iseof = FALSE;
    }
    return GDD_ERR_OK;
}

Sint32 gdFsSetTmode(GDFS gdfs, Sint32 tmode)
{
    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    gdfs->tmode = tmode;
    return GDD_ERR_OK;
}

Sint32 gdFsRead(GDFS gdfs, Sint32 nsct, void *buf)
{
    Bool align32;
    Sint32 tries;
    Sint32 result;
    Sint32 delay;

    /* @hack spelling: align32, where the test of buf in place would do */
    if (((Uint32)buf & (DMA_ALIGN - 1)) == 0) {
        align32 = TRUE;
    } else {
        align32 = FALSE;
    }
    for (tries = 0; tries < READ_TRIES; tries++) {
        if (align32 == TRUE) {
            result = gdFsReqRd32(gdfs, nsct, buf);
        } else {
            result = req_rd_cpu(gdfs, nsct, buf);
        }
        if (result != 0) {
            break;
        }
        if (nsct == 0) {
            break;
        }
        if (gdfs->ofs >= gdfs->fsctsize) {
            break;
        }
        for (delay = READ_DELAY; delay != 0; delay--) {
#ifdef NONMATCHING
            /* GCC deletes an empty loop. the asm keeps each of its
             * READ_DELAY passes */
            __asm__ volatile("");
#endif
        }
    }
    if (tries == READ_TRIES) {
        return GDD_ERR_TOUT;
    }
    if (result >= 0) {
        result = gdFsWaitCompleteReq(gdfs);
    }
    return result;
}

/* gdFsReqRd32 for a buf on any boundary. the CPU moves the data, whatever
 * the handle's transfer mode */
static Sint32 req_rd_cpu(GDFS gdfs, Sint32 nsct, void *buf)
{
    Sint32 fad;
    Sint32 end;
    Sint32 gdchn;
    Sint32 mask;

    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->wk->now_work != NULL) {
        return 0;
    }
    fad = gdfs->fad + gdfs->ofs;
    end = gdfs->fad + gdfs->fsctsize;
    if (fad >= end) {
        gdfs->trnsed = 0;
        gdfs->stat = GDD_STAT_COMPLETE;
        return 0;
    }
    if (fad + nsct > end) {
        nsct = end - fad;
    }
    if (nsct == 0) {
        gdfs->trnsed = 0;
        gdfs->stat = GDD_STAT_COMPLETE;
        return 0;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gdfs->wk->dcf->read(fad, nsct, buf, GDD_FS_TMODE_CPU);
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->rsize = nsct;
        gdfs->stat = GDD_STAT_READ;
        gdfs->trflag = GDD_FS_TRANS_BUSY;
        gdfs->act = GDFS_ACT_READ;
        gdfs->trnsed = 0;
        gdfs->wk->now_work = gdfs;
        gdfs->wk->dcf->exec_server();
    } else {
        gdfs->stat = GDD_STAT_COMPLETE;
    }
    set_imask(mask);
    if (gdchn != 0) {
        return nsct;
    }
    return 0;
}

Sint32 gdFsReqRd32(GDFS gdfs, Sint32 nsct, void *buf)
{
    Sint32 fad;
    Sint32 end;
    Sint32 gdchn;
    Sint32 mask;

    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->wk->now_work != NULL) {
        return 0;
    }
    if (((Uint32)buf & (DMA_ALIGN - 1)) != 0) {
        return GDD_ERR_32ALIGN;
    }
    fad = gdfs->fad + gdfs->ofs;
    end = gdfs->fad + gdfs->fsctsize;
    if (fad >= end) {
        gdfs->trnsed = 0;
        gdfs->stat = GDD_STAT_COMPLETE;
        return 0;
    }
    if (fad + nsct > end) {
        nsct = end - fad;
    }
    if (nsct == 0) {
        gdfs->trnsed = 0;
        gdfs->stat = GDD_STAT_COMPLETE;
        /* @hack spelling: nsct, which is 0 here, where 0 would do */
        return nsct;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gdfs->wk->dcf->read(fad, nsct, buf, gdfs->tmode);
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->rsize = nsct;
        gdfs->stat = GDD_STAT_READ;
        gdfs->trflag = GDD_FS_TRANS_BUSY;
        gdfs->act = GDFS_ACT_READ;
        gdfs->trnsed = 0;
        gdfs->wk->now_work = gdfs;
        gdfs->wk->dcf->exec_server();
    } else {
        gdfs->stat = GDD_STAT_COMPLETE;
    }
    set_imask(mask);
    if (gdchn != 0) {
        return nsct;
    }
    return 0;
}

Sint32 gdFsReqGdRd(GDFS gdfs, Sint32 nsct)
{
    Sint32 fad;
    Sint32 end;
    Sint32 gdchn;
    Sint32 mask;

    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->wk->now_work != NULL) {
        return 0;
    }
    fad = gdfs->fad + gdfs->ofs;
    end = gdfs->fad + gdfs->fsctsize;
    if (fad >= end) {
        gdfs->trnsed = 0;
        gdfs->stat = GDD_STAT_COMPLETE;
        return 0;
    }
    if (fad + nsct > end) {
        nsct = end - fad;
    }
    if (nsct == 0) {
        gdfs->trnsed = 0;
        gdfs->stat = GDD_STAT_COMPLETE;
        /* @hack spelling: nsct, which is 0 here, where 0 would do */
        return nsct;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gdfs->wk->dcf->req_gd_read(fad, nsct);
    if (gdchn != 0) {
        gdfs->wk->now_work = gdfs;
        gdfs->gdchn = gdchn;
        gdfs->rsize = nsct;
        gdfs->stat = GDD_STAT_READ;
        gdfs->trflag = GDD_FS_TRANS_BUSY;
        gdfs->act = GDFS_ACT_GDREAD;
        gdfs->trsize = 0;
        gdfs->trnsed = 0;
        gdfs->wk->dcf->exec_server();
    } else {
        gdfs->stat = GDD_STAT_COMPLETE;
    }
    set_imask(mask);
    if (gdchn != 0) {
        return nsct;
    }
    return 0;
}

Sint32 gdFsTrans32(GDFS gdfs, Sint32 nbytes, void *buf)
{
    Sint32 result;
    Sint32 mask;

    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    if (((Uint32)buf & (DMA_ALIGN - 1)) != 0) {
        return GDD_ERR_32ALIGN;
    }
    if ((nbytes & (DMA_ALIGN - 1)) != 0) {
        return GDD_ERR_SIZE;
    }
    if (gdfs->act != GDFS_ACT_GDREAD) {
        return GDD_ERR_NOTREAD;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    result = gdfs->wk->dcf->trans(gdfs->gdchn, buf, nbytes);
    if (result == 0) {
        gdfs->trflag = GDD_FS_TRANS_BUSY;
        gdfs->trsize = nbytes;
        gdfs->trnsed = 0;
        gdfs->wk->dcf->exec_server();
        /* @hack redundant: result set to GDD_ERR_OK, which it holds */
        result = GDD_ERR_OK;
    } else if (result == -1) {
        result = GDD_ERR_SIZEOVER;
    } else {
        result = GDD_ERR_BUSY;
    }
    set_imask(mask);
    return result;
}

Sint32 gdFsGetTransStat(GDFS gdfs)
{
    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    return gdfs->trflag;
}

Sint32 gdFsMovePickup(GDFS gdfs)
{
    Sint32 mask;
    Sint32 gdchn;
    Sint32 fad;

    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->wk->now_work != NULL) {
        return GDD_ERR_BUSY;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    /* @hack spelling: fad, where the sum in the call would do */
    fad = gdfs->fad + gdfs->ofs;
    gdchn = gdfs->wk->dcf->seek(fad);
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->stat = GDD_STAT_SEEK;
        gdfs->act = GDFS_ACT_SEEK;
        gdfs->wk->now_work = gdfs;
        gdfs->wk->dcf->exec_server();
    }
    set_imask(mask);
    if (gdchn != 0) {
        /* @hack spelling: the wait's result through gdchn, where returning the
         * call would do */
        gdchn = gdFsWaitCompleteReq(gdfs);
        return gdchn;
    }
    return GDD_ERR_BUSY;
}

Sint32 gdFsGetStat(GDFS gdfs)
{
    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    return gdfs->stat;
}

Sint32 gdFsGetNumRd(GDFS gdfs)
{
    if (gdfs == NULL) {
        return 0;
    }
    if (gdfs->used == FALSE) {
        return 0;
    }
    return gdfs->trnsed;
}

Sint32 gdFsStopRd(GDFS gdfs)
{
    Sint32 result;
    Sint32 mask;
    Uint32 start;
    Uint32 elapsed;

    if (gdfs == NULL) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->used == FALSE) {
        return GDD_ERR_ILLHNDL;
    }
    if (gdfs->stat != GDD_STAT_READ && gdfs->stat != GDD_STAT_BUSY) {
        gdfs->stat = GDD_STAT_IDLE;
        return GDD_ERR_OK;
    }
    if (gdfs->wk->now_work != gdfs) {
        return GDD_ERR_BUSY;
    }
    start = syTmrGetCount();
    while (gdfs->gdchn_wait != 1) {
        if (gdfs->stat != GDD_STAT_READ && gdfs->stat != GDD_STAT_BUSY) {
            gdfs->stat = GDD_STAT_IDLE;
            return GDD_ERR_OK;
        }
        /* @hack spelling: elapsed, where the call in the test would do */
        elapsed = syTmrDiffCount(start, syTmrGetCount());
        if (elapsed > WAIT_TOUT) {
            return GDD_ERR_TOUT;
        }
        gdfs->wk->liftbl->exec_server();
    }
    start = syTmrGetCount();
    /* @hack spelling: elapsed, in a loop that tests it after the call, where
     * a loop that tests the call would do */
    do {
        elapsed = syTmrDiffCount(start, syTmrGetCount());
    } while (elapsed < STOP_DELAY);
    mask = get_imask();
    set_imask(IMASK_ALL);
    /* the vertical blank's handler runs the server, which can end the request
     * during the waits above. the state and now_work are tested again with
     * every interrupt masked */
    /* @hack spelling: a loop of one pass, whose breaks share the restore of
     * the mask after it, where a restore before each return would do */
    do {
        if (gdfs->stat != GDD_STAT_READ && gdfs->stat != GDD_STAT_BUSY) {
            gdfs->stat = GDD_STAT_IDLE;
            result = GDD_ERR_OK;
            break;
        }
        if (gdfs->wk->now_work != gdfs) {
            result = GDD_ERR_BUSY;
            break;
        }
        gdfs->wk->dcf->read_abort(gdfs->gdchn);
        gdfs->act = GDFS_ACT_STOP;
        set_imask(mask);
        /* @hack spelling: the wait's result through result, where returning the
         * call would do */
        result = gdFsWaitCompleteReq(gdfs);
        return result;
    } while (FALSE);
    set_imask(mask);
    return result;
}

Sint32 gdFsGetErrStat(GDFS gdfs)
{
    if (gdfs == NULL) {
        return 0;
    }
    if (gdfs->used == FALSE) {
        return 0;
    }
    return gdfs->err;
}

void gdFsEntryRdEndFunc(GDFS gdfs, GDFS_FUNC func, void *obj)
{
    Sint32 mask;

    if (gdfs == NULL) {
        return;
    }
    if (gdfs->used == FALSE) {
        return;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdfs->rdendcb = func;
    gdfs->rdcb_1st = obj;
    set_imask(mask);
}

void gdFsEntryTrEndFunc(GDFS gdfs, GDFS_FUNC func, void *obj)
{
    Sint32 mask;

    if (gdfs == NULL) {
        return;
    }
    if (gdfs->used == FALSE) {
        return;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdfs->trendcb = func;
    gdfs->trcb_1st = obj;
    set_imask(mask);
}

void gdFsEntryErrFunc(GDFS gdfs, GDFS_ERRFUNC erfunc, void *obj)
{
    Sint32 mask;

    if (gdfs == NULL) {
        return;
    }
    if (gdfs->used == FALSE) {
        return;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdfs->errcb = erfunc;
    gdfs->errcb_1st = obj;
    set_imask(mask);
}
