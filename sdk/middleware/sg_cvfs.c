/* cvfs, CRI's file layer */
#include <gdci.h>
#include <sg_cvfs.h>
#include <sg_xpt.h>
#include <string.h>

/* cvfs_split_name copies no more of a name than this many bytes */
#define NAME_SCAN_MAX 297

/* an open file. the entry is free while handle is null */
struct cvfs_file {
    struct gdci_if *devif; /* the table of the file's device */
    void *handle; /* the device's own handle on the file */
};
/* a device. the entry is free while name is empty. a freed entry keeps its
 * table */
struct cvfs_dev {
    struct gdci_if *devif;
    char name[CVFS_DEVNAME_MAX];
};

static struct gdci_if *cvfs_enter_dev(char *devname, void *(*getdevif)(void));
static struct gdci_if *cvfs_find_dev(const char *devname);
static void cvfs_to_upper(char *str);
static Bool cvfs_has_dev(const char *devname, Sint32 len);
static struct cvfs_file *cvfs_find_free(void);
static void cvfs_free(struct cvfs_file *file);
static void cvfs_split_name(char *devname, char *path, const char *fname);
static void cvfs_copy_def_dev(char *devname);
static Sint32 cvfs_num_files_all(void);
static Sint32 cvfs_num_files_dev(const char *devname);
static Sint32 cvfs_load_dir_info_all(struct gdci_dirinfo *info, Sint32 max_ent);
static Sint32 cvfs_load_dir_info_dev(const char *devname,
                                     struct gdci_dirinfo *info, Sint32 max_ent);
static void cvfs_set_dev_name(struct gdci_dirinfo *info, Sint32 first,
                              Sint32 count, const char *devname);

/* the member exports these functions, cvfs_build and cvfs_init_cnt, though
 * no header of the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* pass msg and handle to the error function and its obj, as
 * cvFsEntryErrFunc set them. with no function set, it does nothing. the obj
 * argument is ignored. cvFsAddDev gives this function to each device's
 * EntryErrFunc */
void cvFsCallUsrErrFn(void *obj, char *msg, void *handle);
/* report msg as an error about no file, with a null handle */
void cvFsError(char *msg);
/* free the first device whose name starts with devname. devname isn't put
 * in capitals, so one with a small letter frees nothing and isn't reported.
 * the device's files stay open. the freed entry keeps the device's table.
 * cvFsExecServer still calls through it, and so do cvFsGetNumFiles and
 * cvFsLoadDirInfo given a null devname. a null devname is reported */
void cvFsDelDev(const char *devname);
/* the default device's name, which is empty while there is none */
char *cvFsGetDefDev(void);
/* call the ExecServer of each entry's table that has one, a freed device's
 * included */
void cvFsExecServer(void);
/* the name of the first entry whose table is the file's, which is empty for
 * a freed device. where no entry holds that table, the result points past the
 * device table. a null file is reported, and returns null */
char *cvFsGetDevName(struct cvfs_file *file);
/* have func, with obj, take each error reported. a null func clears obj
 * too, and errors then go nowhere */
void cvFsEntryErrFunc(GDCI_ERRFUNC func, void *obj);

/* each of these makes the call of its name in the file's device, such as
 * ReqWr for cvFsReqWr. it passes the device's own handle on the file, and its
 * other arguments as they are. the file must be open: closing it nulls the
 * table each call reads. a null file is reported, and so is a device without
 * the call. each but cvFsSetSctLen returns the call's result, or 0 after an
 * error. cvFsReqWr, for a device without ReqWr, returns what the report left
 * in R4 */
Sint32 cvFsReqWr(struct cvfs_file *file, Sint32 nsct, void *buf);
Sint32 cvFsGetSctLen(struct cvfs_file *file);
void cvFsSetSctLen(struct cvfs_file *file, Sint32 len);
Sint32 cvFsGetNumTr(struct cvfs_file *file);
Sint32 cvFsGetMaxByteRate(struct cvfs_file *file);
Sint32 cvFsOptFn1(struct cvfs_file *file);
Sint32 cvFsOptFn2(struct cvfs_file *file);

/* each of these makes the call of its name, such as ChangeDir for
 * cvFsChangeDir. the device is the one dirname or fname names, and the call
 * gets the name's path. each returns the call's result. a name it can't use
 * is reported, and so is a device without the call. after either error, each
 * returns -1, but cvFsIsExistFile returns FALSE. cvFsIsExistFile's fname
 * must not be null: it alone doesn't test for one */
Sint32 cvFsChangeDir(const char *dirname);
Sint32 cvFsIsExistFile(const char *fname);
Sint32 cvFsMakeDir(const char *dirname);
Sint32 cvFsRemoveDir(const char *dirname);
Sint32 cvFsDeleteFile(const char *fname);

/* what the device devname's GetFreeSize returns. a null or empty devname
 * stands for the default device. devname must be shorter than CVFS_FNAME_MAX
 * bytes, and isn't put in capitals. each device whose name starts with
 * devname and has the call is asked. the last answer is the result.
 *
 * where no device's name starts with devname, or none that does has the
 * call, nothing is reported. the result is then what the caller left in R8.
 * no device's name starts with a devname that has a small letter.
 *
 * an entry whose name starts with devname may hold no table. the call then
 * returns 0, with the error reported. so it does for a null or empty devname
 * with no default device */
Sint32 cvFsGetFreeSize(const char *devname);
/* what the device devname's GetNumFiles returns. the first device whose
 * name starts with devname and has the call answers. devname isn't put in
 * capitals, so one with a small letter gets no answer. a null devname sums
 * the answers of each entry's table, a freed device's included. with no
 * answer the result is 0, and a result of 0 is reported as an error */
Sint32 cvFsGetNumFiles(const char *devname);
/* have the device devname's LoadDirInfo fill up to max_ent entries of info,
 * and return how many it filled. the first device whose name starts with
 * devname and has the call is asked. each entry it fills gets the device's
 * name. devname isn't put in capitals, so one with a small letter fills
 * nothing.
 *
 * a null devname asks each entry's table in turn, a freed device's included.
 * the result is the sum of their counts. each table is given max_ent less the
 * entries counted before it. the calls stop once the counts reach max_ent.
 * each table fills info from its start, so a later device's files overwrite
 * an earlier one's. n is the count before a device. the device's name goes
 * into info[n] and each entry after it, to the last of max_ent. so an
 * overwritten entry keeps the earlier device's name.
 *
 * a null info and a negative max_ent are reported, and return 0. a max_ent of
 * 0 returns 0. a result of 0 is reported as an error */
Sint32 cvFsLoadDirInfo(const char *devname, struct gdci_dirinfo *info,
                       Sint32 max_ent);

const char *cvfs_build = "\ncvFs Ver.1.03 Build:Jul 06 2000 22:37:41\n";
/* the error function, or null, and the obj it is passed, as
 * cvFsEntryErrFunc set them */
static GDCI_ERRFUNC cvfs_err_func = NULL;
static void *cvfs_err_obj = NULL;
/* the cvFsInit calls that no cvFsFinish has answered */
Sint32 cvfs_init_cnt = 0;
/* NOLINTEND(misc-use-internal-linkage) */

static struct cvfs_file cvfs_file_tbl[CVFS_FILE_NUM];
static struct cvfs_dev cvfs_dev_tbl[CVFS_DEV_NUM];
static char cvfs_def_dev[CVFS_DEVNAME_MAX];

void cvFsCallUsrErrFn(void *obj, char *msg, void *handle)
{
    if (cvfs_err_func != NULL) {
        cvfs_err_func(cvfs_err_obj, msg, handle);
    }
}

void cvFsError(char *msg)
{
    cvFsCallUsrErrFn(&cvfs_err_obj, msg, NULL);
    /* @hack redundant: the return that ends the function */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void cvFsInit(void)
{
    Sint32 i;

    if (cvfs_init_cnt == 0) {
        for (i = 0; i < CVFS_FILE_NUM; i++) {
            cvfs_file_tbl[i].devif = NULL;
            cvfs_file_tbl[i].handle = NULL;
        }
        for (i = 0; i < CVFS_DEV_NUM; i++) {
            cvfs_dev_tbl[i].name[0] = '\0';
        }
        memset(cvfs_def_dev, 0, sizeof(cvfs_def_dev));
        /* @hack redundant: a store of the name's first byte, which the
         * memset has cleared */
        cvfs_def_dev[0] = '\0';
    }
    cvfs_init_cnt++;
}

void cvFsFinish(void)
{
    Sint32 i;

    cvfs_init_cnt--;
    if (cvfs_init_cnt == 0) {
        for (i = 0; i < CVFS_FILE_NUM; i++) {
            if (cvfs_file_tbl[i].devif != NULL) {
                /* @bug memory: calls through null where the file's device
                 * has no Close. cvFsClose tests Close first, and this loop
                 * doesn't (JSR @R2 at 0x8c159066) */
                cvfs_file_tbl[i].devif->Close(cvfs_file_tbl[i].handle);
            }
            cvfs_file_tbl[i].devif = NULL;
            cvfs_file_tbl[i].handle = NULL;
        }
        for (i = 0; i < CVFS_DEV_NUM; i++) {
            cvfs_dev_tbl[i].name[0] = '\0';
        }
        memset(cvfs_def_dev, 0, sizeof(cvfs_def_dev));
        /* @hack redundant: a store of the name's first byte, which the
         * memset has cleared */
        cvfs_def_dev[0] = '\0';
    }
}

void cvFsAddDev(char *devname, void *(*getdevif)(void), void *unused)
{
    struct gdci_if *devif;

    if (devname == NULL) {
        cvFsError("cvFsAddDev #1:illegal device name");
        return;
    }
    if (getdevif == NULL) {
        cvFsError("cvFsAddDev #2:illegal I/F func name");
        return;
    }
    devif = cvfs_enter_dev(devname, getdevif);
    if (devif->EntryErrFunc != NULL) {
        devif->EntryErrFunc(cvFsCallUsrErrFn, NULL);
    }
}

/* put devname in capitals, and return the table getdevif returns. the
 * table goes under devname in the first free entry. it doesn't where a
 * device's name already starts with devname, or where no entry is free */
static struct gdci_if *cvfs_enter_dev(char *devname, void *(*getdevif)(void))
{
    struct gdci_if *devif;
    Sint32 i;

    cvfs_to_upper(devname);
    devif = getdevif();
    if (cvfs_find_dev(devname) == NULL) {
        /* @hack spelling: a break at the first free entry and a test of i
         * after the loop, where the entry's stores inside the loop would do */
        for (i = 0; i < CVFS_DEV_NUM; i++) {
            if (cvfs_dev_tbl[i].name[0] == '\0') {
                break;
            }
        }
        if (i != CVFS_DEV_NUM) {
            cvfs_dev_tbl[i].devif = devif;
            memcpy(cvfs_dev_tbl[i].name, devname, strlen(devname) + 1);
        }
    }
    return devif;
}

/* the table of the first device whose name starts with devname, or null */
static struct gdci_if *cvfs_find_dev(const char *devname)
{
    Sint32 len;
    Sint32 i;

    len = strlen(devname);
    for (i = 0; i < CVFS_DEV_NUM; i++) {
        if (strncmp(devname, cvfs_dev_tbl[i].name, len) == 0) {
            return cvfs_dev_tbl[i].devif;
        }
    }
    return NULL;
}

static void cvfs_to_upper(char *str)
{
    Sint32 len;
    Sint32 i;

    len = strlen(str);
    /* @hack redundant: the pass over the terminator, where i < len would do */
    for (i = 0; i < len + 1; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 'a' - 'A';
        }
    }
}

void cvFsDelDev(const char *devname)
{
    Sint32 len;
    Sint32 i;

    if (devname == NULL) {
        cvFsError("cvFsDelDev #1:illegal device name");
        return;
    }
    len = strlen(devname);
    for (i = 0; i < CVFS_DEV_NUM; i++) {
        if (strncmp(devname, cvfs_dev_tbl[i].name, len) == 0) {
            cvfs_dev_tbl[i].name[0] = '\0';
            break;
        }
    }
}

void cvFsSetDefDev(char *devname)
{
    Sint32 len;

    if (devname == NULL) {
        cvFsError("cvFsSetDefDev #1:illegal device name");
        return;
    }
    len = strlen(devname);
    /* @hack spelling: len <= 0, where len == 0 would do for a length */
    if (len <= 0) {
        cvfs_def_dev[0] = '\0';
        return;
    }
    cvfs_to_upper(devname);
    /* @hack spelling: == TRUE, where a truth test would do */
    if (cvfs_has_dev(devname, len) == TRUE) {
        memcpy(cvfs_def_dev, devname, len + 1);
    } else {
        cvFsError("cvFsSetDefDev #2:unknown device name");
    }
}

/* whether a device's name starts with devname's first len bytes */
static Bool cvfs_has_dev(const char *devname, Sint32 len)
{
    Sint32 i;

    for (i = 0; i < CVFS_DEV_NUM; i++) {
        if (strncmp(devname, cvfs_dev_tbl[i].name, len) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

char *cvFsGetDefDev(void)
{
    return cvfs_def_dev;
}

struct cvfs_file *cvFsOpen(const char *fname, void *dir, Sint32 rw)
{
    char path[CVFS_FNAME_MAX];
    char devname[CVFS_FNAME_MAX];
    struct cvfs_file *file;

    if (fname == NULL) {
        cvFsError("cvFsOpen #1:illegal file name");
        return NULL;
    }
    cvfs_split_name(devname, path, fname);
    if (path[0] == '\0') {
        cvFsError("cvFsOpen #1:illegal file name");
        return NULL;
    }
    if (devname[0] == '\0') {
        cvfs_copy_def_dev(devname);
        if (devname[0] == '\0') {
            cvFsError("cvFsOpen #2:illegal device name");
            return NULL;
        }
    }
    file = cvfs_find_free();
    if (file == NULL) {
        cvFsError("cvFsOpen #3:failed handle alloced");
        return NULL;
    }
    file->devif = cvfs_find_dev(devname);
    if (file->devif == NULL) {
        cvfs_free(file);
        cvFsError("cvFsOpen #4:device not found");
        return NULL;
    }
    /* @hack spelling: the report in the else, where a test for a null Open
     * ahead of the call would do */
    if (file->devif->Open != NULL) {
        file->handle = file->devif->Open(path, dir, rw);
    } else {
        /* @bug logic: leaves the entry holding the table of a device without
         * an Open. the reports on either side of this one each follow a
         * cvfs_free, and this one follows none. unless a later open takes the
         * entry, cvFsFinish calls the device's Close with a null handle (BRA
         * at 0x8c159406) */
        cvFsError("cvFsOpen #5:vtbl error");
        return NULL;
    }
    if (file->handle == NULL) {
        cvfs_free(file);
        cvFsError("cvFsOpen #6:open failed");
        return NULL;
    }
    return file;
}

/* the first free file, or null */
static struct cvfs_file *cvfs_find_free(void)
{
    Sint32 i;

    /* @hack spelling: a break at the first free file and a test of i after
     * the loop, where a return inside the loop would do */
    for (i = 0; i < CVFS_FILE_NUM; i++) {
        if (cvfs_file_tbl[i].handle == NULL) {
            break;
        }
    }
    if (i == CVFS_FILE_NUM) {
        return NULL;
    }
    return &cvfs_file_tbl[i];
}

static void cvfs_free(struct cvfs_file *file)
{
    file->handle = NULL;
    file->devif = NULL;
}

#ifdef NONMATCHING
/* whether a part of fname is CVFS_FNAME_MAX bytes or longer, too long for
 * cvfs_split_name's devname or path. the device part runs to the first
 * colon, and the path part from there to the next colon or the end. a name
 * without a colon is all path */
static Bool cvfs_name_too_long(const char *fname)
{
    const char *colon = strchr(fname, ':');
    const char *end;

    if (colon == NULL) {
        return strlen(fname) >= CVFS_FNAME_MAX;
    }
    end = strchr(colon + 1, ':');
    if (end == NULL) {
        end = colon + 1 + strlen(colon + 1);
    }
    return colon - fname >= CVFS_FNAME_MAX ||
           end - (colon + 1) >= CVFS_FNAME_MAX;
}
#endif

/* split fname into devname, put in capitals, and path. devname is empty for
 * a name without a colon. a second colon ends the path */
static void cvfs_split_name(char *devname, char *path, const char *fname)
{
    Sint32 i;
    Sint32 j;

    /* @bug memory: writes past devname or path where that part of fname is
     * CVFS_FNAME_MAX bytes or longer. every caller's devname and path hold
     * CVFS_FNAME_MAX bytes. both loops stop only at NAME_SCAN_MAX bytes of
     * fname (MOV.W of the bound at 0x8c159470) */
#ifdef NONMATCHING
    /* this side repairs the overrun: a name with a part that long splits
     * into an empty devname and path. each caller refuses the empty path as
     * an illegal name */
    if (cvfs_name_too_long(fname)) {
        devname[0] = '\0';
        path[0] = '\0';
        return;
    }
#endif
    for (i = 0; i < NAME_SCAN_MAX; i++) {
        if (fname[i] == ':' || fname[i] == '\0') {
            break;
        }
        devname[i] = fname[i];
    }
    /* @hack spelling: the terminator stored in each arm, where one store
     * ahead of the test would do */
    if (fname[i] == '\0') {
        devname[i] = '\0';
        memcpy(path, devname, strlen(devname) + 1);
        devname[0] = '\0';
        return;
    }
    devname[i] = '\0';
    i++;
    for (j = i; j < NAME_SCAN_MAX; j++) {
        if (fname[j] == ':' || fname[j] == '\0') {
            break;
        }
        path[j - i] = fname[j];
    }
    path[j - i] = '\0';
    cvfs_to_upper(devname);
}

static void cvfs_copy_def_dev(char *devname)
{
    /* @hack spelling: len, taken ahead of the test, where
     * strlen(cvfs_def_dev) in the copy would do */
    Sint32 len;

    len = strlen(cvfs_def_dev);
    /* @hack redundant: the else after the return */
    if (cvfs_def_dev[0] == '\0') {
        devname[0] = '\0';
        return;
    } else {
        memcpy(devname, cvfs_def_dev, len + 1);
    }
}

void cvFsClose(struct cvfs_file *file)
{
    if (file == NULL) {
        cvFsError("cvFsClose #1:handle error");
        return;
    }
    if (file->devif->Close != NULL) {
        file->devif->Close(file->handle);
        cvfs_free(file);
    } else {
        cvFsError("cvFsClose #2:vtbl error");
    }
}

Sint32 cvFsTell(struct cvfs_file *file)
{
    /* @hack spelling: pos, a local for Tell's result, cleared ahead of the
     * report, where a return in each arm would do */
    Sint32 pos;

    if (file == NULL) {
        cvFsError("cvFsTell #1:handle error");
        return 0;
    }
    if (file->devif->Tell != NULL) {
        pos = file->devif->Tell(file->handle);
    } else {
        pos = 0;
        cvFsError("cvFsTell #2:vtbl error");
    }
    return pos;
}

Sint32 cvFsSeek(struct cvfs_file *file, Sint32 sct, Sint32 origin)
{
    /* @hack spelling: pos, a local for Seek's result, cleared ahead of the
     * report, where a return in each arm would do */
    Sint32 pos;

    if (file == NULL) {
        cvFsError("cvFsSeek #1:handle error");
        return 0;
    }
    if (file->devif->Seek != NULL) {
        pos = file->devif->Seek(file->handle, sct, origin);
    } else {
        pos = 0;
        cvFsError("cvFsSeek #2:vtbl error");
    }
    return pos;
}

Sint32 cvFsReqRd(struct cvfs_file *file, Sint32 nsct, void *buf)
{
    /* @hack spelling: ret, a local for ReqRd's result, cleared ahead of the
     * report, where a return in each arm would do */
    Sint32 ret;

    if (file == NULL) {
        cvFsError("cvFsReqRd #1:handle error");
        return 0;
    }
    if (file->devif->ReqRd != NULL) {
        ret = file->devif->ReqRd(file->handle, nsct, buf);
    } else {
        ret = 0;
        cvFsError("cvFsReqRd #2:vtbl error");
    }
    return ret;
}

Sint32 cvFsReqWr(struct cvfs_file *file, Sint32 nsct, void *buf)
{
    Sint32 ret;

    if (file == NULL) {
        cvFsError("cvFsReqWr #1:handle error");
        return 0;
    }
#ifdef NONMATCHING
    /* this side repairs the result: it returns 0, as cvFsReqRd does for a
     * device without ReqRd. the image returns the R4 cvFsError left, which C
     * can't give */
    ret = 0;
#endif
    /* @bug uninitialized: nothing sets ret where the device has no ReqWr.
     * cvFsReqWr then returns what the report left in R4 (MOV R4,R0 at
     * 0x8c159662) */
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    if (file->devif->ReqWr != NULL) {
        ret = file->devif->ReqWr(file->handle, nsct, buf);
    } else {
        cvFsError("cvFsReqWr #2:vtbl error");
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    return ret;
}

void cvFsStopTr(struct cvfs_file *file)
{
    if (file == NULL) {
        cvFsError("cvFsStopTr #1:handle error");
        return;
    }
    if (file->devif->StopTr != NULL) {
        file->devif->StopTr(file->handle);
    } else {
        cvFsError("cvFsStopTr #2:vtbl error");
    }
}

void cvFsExecServer(void)
{
    struct gdci_if *devif;
    Sint32 i;

    for (i = 0; i < CVFS_DEV_NUM; i++) {
        devif = cvfs_dev_tbl[i].devif;
        if (devif != NULL && devif->ExecServer != NULL) {
            devif->ExecServer();
        }
    }
}

Sint32 cvFsGetStat(struct cvfs_file *file)
{
    /* @hack spelling: stat, a local for GetStat's result that starts as
     * CVFS_STAT_ERROR, where a return in each arm would do */
    Sint32 stat = CVFS_STAT_ERROR;

    if (file == NULL) {
        cvFsError("cvFsGetStat #1:handle error");
        return CVFS_STAT_ERROR;
    }
    if (file->devif->GetStat != NULL) {
        stat = file->devif->GetStat(file->handle);
    } else {
        cvFsError("cvFsGetStat #2:vtbl error");
    }
    return stat;
}

Sint32 cvFsGetFileSize(const char *fname)
{
    char devname[CVFS_FNAME_MAX];
    char path[CVFS_FNAME_MAX];
    struct gdci_if *devif;
    /* @hack spelling: size, a local for GetFileSize's result, where a return
     * in each arm would do */
    Sint32 size;

    if (fname == NULL) {
        cvFsError("cvFsGetFileSize #1:illegal file name");
        return 0;
    }
    cvfs_split_name(devname, path, fname);
    if (path[0] == '\0') {
        cvFsError("cvFsGetFileSize #1:illegal file name");
        return 0;
    }
    if (devname[0] == '\0') {
        cvfs_copy_def_dev(devname);
        if (devname[0] == '\0') {
            cvFsError("cvFsGetFileSize #2:illegal device name");
            return 0;
        }
    }
    devif = cvfs_find_dev(devname);
    if (devif == NULL) {
        cvFsError("cvFsGetFileSize #3:device not found");
        return 0;
    }
    if (devif->GetFileSize != NULL) {
        size = devif->GetFileSize(path);
    } else {
        cvFsError("cvFsGetFileSize #4:vtbl error");
        return 0;
    }
    return size;
}

Sint32 cvFsGetFileSizeEx(const char *fname, void *dir)
{
    char devname[CVFS_FNAME_MAX];
    char path[CVFS_FNAME_MAX];
    struct gdci_if *devif;
    /* @hack spelling: size, a local for GetFileSizeEx's result, where a
     * return in each arm would do */
    Sint32 size;

    /* @bug logic: each report names cvFsGetFileSize, not cvFsGetFileSizeEx
     * (MOV.L of the texts at 0x8c1597f6, 0x8c159810, 0x8c159824 and
     * 0x8c159842) */
    if (fname == NULL) {
        cvFsError("cvFsGetFileSize #1:illegal file name");
        return 0;
    }
    cvfs_split_name(devname, path, fname);
    if (path[0] == '\0') {
        cvFsError("cvFsGetFileSize #1:illegal file name");
        return 0;
    }
    if (devname[0] == '\0') {
        cvfs_copy_def_dev(devname);
        if (devname[0] == '\0') {
            cvFsError("cvFsGetFileSize #2:illegal device name");
            return 0;
        }
    }
    devif = cvfs_find_dev(devname);
    if (devif == NULL) {
        cvFsError("cvFsGetFileSize #3:device not found");
        return 0;
    }
    if (devif->GetFileSizeEx != NULL) {
        size = devif->GetFileSizeEx(path, dir);
    } else {
        cvFsError("cvFsGetFileSize #4:vtbl error");
        return 0;
    }
    return size;
}

Sint32 cvFsGetFreeSize(const char *devname)
{
    char name[CVFS_FNAME_MAX];
    struct gdci_if *devif;
    Sint32 len;
    Sint32 size;
    Sint32 i;

    if (devname == NULL || devname[0] == '\0') {
        cvfs_copy_def_dev(name);
        if (name[0] == '\0') {
            cvFsError("cvFsGetFreeSize #5:device not found");
            return 0;
        }
    } else {
        memcpy(name, devname, strlen(devname) + 1);
    }
    len = strlen(name);
    /* @hack spelling: len <= 0, where len == 0 would do for a length */
    if (len <= 0) {
        cvFsError("cvFsGetFreeSize #5:device not found");
        return 0;
    }
    /* @bug uninitialized: nothing sets size where no device's name starts
     * with name, or where none that does has a GetFreeSize. cvFsGetFreeSize
     * then returns what its caller left in R8 (MOV R8,R0 at 0x8c159914) */
#ifdef NONMATCHING
    /* this side repairs the result: it returns 0, as cvFsGetFreeSize does
     * after each error it reports. the image returns its caller's R8, which C
     * can't give */
    size = 0;
#endif
    for (i = 0; i < CVFS_DEV_NUM; i++) {
        if (strncmp(name, cvfs_dev_tbl[i].name, len) == 0) {
            devif = cvfs_dev_tbl[i].devif;
            /* @hack spelling: the report in the else, where a test for a
             * null devif ahead of the call would do */
            if (devif != NULL) {
                if (devif->GetFreeSize != NULL) {
                    size = devif->GetFreeSize();
                }
            } else {
                cvFsError("cvFsGetFreeSize #6:vtbl error");
                return 0;
            }
        }
    }
    return size;
}

Sint32 cvFsGetSctLen(struct cvfs_file *file)
{
    /* @hack spelling: len, a local for GetSctLen's result that starts as 0,
     * where a return in each arm would do */
    Sint32 len = 0;

    if (file == NULL) {
        cvFsError("cvFsGetSctLen #1:handle error");
        return 0;
    }
    if (file->devif->GetSctLen != NULL) {
        len = file->devif->GetSctLen(file->handle);
    } else {
        cvFsError("cvFsGetSctLen #2:vtbl error");
    }
    return len;
}

void cvFsSetSctLen(struct cvfs_file *file, Sint32 len)
{
    if (file == NULL) {
        cvFsError("cvFsSetSctLen #3:handle error");
        return;
    }
    if (file->devif->SetSctLen != NULL) {
        file->devif->SetSctLen(file->handle, len);
    } else {
        cvFsError("cvFsSetSctLen #4:vtbl error");
    }
}

Sint32 cvFsGetNumTr(struct cvfs_file *file)
{
    /* @hack spelling: num, a local for GetNumTr's result that starts as 0,
     * where a return in each arm would do */
    Sint32 num = 0;

    if (file == NULL) {
        cvFsError("cvFsGetNumTr #1:handle error");
        return 0;
    }
    if (file->devif->GetNumTr != NULL) {
        num = file->devif->GetNumTr(file->handle);
    } else {
        cvFsError("cvFsGetNumTr #2:vtbl error");
    }
    return num;
}

Sint32 cvFsChangeDir(const char *dirname)
{
    char devname[CVFS_FNAME_MAX];
    char path[CVFS_FNAME_MAX];
    struct gdci_if *devif;
    /* @hack spelling: ret, a local for ChangeDir's result, where a return in
     * each arm would do */
    Sint32 ret;

    if (dirname == NULL) {
        cvFsError("cvFsChangeDir #1:illegal directory name");
        return -1;
    }
    cvfs_split_name(devname, path, dirname);
    if (path[0] == '\0') {
        cvFsError("cvFsChangeDir #1:illegal directory name");
        return -1;
    }
    if (devname[0] == '\0') {
        cvfs_copy_def_dev(devname);
        if (devname[0] == '\0') {
            cvFsError("cvFsChangeDir #2:illegal device name");
            return -1;
        }
    }
    devif = cvfs_find_dev(devname);
    if (devif == NULL) {
        cvFsError("cvFsChangeDir #3:device not found");
        return -1;
    }
    if (devif->ChangeDir != NULL) {
        ret = devif->ChangeDir(path);
    } else {
        cvFsError("cvFsChangeDir #4:vtbl error");
        return -1;
    }
    return ret;
}

Sint32 cvFsIsExistFile(const char *fname)
{
    char devname[CVFS_FNAME_MAX];
    char path[CVFS_FNAME_MAX];
    struct gdci_if *devif;
    /* @hack spelling: ret, a local for IsExistFile's result, where a return
     * in each arm would do */
    Sint32 ret;

    cvfs_split_name(devname, path, fname);
    if (path[0] == '\0') {
        cvFsError("cvFsIsExistFile #1:illegal file name");
        return FALSE;
    }
    if (devname[0] == '\0') {
        cvfs_copy_def_dev(devname);
        if (devname[0] == '\0') {
            cvFsError("cvFsIsExistFile #2:illegal device name");
            return FALSE;
        }
    }
    devif = cvfs_find_dev(devname);
    if (devif == NULL) {
        cvFsError("cvFsIsExistFile #3:device not found");
        return FALSE;
    }
    if (devif->IsExistFile != NULL) {
        ret = devif->IsExistFile(path);
    } else {
        cvFsError("cvFsIsExistFile #4:vtbl error");
        return FALSE;
    }
    return ret;
}

Sint32 cvFsGetNumFiles(const char *devname)
{
    Sint32 num;

    if (devname == NULL) {
        num = cvfs_num_files_all();
    } else {
        num = cvfs_num_files_dev(devname);
    }
    if (num == 0) {
        cvFsError("cvFsGetNumFiles #1:file info error");
    }
    return num;
}

static Sint32 cvfs_num_files_all(void)
{
    struct gdci_if *devif;
    Sint32 num;
    Sint32 i;

    num = 0;
    for (i = 0; i < CVFS_DEV_NUM; i++) {
        devif = cvfs_dev_tbl[i].devif;
        if (devif != NULL && devif->GetNumFiles != NULL) {
            num += devif->GetNumFiles();
        }
    }
    return num;
}

static Sint32 cvfs_num_files_dev(const char *devname)
{
    struct gdci_if *devif;
    Sint32 len;
    /* @hack spelling: num, a local for GetNumFiles's result that starts as 0,
     * where a return in the loop and a return of 0 after it would do */
    Sint32 num;
    Sint32 i;

    num = 0;
    len = strlen(devname);
    for (i = 0; i < CVFS_DEV_NUM; i++) {
        if (strncmp(devname, cvfs_dev_tbl[i].name, len) == 0) {
            devif = cvfs_dev_tbl[i].devif;
            if (devif != NULL && devif->GetNumFiles != NULL) {
                num = devif->GetNumFiles();
                break;
            }
        }
    }
    return num;
}

Sint32 cvFsLoadDirInfo(const char *devname, struct gdci_dirinfo *info,
                       Sint32 max_ent)
{
    Sint32 num;

    if (info == NULL) {
        cvFsError("cvFsLoadDirInfo #1:file info error");
        return 0;
    }
    if (max_ent < 0) {
        cvFsError("cvFsLoadDirInfo #2:number error");
        return 0;
    }
    if (max_ent == 0) {
        return 0;
    }
    if (devname == NULL) {
        num = cvfs_load_dir_info_all(info, max_ent);
    } else {
        num = cvfs_load_dir_info_dev(devname, info, max_ent);
    }
    if (num == 0) {
        cvFsError("cvFsLoadDirInfo #3:device not found");
    }
    return num;
}

static Sint32 cvfs_load_dir_info_all(struct gdci_dirinfo *info, Sint32 max_ent)
{
    struct gdci_if *devif;
    Sint32 num;
    Sint32 n;
    Sint32 i;

    num = 0;
    for (i = 0; i < CVFS_DEV_NUM; i++) {
        devif = cvfs_dev_tbl[i].devif;
        if (devif != NULL && devif->LoadDirInfo != NULL) {
            /* @bug logic: gives every device the start of info, not the entry
             * after those filled before. a later device's files overwrite an
             * earlier one's (MOV R9,R4 at 0x8c159c8c) */
            n = devif->LoadDirInfo(info, max_ent);
            /* @bug logic: writes the device's name in every entry left, not
             * only in the n filled. cvfs_load_dir_info_dev writes it only in
             * those filled (MOV R13,R6 at 0x8c159c90) */
            cvfs_set_dev_name(info, num, max_ent, cvfs_dev_tbl[i].name);
            num += n;
            max_ent -= n;
        }
        if (max_ent <= 0) {
            break;
        }
    }
    return num;
}

static Sint32 cvfs_load_dir_info_dev(const char *devname,
                                     struct gdci_dirinfo *info, Sint32 max_ent)
{
    struct gdci_if *devif;
    Sint32 len;
    /* @hack spelling: num, cleared ahead of the loop and returned after it,
     * where a return in the loop and a return of 0 after it would do */
    Sint32 num;
    Sint32 i;

    num = 0;
    len = strlen(devname);
    for (i = 0; i < CVFS_DEV_NUM; i++) {
        if (strncmp(devname, cvfs_dev_tbl[i].name, len) == 0) {
            devif = cvfs_dev_tbl[i].devif;
            if (devif != NULL && devif->LoadDirInfo != NULL) {
                num = devif->LoadDirInfo(info, max_ent);
                cvfs_set_dev_name(info, 0, num, cvfs_dev_tbl[i].name);
                break;
            }
        }
    }
    return num;
}

/* write devname into count entries of info, from info[first] */
static void cvfs_set_dev_name(struct gdci_dirinfo *info, Sint32 first,
                              Sint32 count, const char *devname)
{
    Sint32 len;
    Sint32 i;

    len = strlen(devname);
    for (i = 0; i < count; i++) {
        memcpy(info[first + i].devname, devname, len + 1);
    }
}

Sint32 cvFsGetMaxByteRate(struct cvfs_file *file)
{
    /* @hack spelling: rate, a local for GetMaxByteRate's result, cleared
     * after the report, where a return in each arm would do */
    Sint32 rate;

    if (file == NULL) {
        cvFsError("cvFsGetMaxByteRate #1:handle error");
        return 0;
    }
    if (file->devif->GetMaxByteRate != NULL) {
        rate = file->devif->GetMaxByteRate(file->handle);
    } else {
        cvFsError("cvFsGetMaxByteRate #2:vtbl error");
        rate = 0;
    }
    return rate;
}

Sint32 cvFsMakeDir(const char *dirname)
{
    char devname[CVFS_FNAME_MAX];
    char path[CVFS_FNAME_MAX];
    struct gdci_if *devif;
    /* @hack spelling: ret, a local for MakeDir's result, where a return in
     * each arm would do */
    Sint32 ret;

    if (dirname == NULL) {
        cvFsError("cvFsMakeDir #1:illegal directory name");
        return -1;
    }
    cvfs_split_name(devname, path, dirname);
    if (path[0] == '\0') {
        cvFsError("cvFsMakeDir #1:illegal directory name");
        return -1;
    }
    if (devname[0] == '\0') {
        cvfs_copy_def_dev(devname);
        if (devname[0] == '\0') {
            cvFsError("cvFsMakeDir #2:illegal device name");
            return -1;
        }
    }
    devif = cvfs_find_dev(devname);
    if (devif == NULL) {
        cvFsError("cvFsMakeDir #3:device not found");
        return -1;
    }
    if (devif->MakeDir != NULL) {
        ret = devif->MakeDir(path);
    } else {
        cvFsError("cvFsMakeDir #4:vtbl error");
        return -1;
    }
    return ret;
}

Sint32 cvFsRemoveDir(const char *dirname)
{
    char devname[CVFS_FNAME_MAX];
    char path[CVFS_FNAME_MAX];
    struct gdci_if *devif;
    /* @hack spelling: ret, a local for RemoveDir's result, where a return in
     * each arm would do */
    Sint32 ret;

    if (dirname == NULL) {
        cvFsError("cvFsRemoveDir #1:illegal directory name");
        return -1;
    }
    cvfs_split_name(devname, path, dirname);
    if (path[0] == '\0') {
        cvFsError("cvFsRemoveDir #1:illegal directory name");
        return -1;
    }
    if (devname[0] == '\0') {
        cvfs_copy_def_dev(devname);
        if (devname[0] == '\0') {
            cvFsError("cvFsRemoveDir #2:illegal device name");
            return -1;
        }
    }
    devif = cvfs_find_dev(devname);
    if (devif == NULL) {
        cvFsError("cvFsRemoveDir #3:device not found");
        return -1;
    }
    if (devif->RemoveDir != NULL) {
        ret = devif->RemoveDir(path);
    } else {
        cvFsError("cvFsRemoveDir #4:vtbl error");
        return -1;
    }
    return ret;
}

Sint32 cvFsDeleteFile(const char *fname)
{
    char devname[CVFS_FNAME_MAX];
    char path[CVFS_FNAME_MAX];
    struct gdci_if *devif;
    /* @hack spelling: ret, a local for DeleteFile's result, where a return
     * in each arm would do */
    Sint32 ret;

    if (fname == NULL) {
        cvFsError("cvFsDeleteFile #1:illegal file name");
        return -1;
    }
    cvfs_split_name(devname, path, fname);
    if (path[0] == '\0') {
        cvFsError("cvFsDeleteFile #1:illegal file name");
        return -1;
    }
    if (devname[0] == '\0') {
        cvfs_copy_def_dev(devname);
        if (devname[0] == '\0') {
            cvFsError("cvFsDeleteFile #2:illegal device name");
            return -1;
        }
    }
    devif = cvfs_find_dev(devname);
    if (devif == NULL) {
        cvFsError("cvFsDeleteFile #3:device not found");
        return -1;
    }
    if (devif->DeleteFile != NULL) {
        ret = devif->DeleteFile(path);
    } else {
        cvFsError("cvFsDeleteFile #4:vtbl error");
        return -1;
    }
    return ret;
}

char *cvFsGetDevName(struct cvfs_file *file)
{
    Sint32 i;

    if (file == NULL) {
        /* @bug logic: reports a null file as a vtbl error. each other call
         * given a null file reports a handle error (MOV.L of the text at
         * 0x8c159fbe) */
        cvFsError("cvFsGetDevName #1:vtbl error");
        return NULL;
    }
    for (i = 0; i < CVFS_DEV_NUM; i++) {
        if (cvfs_dev_tbl[i].devif == file->devif) {
            break;
        }
    }
    /* @bug logic: returns an address past cvfs_dev_tbl where no entry holds
     * the file's table. that happens where cvFsDelDev freed the device and
     * cvFsAddDev then took its entry. in the image, the address lies inside
     * cvfs_def_dev. no test of i follows the loop (MOV R5,R0 at
     * 0x8c159fe6) */
    return cvfs_dev_tbl[i].name;
}

void cvFsEntryErrFunc(GDCI_ERRFUNC func, void *obj)
{
    if (func == NULL) {
        cvfs_err_func = NULL;
        cvfs_err_obj = NULL;
        return;
    }
    cvfs_err_func = func;
    cvfs_err_obj = obj;
}

Sint32 cvFsOptFn1(struct cvfs_file *file)
{
    /* @hack spelling: ret, a local for OptFn1's result that starts as 0,
     * where a return in each arm would do */
    Sint32 ret = 0;

    if (file == NULL) {
        cvFsError("cvFsOptFn1 #1:handle error");
        return 0;
    }
    if (file->devif->OptFn1 != NULL) {
        ret = file->devif->OptFn1(file->handle);
    } else {
        cvFsError("cvFsOptFn1 #2:vtbl error");
    }
    return ret;
}

Sint32 cvFsOptFn2(struct cvfs_file *file)
{
    /* @hack spelling: ret, a local for OptFn2's result that starts as 0,
     * where a return in each arm would do */
    Sint32 ret = 0;

    if (file == NULL) {
        cvFsError("cvFsOptFn2 #1:handle error");
        return 0;
    }
    if (file->devif->OptFn2 != NULL) {
        ret = file->devif->OptFn2(file->handle);
    } else {
        cvFsError("cvFsOptFn2 #2:vtbl error");
    }
    return ret;
}
