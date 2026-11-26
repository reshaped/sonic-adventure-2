/* GD file system initialization, server, directories and file handles, with
 * CD-DA playback and the tray state: the functions behind the library's
 * interface */
#include <app_root.h>
#include <gdctl.h>
#include <subcode_position.h>
#include <gdfs_lif.h>
#include <gdfsdir.h>
#include <machine.h>
#include <sg_chain.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* a sector's frame address is its ISO 9660 sector number plus FAD_OFS */
#define FAD_OFS 150

/* the primary volume descriptor's sector, counted from the volume's first,
 * and byte offsets in it. the standard identifier, "CD001", is PVD_ID_SIZE
 * bytes from PVD_ID. each other offset holds a little-endian 32-bit value */
#define PVD_SECTOR 16
#define PVD_ID 1
#define PVD_ID_SIZE 5
#define PVD_PATHTBL_SIZE 0x84
#define PVD_PATHTBL_SECTOR 0x8c
#define PVD_ROOT_SECTOR 0x9e
#define PVD_ROOT_SIZE 0xa6

/* a word of a table of contents holds its track's frame address in its low
 * 24 bits */
#define TOC_FAD_MASK 0xffffff

/* extended error codes that come with GDD_ERR_NOTREADY */
#define SENSE_NODISC 0x3a /* no disc */
#define SENSE_CHECKBUSY 0x104 /* the drive is still busy with its check */

/* the room for get_ver's text */
#define VER_TEXT_SIZE 32
/* the offsets in get_ver's text of the version's three digits, as in 1.10.
 * gdc_ver packs each in a byte, the first in bits 16 to 23 */
#define VER_UNITS 12
#define VER_TENTHS 14
#define VER_HUNDREDTHS 15
#define VER_UNITS_SHIFT 16
#define VER_TENTHS_SHIFT 8
/* drive version 1.01, packed as gdc_ver is. on an older drive, a play is
 * not kept for gdFsDaReleaseG. no play or pause on it asks for the head's
 * position */
#define GDC_VER_POS 0x10001

/* get_scd's request types, and the bytes each returns: a frame's whole
 * subcode, or its Q channel alone */
#define SCD_ALL 0
#define SCD_ALL_SIZE 100
#define SCD_Q 1
#define SCD_Q_SIZE 14
/* disc_frame's low 24 bits: the absolute time in frames */
#define DISC_TIME_MASK 0xffffff

/* da_mode's values, which say whether da_first and da_last are tracks or
 * frame addresses */
#define DA_BY_TRACK 1
#define DA_BY_SECTOR 2

#define VBLANK_PRI 80
#define CDDMAE_PRI 16

/* the boundary gdFsInitG needs gdfs_work on, else it returns GDD_ERR_32ALIGN */
#define WORK_ALIGN 32

/* what gdfs_mount keeps of the record at APP_ROOT_INFO. track, fad and root
 * are set only where mode is APP_ROOT_SET */
struct app_root {
    Sint16 mode;
    Sint16 track;
    /* the app root's frame address, or 0 until gdfs_mount finds it */
    Sint32 fad;
    /* the record's name up to its first space, at most APP_ROOT_NAME_SIZE
     * bytes, and a NUL */
    char root[APP_ROOT_NAME_SIZE + 1];
};

/* the member exports these names, though no header of the SDK declares
 * them. each function in gdg_fs_lif_tbl is the function that the gdFs call
 * of its name, without the G, makes. it has that call's contract */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 gdFsInitG(Sint32 max_open, void *gdfs_work, Sint32 max_dirent,
                 void *dirbuf, GDFS_LIF *lif);
Sint32 gdFsReinitG(void);
void gdFsFinishG(void);
Sint32 gdFsSetGlobalErrG(Sint32 err);
Sint32 gdFsLoadDirG(const char *dirname, GDFS_DIRREC gf_dirrec);
Sint32 gdFsLoadDirFastG(const char *dirname, GDFS_DIRREC gf_dirrec);
Sint32 gdFsSetDirG(GDFS_DIRREC gf_dirrec);
Sint32 gdFsGetDirInfoG(const char *name, GDFS_DIRINFO *dirinfo);
GDFS gdFsGetWorkHnG(void);
GDFS gdFsGetSysHnG(void);
GDFS gdFsCreateHnG(GDFS gdfs, Sint32 fid, Sint32 fad, Sint32 fsize);
GDFS gdFsOpenG(const char *fname, GDFS_DIRREC gf_dirrec);
GDFS gdFsOpenFastG(const char *fname, GDFS_DIRREC gf_dirrec);
GDFS gdFsOpenRangeG(Sint32 stsct, Sint32 nsct);
void gdFsExecServerG(void);
Sint32 gdFsGetGlobalErrG(void);
Sint32 gdFsGetTocG(Sint32 type, void *buf);
Sint32 gdFsGetDrvStatG(void);
Sint32 gdFsReqDrvStatG(void);
Sint32 gdFsDaPlayG(Sint32 st_track, Sint32 end_track, Sint32 reptime);
Sint32 gdFsDaPlaySctG(Sint32 st_sct, Sint32 end_sct, Sint32 reptime);
Sint32 gdFsDaStopG(void);
Sint32 gdFsDaPauseG(void);
Sint32 gdFsDaReleaseG(Sint32 reptime);
Sint32 gdFsDaGetInfoG(GDFS_DAINFO *dainfo);
void gdFsEntryErrFuncAllG(GDFS_ERRFUNC erfunc, void *obj);
Bool gdFsIsTrayOpenG(void);
Sint32 gdFsGetStatG(void);

const char *const gdgFsBuild = "\ngdFs Ver 1.12 Build:Sep 06 2000 13:59:24\n";

const GDFS_LIF gdg_fs_lif_tbl = {
    {NULL, NULL, NULL}, gdFsInitG,         gdFsReinitG,    gdFsFinishG,
    gdFsSetGlobalErrG,  gdFsReadWG,        gdFsLoadDirG,   gdFsLoadDirFastG,
    gdFsSetDirG,        gdFsGetDirInfoG,   gdFsGetWorkHnG, gdFsGetSysHnG,
    gdFsCreateHnG,      gdFsOpenG,         gdFsOpenFastG,  gdFsOpenRangeG,
    gdFsExecServerG,    gdFsGetGlobalErrG, gdFsGetTocG,    gdFsGetDrvStatG,
    gdFsReqDrvStatG,    gdFsDaPlayG,       gdFsDaPlaySctG, gdFsDaStopG,
    gdFsDaPauseG,       gdFsDaReleaseG,    gdFsDaGetInfoG, gdFsEntryErrFuncAllG,
    gdFsIsTrayOpenG,
};
const Sint32 gdg_fs_pvd = GD_START_FAD + PVD_SECTOR;
const Sint32 gdg_fs_def_tmode = GDD_FS_TMODE_DMA;
/* NOLINTEND(misc-use-internal-linkage) */

static const Uint8 iso_vd_id[] = "CD001";

/* the work area gdFsInitG was given. it is null before gdFsInitG and after
 * gdFsFinishG */
static GDFS_WORK *gd_wk = NULL;

/* the handlers' numbers from syChainAddHandler, 0 for none */
static SYCHAIN gdfs_vblank_chain = 0;
static SYCHAIN gdfs_cddmae_chain = 0;

static char gdc_ver_str[VER_TEXT_SIZE];
static struct app_root app_root_work;
/* null until gdfs_mount reads APP_ROOT_INFO, then &app_root_work */
static struct app_root *app_root;
/* the drive state, track and index that the last head position request
 * wrote */
static Sint32 da_stat;
static Sint32 da_track;
static Sint32 da_index;
/* the kept play: the last that gdFsDaPlayG or gdFsDaPlaySctG started.
 * gdFsDaReleaseG can start it again while gd_wk->daplayed is TRUE */
static Sint32 da_mode; /* DA_BY_TRACK or DA_BY_SECTOR */
static Sint32 da_first;
static Sint32 da_last;
static Sint32 da_repeats;
/* the head's frame address once the kept play's command completes */
static Sint32 da_play_fad;
/* the head's frame address once a pause's command completes */
static Sint32 da_stop_fad;

Sint32 gdFsGetLE4(const Uint8 *bytes)
{
    /* @hack spelling: the word through val, not returned in place */
    Sint32 val;

    val = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
    return val;
}

static Bool gdfs_is_iso_vd(const Uint8 *sct)
{
    const Uint8 *id = iso_vd_id;
    const Uint8 *field = sct + PVD_ID;
    Sint32 i;

    for (i = 0; i < PVD_ID_SIZE; i++) {
        if (*id++ != *field++) {
            return FALSE;
        }
    }
    return TRUE;
}

Sint32 gdFsSetGlobalErrG(Sint32 err)
{
    gd_wk->errstat = err;
    return TRUE;
}

/* the server's step, which gdg_fs_lif_tbl doesn't hold. it brings the
 * running request's handle, now_work, up to date with the drive. it returns
 * the request's state as get_cmd_stat gives it */
Sint32 gdFsGetStatG(void)
{
    Sint32 info[3];
    struct subcode_position pos;
    Sint32 size;
    GDFS gdfs;
    Sint32 stat;
    Sint32 gdchn;
    Sint32 result;
    Sint32 drvstat;
    Sint32 act;

    gd_wk->f_svr = TRUE;
    gdfs = gd_wk->now_work;
    /* @hack spelling: the request number through gdchn, not passed in
     * place */
    gdchn = gdfs == NULL ? 0 : gdfs->gdchn;
    stat = gd_wk->dcf->get_cmd_stat(gdchn, info);
    if (gdfs != NULL) {
        switch (stat) {
        case GDD_STAT_IDLE:
        case GDD_STAT_COMPLETE:
            act = gdfs->act;
            gd_wk->now_work = NULL;
            gdfs->stat = stat;
            gdfs->gdchn = 0;
            gdfs->act = GDFS_ACT_NONE;
            if (stat == GDD_STAT_COMPLETE) {
                if (act == GDFS_ACT_READ) {
                    gdfs->trnsed = info[REQ_TRNSED];
                    gdfs->trflag = GDD_FS_TRANS_COMPLETE;
                    if (gdfs->trnsed != gdfs->rsize * GDD_FS_SCTSIZE) {
                        gdfs->stat = GDD_STAT_ERR;
                        gdfs->err = GDD_ERR_READ;
                        gdfs->ex_errcode = 0;
                        goto error;
                    }
                    gdfs->ofs += gdfs->rsize;
                    if (gdfs->rdendcb != NULL) {
                        gdfs->rdendcb(gdfs->rdcb_1st);
                    }
                } else if (act == GDFS_ACT_STOP) {
                    gdfs->trflag = GDD_FS_TRANS_COMPLETE;
                } else if (act == GDFS_ACT_GDREAD) {
                    if (info[REQ_TRNSED] != gdfs->trsize) {
                        gdfs->stat = GDD_STAT_ERR;
                        gdfs->err = GDD_ERR_READ;
                        gdfs->ex_errcode = 0;
                        goto error;
                    }
                    gdfs->trnsed = gdfs->trsize;
                    gdfs->ofs += gdfs->rsize;
                    gdfs->trflag = GDD_FS_TRANS_COMPLETE;
                } else if (act == GDFS_ACT_SUBCODE) {
                    /* @hack spelling: gdDecSubcode's result through result,
                     * not tested in place */
                    result = gdDecSubcode((Sint8 *)gd_wk->sctbuf, &pos);
                    /* @hack redundant: the else after the goto */
                    if (result != 0) {
                        gdfs->stat = GDD_STAT_ERR;
                        gdfs->err = GDD_ERR_READ;
                        gdfs->ex_errcode = 0;
                        goto error;
                    } else {
                        ((GDFS_DAINFO *)gdfs->fad)->track = pos.track;
                        ((GDFS_DAINFO *)gdfs->fad)->min = pos.minute;
                        ((GDFS_DAINFO *)gdfs->fad)->sec = pos.second;
                        ((GDFS_DAINFO *)gdfs->fad)->frame = pos.frame;
                        ((GDFS_DAINFO *)gdfs->fad)->fad =
                            pos.disc_frame & DISC_TIME_MASK;
                    }
                } else if (act == GDFS_ACT_DAPOS) {
                    /* request the head position. its frame address goes
                     * where fid points, to da_play_fad or da_stop_fad */
                    gdchn = gd_wk->dcf->get_pos(&da_stat, &da_track,
                                                (Sint32 *)gdfs->fid, &da_index);
                    if (gdchn != 0) {
                        gdfs->gdchn = gdchn;
                        gdfs->stat = GDD_STAT_BUSY;
                        gdfs->act = GDFS_ACT_CMD;
                        gd_wk->now_work = gdfs;
                    } else {
                        gdfs->stat = GDD_STAT_ERR;
                        gdfs->err = GDD_ERR_RECOVER;
                        gdfs->ex_errcode = 0;
                        *(Sint32 *)gdfs->fid = 0;
                        goto error;
                    }
                }
            }
            break;
        case GDFS_STAT_CONTINUE:
            result = gd_wk->dcf->check_trans(gdfs->gdchn, &size);
            if (result == 0) {
                if (size > 0) {
                    gdfs->trflag = GDD_FS_TRANS_READY;
                } else {
                    gdfs->trflag = GDD_FS_TRANS_COMPLETE;
                }
                gdfs->trnsed = gdfs->trsize;
            } else if (result == 1) {
                gdfs->trnsed = size;
            }
            break;
        case GDD_STAT_BUSY:
            gdfs->gdchn_wait = info[REQ_WAIT];
            if (gdfs->act == GDFS_ACT_READ) {
                gdfs->trnsed = info[REQ_TRNSED];
            }
            break;
        case GDFS_STAT_ABORTED:
            break;
        case GDD_STAT_ERR:
        case GDFS_STAT_FATAL:
            gdfs->stat = stat;
            gdfs->err = info[REQ_ERR];
            gdfs->ex_errcode = info[REQ_EX_ERRCODE];
            gdfs->gdchn = 0;
            gd_wk->now_work = NULL;
            if (gdfs->err == GDD_ERR_NOTREADY) {
                if (gdfs->ex_errcode == SENSE_NODISC) {
                    drvstat = gdFsGetDrvStatG();
                    if (drvstat == GDD_DRVSTAT_OPEN ||
                        drvstat == GDD_DRVSTAT_BUSY) {
                        gdfs->err = GDD_ERR_TRAYOPEND;
                    }
                } else if (gdfs->ex_errcode == SENSE_CHECKBUSY) {
                    gdfs->err = GDD_ERR_CHECKBUSY;
                }
            }
        error:
            if (gdfs->err == GDD_ERR_TRAYOPEND ||
                gdfs->err == GDD_ERR_UNITATTENT) {
                gd_wk->istray = TRUE;
            }
            if (gd_wk->g_errcb != NULL) {
                gd_wk->g_errcb(gd_wk->g_errcb_1st, gdfs->err);
            }
            if (gdfs->errcb != NULL) {
                gdfs->errcb(gdfs->errcb_1st, gdfs->err);
            }
            break;
        default:
            /* any other state ends the request. the handle keeps only the
             * state's low byte */
            gdfs->stat = (Uint8)stat;
            gdfs->gdchn = 0;
            gd_wk->now_work = NULL;
            break;
        }
    }
    gd_wk->f_svr = FALSE;
    return stat;
}

Sint32 gdFsReadWG(Sint32 fad, Sint32 nsct, void *buf)
{
    GDFS gdfs;
    Sint32 result;

    gdfs = gdFsReopenHn(gd_wk->syshdl, 0, fad, nsct * GDD_FS_SCTSIZE);
    if (gdfs == NULL) {
        return GDD_ERR_BUSY;
    }
    /* @hack spelling: the read's result through result, not returned in
     * place */
    result = gdFsRead(gdfs, nsct, buf);
    return result;
}

/* the GD-ROM DMA end interrupt's handler, given the work area. it ends the DMA
 * with now_work's transfer end callback, or with none */
static void gdfs_cddmae_handler(void *work)
{
    GDFS gdfs = ((GDFS_WORK *)work)->now_work;
    GDFS_FUNC func = NULL;
    void *arg = NULL;

    if (gdfs != NULL && gdfs->trendcb != NULL) {
        func = gdfs->trendcb;
        arg = gdfs->trcb_1st;
    }
    gdGdcG1DmaEnd(func, arg);
}

/* the vertical blank's handler, given the work area. it runs the server,
 * unless the work area's f_svr says gdFsGetStatG is running */
static void gdfs_vblank_handler(void *work)
{
    if (((GDFS_WORK *)work)->f_svr != TRUE) {
        gdFsExecServerG();
    }
}

/* make gf_dirrec a root, whose ".." leads back to itself: entry 1 takes the
 * sector of entry 0. a null gf_dirrec, or one of fewer than two entries, stays
 * as it was */
static void gdfs_make_root(GDFS_DIRREC gf_dirrec)
{
    GDFS_DIRREC_ENT *ent;

    if (gf_dirrec != NULL && gf_dirrec->dir_num >= 2) {
        /* @hack spelling: the table through ent, not indexed in place */
        ent = gf_dirrec->dirrec_tbl;
        ent[1].fad = ent[0].fad;
    }
}

/* start the drive and mount its disc. curdir becomes the root directory, or
 * the app root where APP_ROOT_INFO names one. it returns GDD_ERR_OK, or the
 * GDD_ERR_ code of the step that failed. where the table of contents puts
 * GD_START_TRACK elsewhere than GD_START_FAD, the mount goes on. the call then
 * returns GDFS_ERR_TOC, whether or not the mount succeeds */
static Sint32 gdfs_mount(void)
{
    Sint32 mask;
    Sint32 err;
    Sint32 toc_err = 0;

    /* @hack spelling: a loop of one pass, whose breaks share the restore of
     * the mask after it, where gotos to that restore would do */
    do {
        Sint32 result;

        mask = get_imask();
        set_imask(IMASK_ALL);
        /* @bug memory: the handlers keep reading a failed start's work area
         * after a later start takes another. a start adds each handler only
         * while its chain number is 0 (TST R2,R2 at 0x8c10cf1e, TST R3,R3 at
         * 0x8c10cf34). the handler is passed the gd_wk of the start that
         * added it (MOV.L @R14,R7 at 0x8c10cf26 and 0x8c10cf3c). no shipped
         * path reaches it: sbInitSystem passes gdFsInit one work area on
         * every try */
        if (gdfs_vblank_chain == 0) {
            gdfs_vblank_chain =
                syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK,
                                  gdfs_vblank_handler, VBLANK_PRI, gd_wk);
        }
        if (gdfs_cddmae_chain == 0) {
            gdfs_cddmae_chain =
                syChainAddHandler(SYD_CHAIN_EVENT_IML4_CDDMAE,
                                  gdfs_cddmae_handler, CDDMAE_PRI, gd_wk);
        }
        /* @hack spelling: each step's locals in a block of its own, where
         * declarations at the function's top would do */
        {
            Sint32 gdchn;
            Sint32 info[2];
            GDFS gdfs;

            gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
            if (gdfs == NULL) {
                err = GDD_ERR_RESET;
                break;
            }
            gdchn = gd_wk->dcf->init_drive();
            if (gdchn == 0) {
                err = GDD_ERR_RESET;
                break;
            }
            gdfs->gdchn = gdchn;
            gdfs->stat = GDD_STAT_BUSY;
            gdfs->wk->now_work = gdfs;
            result = gdFsWaitCompleteReq(gdfs);
            if (gd_wk->dcf->get_drv_stat(info) == FALSE) {
                err = GDD_ERR_RESET;
                break;
            }
            if (result != GDD_ERR_OK) {
                /* @hack spelling: the state read as *info, not as
                 * info[DRV_STAT] */
                if (result == GDD_ERR_NOTREADY && *info == GDD_DRVSTAT_OPEN) {
                    err = GDD_ERR_TRAYOPEND;
                } else {
                    err = result;
                }
                break;
            }
            if (info[DRV_DISC] != DISC_GDROM) {
                err = GDD_ERR_DISC;
                break;
            }
        }
        {
            Sint32 gdchn;
            GDFS gdfs;

            gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
            if (gdfs == NULL) {
                err = GDD_ERR_RESET;
                break;
            }
            gd_wk->gdc_ver = 0;
            gdchn = gd_wk->dcf->get_ver(gdc_ver_str);
            if (gdchn != 0) {
                gdfs->gdchn = gdchn;
                gdfs->stat = GDD_STAT_BUSY;
                gdfs->wk->now_work = gdfs;
                result = gdFsWaitCompleteReq(gdfs);
                if (result == GDD_ERR_OK) {
                    gd_wk->gdc_ver |= (gdc_ver_str[VER_UNITS] - '0')
                                      << VER_UNITS_SHIFT;
                    gd_wk->gdc_ver |= (gdc_ver_str[VER_TENTHS] - '0')
                                      << VER_TENTHS_SHIFT;
                    gd_wk->gdc_ver |= gdc_ver_str[VER_HUNDREDTHS] - '0';
                }
            }
        }
        /* @bug uninitialized: tests track 3's word of the table of contents
         * in sctbuf even where gdFsGetTocG failed to read the table. nothing
         * tests the result of the BSR at 0x8c10d05a, and MOV.L @(R0,R2),R3
         * at 0x8c10d064 reads the word */
#ifdef NONMATCHING
        /* this side repairs the tag above: a table that gdFsGetTocG fails to
         * read sets toc_err, as one that puts GD_START_TRACK elsewhere than
         * GD_START_FAD does */
        if (gdFsGetTocG(GDFS_TOC_HD_AREA, gd_wk->sctbuf) != GDD_ERR_OK ||
            (gd_wk->sctbuf[GD_START_TRACK - 1] & TOC_FAD_MASK) !=
                GD_START_FAD) {
            toc_err = GDFS_ERR_TOC;
        }
#else
        gdFsGetTocG(GDFS_TOC_HD_AREA, gd_wk->sctbuf);
        if ((gd_wk->sctbuf[GD_START_TRACK - 1] & TOC_FAD_MASK) !=
            GD_START_FAD) {
            toc_err = GDFS_ERR_TOC;
        }
#endif
        result = gdFsReadWG(gdg_fs_pvd, 1, gd_wk->sctbuf);
        if (result != GDD_ERR_OK) {
            err = GDD_ERR_MOUNT;
            break;
        }
        if (!gdfs_is_iso_vd((Uint8 *)gd_wk->sctbuf)) {
            err = GDD_ERR_DISC;
            break;
        }
        /* @hack spelling: the path table's sector number, then its frame
         * address, in two steps, not one */
        gd_wk->pathtbl_fad =
            gdFsGetLE4((Uint8 *)gd_wk->sctbuf + PVD_PATHTBL_SECTOR);
        gd_wk->pathtbl_fad += FAD_OFS;
        gd_wk->pathtbl_size =
            gdFsGetLE4((Uint8 *)gd_wk->sctbuf + PVD_PATHTBL_SIZE);
        /* @bug memory: gdFsLoadDirG and gdFsDaPlayG read app_root->mode
         * through null after a start that fails before this line (MOV.W
         * @R10,R0 at 0x8c10d4a6, MOV.W @R0,R0 at 0x8c10d54e and MOV.W @R4,R0
         * at 0x8c10d976). such a start has set gd_wk (MOV.L R2,@R14 at
         * 0x8c10d276), and neither call checks app_root. no shipped path
         * reaches it: after a failed start, sbInitSystem calls gdFsInit again
         * or leaves the program */
        app_root = &app_root_work;
        app_root->mode = APP_ROOT_INFO->mode;
        if (app_root->mode == APP_ROOT_SET) {
            Sint32 i;
            char *from;
            char *to;

            app_root->track = APP_ROOT_INFO->track;
            app_root->fad = 0;
            to = app_root->root;
            from = APP_ROOT_INFO->root;
            for (i = 0; i < APP_ROOT_NAME_SIZE; i++) {
                if (*from == ' ') {
                    break;
                }
                *to++ = *from++;
            }
            *to = '\0';
        }
        {
            Sint32 fad;
            Sint32 size;

            /* @hack spelling: the root's sector number, then its frame
             * address, in two steps, not one */
            fad = gdFsGetLE4((Uint8 *)gd_wk->sctbuf + PVD_ROOT_SECTOR);
            fad += FAD_OFS;
            size = gdFsGetLE4((Uint8 *)gd_wk->sctbuf + PVD_ROOT_SIZE);
            gd_wk->curdir->dir_fad = 0;
            result =
                gdFsGetDirEnt((Uint8 *)gd_wk->sctbuf, fad, gd_wk->curdir, size);
            /* @hack redundant: the else after the break */
            if (result == GDD_ERR_DIROVER) {
                err = result;
                break;
            } else if (result != GDD_ERR_OK) {
                err = GDD_ERR_MOUNT;
                break;
            }
        }
        if (gd_wk->curdir->dir_num == 0) {
            err = GDD_ERR_DISC;
            break;
        }
        if (app_root->mode == APP_ROOT_SET) {
            GDFS_DIRINFO dirinfo;
            Sint32 fad;

            if (gdFsGetDirInfoG(app_root->root, &dirinfo) != GDD_ERR_OK) {
                err = GDD_ERR_MOUNT;
                break;
            }
            /* @hack spelling: fad and app_root->fad set in one statement,
             * where app_root->fad = dirinfo.fad and fad = dirinfo.fad would
             * do */
            fad = app_root->fad = dirinfo.fad;
            result =
                gdFsGetDirEnt((Uint8 *)gd_wk->sctbuf, fad, gd_wk->curdir, 1);
            /* @hack redundant: the else after the break */
            if (result == GDD_ERR_DIROVER) {
                err = result;
                break;
            } else if (result != GDD_ERR_OK) {
                err = GDD_ERR_MOUNT;
                break;
            }
            if (gd_wk->curdir->dir_num == 0) {
                err = GDD_ERR_DISC;
                break;
            }
            gdfs_make_root(gd_wk->curdir);
        }
        gd_wk->f_init = TRUE;
        err = GDD_ERR_OK;
    } while (FALSE);
    set_imask(mask);
    if (toc_err != 0) {
        return toc_err;
    }
    return err;
}

Sint32 gdFsInit(Sint32 max_open, void *gdfs_work, Sint32 max_dirent,
                void *dirbuf)
{
    return gdFsInitOvl(max_open, gdfs_work, max_dirent, dirbuf);
}

Sint32 gdFsInitG(Sint32 max_open, void *gdfs_work, Sint32 max_dirent,
                 void *dirbuf, GDFS_LIF *lif)
{
    GD_DCF *dcf;
    Sint32 mask;
    GDFS gdfs;
    Sint32 i;

    if (gd_wk != NULL && gd_wk->f_init == TRUE) {
        return GDD_ERR_OK;
    }
    if (((Uint32)gdfs_work & (WORK_ALIGN - 1)) != 0) {
        return GDD_ERR_32ALIGN;
    }
    dcf = gdFsGetGdDcf();
    dcf->init_system();
    mask = get_imask();
    set_imask(IMASK_ALL);
    gd_wk = gdfs_work;
    gd_wk->f_init = FALSE;
    gd_wk->f_svr = FALSE;
    gd_wk->max_open = max_open;
    gd_wk->curdir = gdFsCreateDirhn(dirbuf, max_dirent);
    gd_wk->dcf = dcf;
    gd_wk->now_work = NULL;
    gd_wk->daplayed = FALSE;
    gd_wk->g_errcb = NULL;
    gd_wk->istray = FALSE;
    gd_wk->liftbl = lif;
    for (i = 0, gdfs = gd_wk->hndlist; i < gd_wk->max_open + 1; i++, gdfs++) {
        gdfs->used = FALSE;
        gdfs->wk = gd_wk;
    }
    gd_wk->syshdl = &gd_wk->hndlist[0];
    gd_wk->hndtbl = &gd_wk->hndlist[1];
    gdFsCreateHnG(gd_wk->syshdl, 0, 0, 0);
    set_imask(mask);
    return gdfs_mount();
}

/* it returns GDD_ERR_INIT while gd_wk is null */
Sint32 gdFsReinitG(void)
{
    Sint32 mask;
    GDFS gdfs;
    Sint32 i;

    if (gd_wk == NULL) {
        return GDD_ERR_INIT;
    }
    /* @hack spelling: the step gdfs++, i++, where i++, gdfs++ would do */
    for (i = 0, gdfs = gd_wk->hndlist; i < gd_wk->max_open + 1; gdfs++, i++) {
        gdFsClose(gdfs);
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gd_wk->dcf->init_system();
    gd_wk->f_init = FALSE;
    gd_wk->f_svr = FALSE;
    gd_wk->now_work = NULL;
    gd_wk->daplayed = FALSE;
    gd_wk->istray = FALSE;
    for (i = 0, gdfs = gd_wk->hndlist; i < gd_wk->max_open + 1; i++, gdfs++) {
        gdfs->used = FALSE;
        gdfs->wk = gd_wk;
    }
    gd_wk->syshdl = &gd_wk->hndlist[0];
    gd_wk->hndtbl = &gd_wk->hndlist[1];
    gdFsCreateHnG(gd_wk->syshdl, 0, 0, 0);
    set_imask(mask);
    return gdfs_mount();
}

/* remove the interrupt handlers, close every handle, and let the work area
 * go. a start must have taken a work area: gd_wk isn't tested for null */
void gdFsFinishG(void)
{
    GDFS gdfs;
    Sint32 i;

    if (gdfs_vblank_chain != 0) {
        syChainDeleteHandler(gdfs_vblank_chain);
    }
    if (gdfs_cddmae_chain != 0) {
        syChainDeleteHandler(gdfs_cddmae_chain);
    }
    gdfs_vblank_chain = 0;
    gdfs_cddmae_chain = 0;
    /* @hack spelling: the step gdfs++, i++, where i++, gdfs++ would do */
    for (i = 0, gdfs = gd_wk->hndlist; i < gd_wk->max_open + 1; gdfs++, i++) {
        gdFsClose(gdfs);
    }
    gd_wk = NULL;
}

/* a load of the app root points its ".." entry back to itself, as the root's
 * does */
Sint32 gdFsLoadDirG(const char *dirname, GDFS_DIRREC gf_dirrec)
{
    GDFS_DIRREC_ENT *ent;
    struct app_root *app;
    Sint32 fad;
    Sint32 size;
    Sint32 sector;
    Sint32 no;
    Sint32 result;

    if (gf_dirrec == NULL) {
        gf_dirrec = gd_wk->curdir;
    }
    /* @hack spelling: app_root through app, not read in place */
    app = app_root;
    if (*dirname == '/' || *dirname == '\\') {
        if (app->mode == APP_ROOT_SET) {
            sector = gdFsSearchPath_dds((Uint8 *)gd_wk->sctbuf + PATHTBL_ROOM,
                                        gd_wk->pathtbl_fad, gd_wk->pathtbl_size,
                                        dirname, app->root);
        } else {
            sector = gdFsSearchPath((Uint8 *)gd_wk->sctbuf + PATHTBL_ROOM,
                                    gd_wk->pathtbl_fad, gd_wk->pathtbl_size,
                                    dirname);
        }
        if (sector == 0) {
            return GDD_ERR_NOTFOUND;
        }
        fad = sector + FAD_OFS;
        size = GDD_FS_SCTSIZE;
    } else {
        no = gdFsSearchName(dirname, gd_wk->curdir, FALSE);
        if (no == 0) {
            return GDD_ERR_NOTFOUND;
        }
        /* @hack spelling: the entry stepped to from the table's start, not
         * taken as &dirrec_tbl[no - 1] */
        ent = gd_wk->curdir->dirrec_tbl;
        ent += no - 1;
        if ((ent->flag & GDD_FF_DIRECTORY) == 0) {
            return GDD_ERR_NOTDIR;
        }
        fad = ent->fad + FAD_OFS;
        size = ent->fsize;
    }
    result = gdFsGetDirEnt((Uint8 *)gd_wk->sctbuf, fad, gf_dirrec, size);
    if (app_root->mode == APP_ROOT_SET && app_root->fad == fad &&
        (result == GDD_ERR_OK || result == GDD_ERR_DIROVER)) {
        gdfs_make_root(gf_dirrec);
    }
    return result;
}

Sint32 gdFsLoadDirFastG(const char *dirname, GDFS_DIRREC gf_dirrec)
{
    return gdFsLoadDirG(dirname, gf_dirrec);
}

Sint32 gdFsSetDirG(GDFS_DIRREC gf_dirrec)
{
    GDFS_DIRREC curdir = gd_wk->curdir;
    Sint32 max_ent = curdir->max_ent;
    Sint32 dir_num = gf_dirrec->dir_num;
    Sint32 num;
    Sint32 i;
    GDFS_DIRREC_ENT *src;
    GDFS_DIRREC_ENT *dst;

    num = max_ent > dir_num ? dir_num : max_ent;
    curdir->dir_fad = gf_dirrec->dir_fad;
    for (i = 0, src = gf_dirrec->dirrec_tbl, dst = curdir->dirrec_tbl; i < num;
         i++) {
        gdFsMemcpy(src++, dst++, sizeof(GDFS_DIRREC_ENT));
    }
    curdir->dir_num = i;
    if (max_ent < dir_num) {
        return GDD_ERR_DIROVER;
    }
    return GDD_ERR_OK;
}

Sint32 gdFsGetDirInfoG(const char *name, GDFS_DIRINFO *dirinfo)
{
    GDFS_DIRREC_ENT *ent;
    Sint32 no;

    no = gdFsSearchName(name, gd_wk->curdir, FALSE);
    if (no == 0) {
        return GDD_ERR_NOTFOUND;
    }
    /* @hack spelling: the entry stepped to from the table's start, not taken
     * as &dirrec_tbl[no - 1] */
    ent = gd_wk->curdir->dirrec_tbl;
    ent += no - 1;
    dirinfo->fad = ent->fad + FAD_OFS;
    dirinfo->fsize = ent->fsize;
    dirinfo->flag = ent->flag;
    return GDD_ERR_OK;
}

GDFS gdFsCreateHnG(GDFS gdfs, Sint32 fid, Sint32 fad, Sint32 fsize)
{
    if (gdfs->used != FALSE) {
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
    gdfs->rdendcb = NULL;
    gdfs->trendcb = NULL;
    gdfs->errcb = NULL;
    gdfs->gdchn = 0;
    gdfs->gdchn_wait = 0;
    gdfs->ex_errcode = 0;
    gdfs->act = GDFS_ACT_NONE;
    gdfs->trflag = GDD_FS_TRANS_READY;
    gdfs->used = TRUE;
    gdfs->tmode = gdg_fs_def_tmode;
    gdfs->stat = GDD_STAT_IDLE;
    gdfs->err = GDD_ERR_OK;
    return gdfs;
}

GDFS gdFsOpenG(const char *fname, GDFS_DIRREC gf_dirrec)
{
    GDFS gdfs;
    GDFS_DIRREC_ENT *ent;
    Sint32 no;
    Sint32 i;

    if (gf_dirrec == NULL) {
        gf_dirrec = gd_wk->curdir;
    }
    no = gdFsSearchName(fname, gf_dirrec, FALSE);
    if (no == 0) {
        gdFsSetGlobalErrG(GDD_ERR_NOTFOUND);
        return NULL;
    }
    for (i = 0, gdfs = gd_wk->hndtbl; i < gd_wk->max_open; i++, gdfs++) {
        if (gdfs->used == FALSE) {
            ent = &gf_dirrec->dirrec_tbl[no - 1];
            gdFsCreateHnG(gdfs, no, ent->fad + FAD_OFS, ent->fsize);
            gdFsSetGlobalErrG(GDD_ERR_OK);
            return gdfs;
        }
    }
    gdFsSetGlobalErrG(GDD_ERR_NOHNDL);
    return NULL;
}

GDFS gdFsOpenFastG(const char *fname, GDFS_DIRREC gf_dirrec)
{
    return gdFsOpenG(fname, gf_dirrec);
}

GDFS gdFsOpenRangeG(Sint32 stsct, Sint32 nsct)
{
    GDFS gdfs;
    Sint32 i;

    if (stsct < GD_START_FAD) {
        gdFsSetGlobalErrG(GDD_ERR_ILLREQ);
        return NULL;
    }
    for (i = 0, gdfs = gd_wk->hndtbl; i < gd_wk->max_open; i++, gdfs++) {
        if (gdfs->used == FALSE) {
            gdFsCreateHnG(gdfs, 0, stsct, nsct * GDD_FS_SCTSIZE);
            gdFsSetGlobalErrG(GDD_ERR_OK);
            return gdfs;
        }
    }
    gdFsSetGlobalErrG(GDD_ERR_NOHNDL);
    return NULL;
}

void gdFsExecServerG(void)
{
    gd_wk->dcf->exec_server();
    if (gd_wk->now_work != NULL) {
        gdFsGetStatG();
    }
}

Sint32 gdFsGetGlobalErrG(void)
{
    return gd_wk->errstat;
}

GDFS gdFsGetWorkHnG(void)
{
    return gd_wk->now_work;
}

GDFS gdFsGetSysHnG(void)
{
    return gd_wk->syshdl;
}

Sint32 gdFsGetTocG(Sint32 type, void *buf)
{
    GDFS gdfs;
    Sint32 mask;
    Sint32 gdchn;
    Sint32 result;

    gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
    if (gdfs == NULL) {
        return GDD_ERR_BUSY;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gd_wk->dcf->get_toc(type, buf);
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->stat = GDD_STAT_BUSY;
        gdfs->wk->now_work = gdfs;
    }
    set_imask(mask);
    /* @hack spelling: both results through result to one return, where a
     * return of its own for each would do */
    if (gdchn == 0) {
        result = GDD_ERR_BUSY;
    } else {
        result = gdFsWaitCompleteReq(gdfs);
    }
    return result;
}

Sint32 gdFsGetDrvStatG(void)
{
    Sint32 info[2];
    /* @hack spelling: the call's result through reported, not tested in
     * place */
    Bool reported;

    reported = gd_wk->dcf->get_drv_stat(info);
    if (reported == FALSE) {
        return GDD_DRVSTAT_CANTREAD;
    }
    return info[DRV_STAT];
}

Sint32 gdFsReqDrvStatG(void)
{
    GDFS gdfs;
    Sint32 mask;
    Sint32 gdchn;

    gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
    if (gdfs == NULL) {
        return GDD_ERR_BUSY;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gd_wk->dcf->get_scd(SCD_Q, SCD_Q_SIZE, gd_wk->sctbuf);
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->stat = GDD_STAT_BUSY;
        gdfs->act = GDFS_ACT_DRVSTAT;
        gdfs->wk->now_work = gdfs;
        gd_wk->dcf->exec_server();
    }
    set_imask(mask);
    if (gdchn == 0) {
        return GDD_ERR_BUSY;
    }
    return GDD_ERR_OK;
}

void gdFsEntryErrFuncAllG(GDFS_ERRFUNC erfunc, void *obj)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    gd_wk->g_errcb = erfunc;
    gd_wk->g_errcb_1st = obj;
    set_imask(mask);
}

Sint32 gdFsDaPlayG(Sint32 st_track, Sint32 end_track, Sint32 reptime)
{
    GDFS gdfs;
    Sint32 mask;
    Sint32 gdchn;

    if (app_root->mode == APP_ROOT_SET) {
        st_track += app_root->track - APP_TRACK_BASE;
        end_track += app_root->track - APP_TRACK_BASE;
    }
    if (st_track < GD_START_TRACK) {
        return GDD_ERR_BUSY;
    }
    gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
    if (gdfs == NULL) {
        return GDD_ERR_BUSY;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gd_wk->dcf->da_play(st_track, end_track, reptime);
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->stat = GDD_STAT_BUSY;
        if (gd_wk->gdc_ver >= GDC_VER_POS) {
            gd_wk->daplayed = TRUE;
            da_mode = DA_BY_TRACK;
            da_first = st_track;
            da_last = end_track;
            da_repeats = reptime;
            gdfs->fid = (Sint32)&da_play_fad;
            gdfs->act = GDFS_ACT_DAPOS;
        } else {
            gdfs->act = GDFS_ACT_CMD;
        }
        gdfs->wk->now_work = gdfs;
        gd_wk->dcf->exec_server();
    }
    set_imask(mask);
    if (gdchn == 0) {
        return GDD_ERR_BUSY;
    }
    return GDD_ERR_OK;
}

Sint32 gdFsDaPlaySctG(Sint32 st_sct, Sint32 end_sct, Sint32 reptime)
{
    GDFS gdfs;
    Sint32 mask;
    Sint32 gdchn;

    if (st_sct < GD_START_FAD) {
        return GDD_ERR_BUSY;
    }
    gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
    if (gdfs == NULL) {
        return GDD_ERR_BUSY;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gd_wk->dcf->da_play_sct(st_sct, end_sct, reptime);
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->stat = GDD_STAT_BUSY;
        if (gd_wk->gdc_ver >= GDC_VER_POS) {
            gd_wk->daplayed = TRUE;
            da_mode = DA_BY_SECTOR;
            da_first = st_sct;
            da_last = end_sct;
            da_repeats = reptime;
            gdfs->fid = (Sint32)&da_play_fad;
            gdfs->act = GDFS_ACT_DAPOS;
        } else {
            gdfs->act = GDFS_ACT_CMD;
        }
        gdfs->wk->now_work = gdfs;
        gd_wk->dcf->exec_server();
    }
    set_imask(mask);
    if (gdchn == 0) {
        return GDD_ERR_BUSY;
    }
    return GDD_ERR_OK;
}

/* it pauses, as gdFsDaPauseG does: the drive's functions have no stop */
Sint32 gdFsDaStopG(void)
{
    GDFS gdfs;
    Sint32 mask;
    Sint32 gdchn;

    gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
    if (gdfs == NULL) {
        return GDD_ERR_BUSY;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gd_wk->dcf->da_pause();
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->stat = GDD_STAT_BUSY;
        if (gd_wk->daplayed && gd_wk->gdc_ver >= GDC_VER_POS) {
            gdfs->fid = (Sint32)&da_stop_fad;
            gdfs->act = GDFS_ACT_DAPOS;
        } else {
            gdfs->act = GDFS_ACT_CMD;
        }
        gdfs->wk->now_work = gdfs;
        gd_wk->dcf->exec_server();
    }
    set_imask(mask);
    if (gdchn == 0) {
        return GDD_ERR_BUSY;
    }
    return GDD_ERR_OK;
}

Sint32 gdFsDaPauseG(void)
{
    GDFS gdfs;
    Sint32 mask;
    Sint32 gdchn;

    gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
    if (gdfs == NULL) {
        return GDD_ERR_BUSY;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gd_wk->dcf->da_pause();
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->stat = GDD_STAT_BUSY;
        if (gd_wk->daplayed && gd_wk->gdc_ver >= GDC_VER_POS) {
            gdfs->fid = (Sint32)&da_stop_fad;
            gdfs->act = GDFS_ACT_DAPOS;
        } else {
            gdfs->act = GDFS_ACT_CMD;
        }
        gdfs->wk->now_work = gdfs;
        gd_wk->dcf->exec_server();
    }
    set_imask(mask);
    if (gdchn == 0) {
        return GDD_ERR_BUSY;
    }
    return GDD_ERR_OK;
}

Sint32 gdFsDaReleaseG(Sint32 reptime)
{
    GDFS gdfs;
    Sint32 mask;
    Sint32 gdchn;

    gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
    if (gdfs == NULL) {
        return GDD_ERR_BUSY;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    /* where the pause left the head elsewhere than the kept play's command
     * did, the play is forgotten. the drive's release then resumes the
     * CD-DA */
    if (gd_wk->daplayed && da_play_fad != da_stop_fad) {
        gd_wk->daplayed = FALSE;
    }
    if (gd_wk->daplayed && gd_wk->gdc_ver >= GDC_VER_POS) {
        if (da_mode == DA_BY_TRACK) {
            gdchn = gd_wk->dcf->da_play(da_first, da_last, da_repeats);
        } else {
            gdchn = gd_wk->dcf->da_play_sct(da_first, da_last, da_repeats);
        }
        if (gdchn != 0) {
            gdfs->gdchn = gdchn;
            gdfs->stat = GDD_STAT_BUSY;
            gdfs->fid = (Sint32)&da_play_fad;
            gdfs->act = GDFS_ACT_DAPOS;
            gdfs->wk->now_work = gdfs;
            gd_wk->dcf->exec_server();
        }
    } else {
        gdchn = gd_wk->dcf->da_release(reptime);
        if (gdchn != 0) {
            gdfs->gdchn = gdchn;
            gdfs->stat = GDD_STAT_BUSY;
            gdfs->act = GDFS_ACT_CMD;
            gdfs->wk->now_work = gdfs;
            gd_wk->dcf->exec_server();
        }
    }
    set_imask(mask);
    if (gdchn == 0) {
        return GDD_ERR_BUSY;
    }
    return GDD_ERR_OK;
}

Sint32 gdFsDaGetInfoG(GDFS_DAINFO *dainfo)
{
    GDFS gdfs;
    Sint32 mask;
    Sint32 gdchn;

    gdfs = gdFsReopenHn(gd_wk->syshdl, 0, 0, 0);
    if (gdfs == NULL) {
        return GDD_ERR_BUSY;
    }
    mask = get_imask();
    set_imask(IMASK_ALL);
    gdchn = gd_wk->dcf->get_scd(SCD_ALL, SCD_ALL_SIZE, gd_wk->sctbuf);
    if (gdchn != 0) {
        gdfs->gdchn = gdchn;
        gdfs->stat = GDD_STAT_BUSY;
        gdfs->act = GDFS_ACT_SUBCODE;
        gdfs->fad = (Sint32)dainfo;
        gdfs->wk->now_work = gdfs;
        gd_wk->dcf->exec_server();
    }
    set_imask(mask);
    if (gdchn == 0) {
        return GDD_ERR_BUSY;
    }
    return GDD_ERR_OK;
}

Bool gdFsIsTrayOpenG(void)
{
    Sint32 stat;

    if (gd_wk->istray == TRUE) {
        return TRUE;
    }
    stat = gdFsGetDrvStatG();
    if (stat == GDD_DRVSTAT_OPEN) {
        gd_wk->istray = TRUE;
        return TRUE;
    }
    if (stat == GDD_DRVSTAT_BUSY) {
        /* where this request fails with GDD_ERR_TRAYOPEND or
         * GDD_ERR_UNITATTENT, the server sets istray. a later call then
         * returns TRUE */
        gdFsReqDrvStatG();
    }
    return FALSE;
}
