/* the disc as a device of cvfs, CRI's file layer */
#include <gdci.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

/* the room for an error's text, with its terminator */
#define ERR_MSG_LEN 129

/* an error: its code, which nothing reads, and its message */
struct gdci_err {
    const char *code;
    const char *msg;
};
/* indexes into gdci_err_tbl */
enum {
    ERR_NOT_FOUND,
    ERR_ILL_HNDL,
    ERR_ILL_PARAM_1,
    ERR_TBL_EMPTY,
    ERR_SEEK,
    ERR_REQ_RD,
    ERR_ILL_DIR_NAME,
    ERR_CH_DIR,
    ERR_ILL_FNAME,
    ERR_GDFS,
    ERR_ILL_PARAM_2
};

static struct gdci_obj *gdci_find_free(void);
static void gdci_free(struct gdci_obj *obj);
static void gdci_call_err_func(void *handle);
static void gdci_error(const char *caller, const struct gdci_err *err,
                       Sint32 num);

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
GDFS_WORK *gdCiGetGdfsWork(void);

const char *const gdci_build = "\ngdCi Ver.1.03 Build:Jul 06 2000 22:37:42\n";
/* the error function and its obj, as gdCiEntryErrFunc entered them */
GDCI_ERRFUNC gdci_err_func = NULL;
void *gdci_err_obj = NULL;
/* the file system's work, as gdCiGetGdfsWork last found it */
GDFS_WORK *gdci_gdfs_work = NULL;
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
static struct gdci_if gdci_if = {
    gdCiExecServer, gdCiEntryErrFunc, gdCiGetFileSize, NULL,
    gdCiOpen, gdCiClose, gdCiSeek, gdCiTell,
    gdCiReqRd, NULL, gdCiStopTr, gdCiGetStat,
    gdCiGetSctLen, NULL, gdCiGetNumTr, gdCiChangeDir,
    gdCiIsExistFile, gdCiGetNumFiles, gdCiLoadDirInfo, gdCiGetMaxByteRate,
    NULL, NULL, NULL, gdCiGetFileSizeEx,
    NULL, NULL,
};
/* clang-format on */
static struct gdci_err gdci_err_tbl[] = {
    {"E00000001:", "File not Found"},
    {"E00000002:", "Gdfs Illegal Handle"},
    {"E00000003:", "Illegal Parameter1"},
    {"E00000004:", "Gdci Table Empty"},
    {"E00000005:", "Gdfs Seek Error"},
    {"E00000006:", "Gdfs Read Request Error"},
    {"E00000007:", "Illegal Directory Name"},
    {"E00000008:", "Change Directory Failed"},
    {"E00000009:", "Illegal File Name"},
    {"E00000010:", "GDFS Error"},
    {"E00000011:", "Illegal Parameter2"},
};
/* the device's files, each open or free */
static struct gdci_obj gdci_obj_tbl[GDCI_OBJ_NUM];
/* the last error's text, as gdci_error writes it */
static char gdci_err_msg[ERR_MSG_LEN];

void *gdCiGetInterface(void)
{
    return &gdci_if;
}

/* the file system's work, found by opening the current directory */
GDFS_WORK *gdCiGetGdfsWork(void)
{
    GDFS gdfs;

    gdfs = gdFsOpen(".", NULL);
    /* @bug memory: reads through a null handle while every GDFS handle is
     * open. gdFsOpen then returns null, which gdCiOpen tests for (TST R4,R4
     * at 0x8c15a21e) and this read doesn't (MOV.L @R4,R2 at 0x8c15a0d4) */
    gdci_gdfs_work = gdfs->wk;
    gdFsClose(gdfs);
    return gdci_gdfs_work;
}

void gdCiExecServer(void)
{
    struct gdci_obj *obj;
    Sint32 i;

    /* @hack redundant: a loop that tests each file's used and does nothing
     * either way */
    for (i = 0; i < GDCI_OBJ_NUM; i++) {
        obj = &gdci_obj_tbl[i];
        if (obj->used != 1) {
            continue;
        }
    }
}

void gdCiEntryErrFunc(GDCI_ERRFUNC func, void *obj)
{
    if (func != NULL) {
        gdci_err_func = func;
    }
    if (obj != NULL) {
        gdci_err_obj = obj;
    }
}

Sint32 gdCiGetFileSizeEx(const char *fname, GDFS_DIRREC dir)
{
    GDFS gdfs;
    Sint32 fsize;

    if (fname == NULL) {
        gdci_error("gdCiGetFileSizeEx ", &gdci_err_tbl[ERR_ILL_FNAME], 1);
        gdci_call_err_func(NULL);
        return 0;
    }
    gdfs = gdFsOpen(fname, dir);
    if (!gdFsGetFileSize(gdfs, &fsize)) {
        gdci_error("gdCiGetFileSizeEx ", &gdci_err_tbl[ERR_NOT_FOUND], 2);
        fsize = 0;
    }
    gdFsClose(gdfs);
    return fsize;
}

Sint32 gdCiGetFileSize(const char *fname)
{
    GDFS_DIRINFO info;
    Sint32 err;

    if (fname == NULL) {
        gdci_error("gdCiGetFileSize ", &gdci_err_tbl[ERR_ILL_FNAME], 1);
        gdci_call_err_func(NULL);
        return 0;
    }
    err = gdFsGetDirInfo(fname, &info);
    if (err != GDD_ERR_OK) {
        if (err != GDD_ERR_BUSY) {
            if (err == GDD_ERR_NOTFOUND) {
                gdci_error("gdCiGetFileSize ", &gdci_err_tbl[ERR_NOT_FOUND], 2);
            } else {
                gdci_error("gdCiGetFileSize ", &gdci_err_tbl[ERR_GDFS], 3);
            }
            gdci_call_err_func(NULL);
        }
        return 0;
    }
    return info.fsize;
}

struct gdci_obj *gdCiOpen(const char *fname, GDFS_DIRREC dir, Sint32 rw)
{
    struct gdci_obj *obj;
    GDFS gdfs;

    if (fname == NULL) {
        gdci_error("gdCiOpen ", &gdci_err_tbl[ERR_ILL_FNAME], 1);
        gdci_call_err_func(NULL);
        return NULL;
    }
    if (rw != 0) {
        gdci_error("gdCiOpen ", &gdci_err_tbl[ERR_ILL_PARAM_1], 2);
        gdci_call_err_func(NULL);
        return NULL;
    }
    obj = gdci_find_free();
    if (obj == NULL) {
        gdci_error("gdCiOpen ", &gdci_err_tbl[ERR_TBL_EMPTY], 3);
        gdci_call_err_func(NULL);
        return NULL;
    }
    gdfs = gdFsOpen(fname, dir);
    if (gdfs == NULL) {
        gdci_free(obj);
        gdci_error("gdCiOpen ", &gdci_err_tbl[ERR_NOT_FOUND], 4);
        gdci_call_err_func(NULL);
        return NULL;
    }
    obj->used = 1;
    obj->gdfs = gdfs;
    obj->stat = GDCI_STAT_IDLE;
    return obj;
}

void gdCiClose(struct gdci_obj *obj)
{
    if (obj->stat != GDCI_STAT_IDLE) {
        /* @bug mismatched-call: gdCiStopTr takes the file as its argument,
         * and this call passes none. R4 still holds gdCiClose's own obj, so
         * gdCiStopTr stops that file's read all the same (BSR at
         * 0x8c15a25a) */
        gdCiStopTr();
    }
    gdFsClose(obj->gdfs);
    gdci_free(obj);
}

Sint32 gdCiSeek(struct gdci_obj *obj, Sint32 sct, Sint32 type)
{
    /* @hack spelling: file, a copy of obj, where obj itself would do */
    struct gdci_obj *file = obj;
    Sint32 err;

    err = gdFsSeek(file->gdfs, sct, type);
    if (err != GDD_ERR_OK && err != GDD_ERR_BUSY) {
        gdci_error("gdCiSeek ", &gdci_err_tbl[ERR_SEEK], 1);
        gdci_call_err_func(file);
    }
    return gdCiTell(obj);
}

Sint32 gdCiTell(struct gdci_obj *obj)
{
    return gdFsTell(obj->gdfs);
}

Sint32 gdCiReqRd(struct gdci_obj *obj, Sint32 nsct, void *buf)
{
    Sint32 err;

    err = gdFsReqRd32(obj->gdfs, nsct, buf);
    if (err < 0) {
        if (err != GDD_ERR_BUSY) {
            gdci_error("gdCiReqRd ", &gdci_err_tbl[ERR_REQ_RD], 1);
            gdci_call_err_func(obj);
        }
        return 0;
    }
    obj->stat = GDCI_STAT_TRANS;
    return err;
}

void gdCiStopTr(struct gdci_obj *obj)
{
    /* @hack spelling: file, a copy of obj, where obj itself would do */
    struct gdci_obj *file = obj;

    gdFsStopRd(file->gdfs);
    file->stat = GDCI_STAT_IDLE;
}

Sint32 gdCiGetStat(struct gdci_obj *obj)
{
    Sint32 stat;

    stat = gdFsGetStat(obj->gdfs);
    /* @hack redundant: empty arms for GDD_STAT_READ and GDD_STAT_IDLE */
    if (stat == GDD_STAT_READ) {
    } else if (stat == GDD_STAT_COMPLETE) {
        obj->stat = GDCI_STAT_COMPLETE;
    } else if (stat == GDD_STAT_IDLE) {
    } else if (stat == GDD_STAT_ERR) {
        obj->stat = GDCI_STAT_ERR;
    }
    return obj->stat;
}

Sint32 gdCiGetSctLen(struct gdci_obj *obj)
{
    return GDD_FS_SCTSIZE;
}

Sint32 gdCiGetNumTr(struct gdci_obj *obj)
{
    return gdFsGetNumRd(obj->gdfs);
}

Sint32 gdCiChangeDir(const char *dirname)
{
    Sint32 err;

    if (dirname == NULL) {
        gdci_error("gdCiChangeDir ", &gdci_err_tbl[ERR_ILL_DIR_NAME], 1);
        gdci_call_err_func(NULL);
        return -1;
    }
    err = gdFsChangeDir(dirname);
    if (err != GDD_ERR_OK) {
        gdci_error("gdCiChangeDir ", &gdci_err_tbl[ERR_CH_DIR], 2);
        gdci_call_err_func(NULL);
        return -1;
    }
    return 0;
}

Sint32 gdCiGetNumFiles(void)
{
    GDFS_WORK *wk;
    GDFS_DIRREC dir;
    GDFS_DIRREC_ENT *ent;
    Sint32 i;
    Sint32 num;

    /* @hack spelling: wk and ent, locals for the work and for the entries,
     * where gdCiGetGdfsWork()->curdir and dir->dirrec_tbl[i] would do */
    wk = gdCiGetGdfsWork();
    dir = wk->curdir;
    ent = dir->dirrec_tbl;
    num = 0;
    for (i = 0; i < dir->dir_num; i++) {
        if ((ent[i].flag & GDD_FF_DIRECTORY) == 0) {
            num++;
        }
    }
    return num;
}

Sint32 gdCiIsExistFile(const char *fname)
{
    GDFS_DIRINFO info;
    Sint32 err;

    if (fname == NULL) {
        gdci_error("gdCiIsExistFile ", &gdci_err_tbl[ERR_ILL_FNAME], 1);
        gdci_call_err_func(NULL);
        return FALSE;
    }
    err = gdFsGetDirInfo(fname, &info);
    if (err != GDD_ERR_OK) {
        if (err != GDD_ERR_BUSY) {
            if (err == GDD_ERR_NOTFOUND) {
                gdci_error("gdCiIsExistFile ", &gdci_err_tbl[ERR_NOT_FOUND], 2);
            } else {
                gdci_error("gdCiIsExistFile ", &gdci_err_tbl[ERR_GDFS], 3);
            }
            gdci_call_err_func(NULL);
        }
        return FALSE;
    }
    return TRUE;
}

Sint32 gdCiLoadDirInfo(struct gdci_dirinfo *info, Sint32 max_ent)
{
    GDFS_WORK *wk;
    GDFS_DIRREC dir;
    GDFS_DIRREC_ENT *ent;
    Sint32 i;
    Sint32 num;

    if (info == NULL) {
        gdci_error("gdCiLoadDirInfo ", &gdci_err_tbl[ERR_ILL_PARAM_1], 1);
        gdci_call_err_func(NULL);
        return 0;
    }
    if (max_ent == 0) {
        gdci_error("gdCiLoadDirInfo ", &gdci_err_tbl[ERR_ILL_PARAM_2], 2);
        gdci_call_err_func(NULL);
        return 0;
    }
    /* @hack spelling: wk and ent, locals for the work and for the entries,
     * where gdCiGetGdfsWork()->curdir and dir->dirrec_tbl[i] would do */
    wk = gdCiGetGdfsWork();
    dir = wk->curdir;
    ent = dir->dirrec_tbl;
    num = 0;
    for (i = 0; i < dir->dir_num; i++) {
        if ((ent[i].flag & GDD_FF_DIRECTORY) == 0) {
            memset(info[num].fname, 0, GDCI_FNAME_LEN);
            memcpy(info[num].fname, ent[i].fname, strlen((char *)ent[i].fname));
            info[num].fsize = ent[i].fsize;
            num++;
            if (num >= max_ent) {
                break;
            }
        }
    }
    return num;
}

Sint32 gdCiGetMaxByteRate(struct gdci_obj *obj)
{
    return GDCI_MAX_BYTE_RATE;
}

static struct gdci_obj *gdci_find_free(void)
{
    struct gdci_obj *obj;
    Sint32 i;

    for (i = 0; i < GDCI_OBJ_NUM; i++) {
        obj = &gdci_obj_tbl[i];
        /* @hack spelling: a continue past an open file, where a return
         * under if (obj->used == 0) would do */
        if (obj->used != 0) {
            continue;
        }
        return obj;
    }
    return NULL;
}

static void gdci_free(struct gdci_obj *obj)
{
    obj->used = 0;
    obj->gdfs = NULL;
    obj->stat = GDCI_STAT_IDLE;
}

static void gdci_call_err_func(void *handle)
{
    if (gdci_err_func != NULL) {
        gdci_err_func(gdci_err_obj, gdci_err_msg, handle);
    }
}

/* write the error's text into gdci_err_msg: caller, "#", the digit num, ":"
 * and err->msg. num must be 0 to 9 */
static void gdci_error(const char *caller, const struct gdci_err *err,
                       Sint32 num)
{
    /* @hack spelling: digits, an array the function fills at each call,
     * where a static const array would do */
    const char *digits[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

    memset(gdci_err_msg, 0, sizeof(gdci_err_msg));
    strcpy(gdci_err_msg, caller);
    strcat(gdci_err_msg, "#");
    strcat(gdci_err_msg, digits[num]);
    strcat(gdci_err_msg, ":");
    strcat(gdci_err_msg, err->msg);
}
