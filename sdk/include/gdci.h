/* the disc as a device of cvfs, CRI's file layer */
#ifndef GDCI_H
#define GDCI_H
#include <sg_cvfs.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* how many files the device can have open */
#define GDCI_OBJ_NUM 8
/* a file's states: no read asked for, a read done, a read under way, and a
 * read that failed */
#define GDCI_STAT_IDLE 0
#define GDCI_STAT_COMPLETE 1
#define GDCI_STAT_TRANS 2
#define GDCI_STAT_ERR 3
/* the rate the device gives for every file, in bytes a second */
#define GDCI_MAX_BYTE_RATE ((18 * 1024 * 1024) / 10)
/* the bytes of a name gdCiLoadDirInfo clears before it copies the name in */
#define GDCI_FNAME_LEN 33

/* an open file, or a free slot of the device's files */
struct gdci_obj {
    Sint32 used; /* 1 while the file is open, and 0 while the slot is free */
    GDFS gdfs; /* the file system's handle on the file */
    Sint32 stat; /* a GDCI_STAT_ value */
};
/* a file of the current directory, as gdCiLoadDirInfo gives it */
struct gdci_dirinfo {
    /* its device's name, which cvFsLoadDirInfo writes */
    Sint8 devname[CVFS_DEVNAME_MAX];
    Sint8 fname[35];
    Sint32 fsize; /* the file's size in bytes */
};
/* what takes the device's errors: obj as gdCiEntryErrFunc was given it, the
 * error's text, and the file the error is about, or null */
typedef void (*GDCI_ERRFUNC)(void *obj, char *msg, void *handle);

/* a device's calls, in the order cvfs knows them. a device leaves null the
 * calls it doesn't have */
struct gdci_if {
    void (*ExecServer)(void);
    void (*EntryErrFunc)(GDCI_ERRFUNC func, void *obj);
    Sint32 (*GetFileSize)(const char *fname);
    Sint32 (*GetFreeSize)(void);
    struct gdci_obj *(*Open)(const char *fname, GDFS_DIRREC dir, Sint32 rw);
    void (*Close)(struct gdci_obj *obj);
    Sint32 (*Seek)(struct gdci_obj *obj, Sint32 sct, Sint32 type);
    Sint32 (*Tell)(struct gdci_obj *obj);
    Sint32 (*ReqRd)(struct gdci_obj *obj, Sint32 nsct, void *buf);
    Sint32 (*ReqWr)(struct gdci_obj *obj, Sint32 nsct, void *buf);
    void (*StopTr)(struct gdci_obj *obj);
    Sint32 (*GetStat)(struct gdci_obj *obj);
    Sint32 (*GetSctLen)(struct gdci_obj *obj);
    void (*SetSctLen)(struct gdci_obj *obj, Sint32 len);
    Sint32 (*GetNumTr)(struct gdci_obj *obj);
    Sint32 (*ChangeDir)(const char *dirname);
    Sint32 (*IsExistFile)(const char *fname);
    Sint32 (*GetNumFiles)(void);
    Sint32 (*LoadDirInfo)(struct gdci_dirinfo *info, Sint32 max_ent);
    Sint32 (*GetMaxByteRate)(struct gdci_obj *obj);
    Sint32 (*MakeDir)(const char *dirname);
    Sint32 (*RemoveDir)(const char *dirname);
    Sint32 (*DeleteFile)(const char *fname);
    Sint32 (*GetFileSizeEx)(const char *fname, GDFS_DIRREC dir);
    Sint32 (*OptFn1)(void *handle);
    Sint32 (*OptFn2)(void *handle);
};

/* the device's table of calls, a struct gdci_if */
void *gdCiGetInterface(void);
/* do nothing */
void gdCiExecServer(void);
/* have func, with obj, take the errors the calls report. a null func or a
 * null obj keeps the one set before.
 *
 * a call reports an error by writing its text, such as
 * "gdCiOpen #4:File not Found". it then passes the text to func, if one is
 * set */
void gdCiEntryErrFunc(GDCI_ERRFUNC func, void *obj);
/* the bytes of the file fname of dir, or of the current directory for a
 * null dir. a null fname returns 0 with the error reported. a file that
 * isn't found returns 0 without a call of func */
Sint32 gdCiGetFileSizeEx(const char *fname, GDFS_DIRREC dir);
/* the bytes of the file fname of the current directory. a null fname, or a
 * failed gdFsGetDirInfo, gives 0. each failure but GDD_ERR_BUSY is
 * reported */
Sint32 gdCiGetFileSize(const char *fname);
/* open the file fname of dir, or of the current directory for a null dir,
 * and return it. rw must be 0.
 *
 * a failure returns null with the error reported. the call fails for a null
 * fname or another rw, and while GDCI_OBJ_NUM files are open. it reports a
 * failed gdFsOpen as "File not Found", even where no GDFS handle was free */
struct gdci_obj *gdCiOpen(const char *fname, GDFS_DIRREC dir, Sint32 rw);
/* close the file, after stopping a read it has asked for */
void gdCiClose(struct gdci_obj *obj);
/* move the file's read position to sct sectors from type, a GDD_SEEK_
 * origin. it returns the position the file then has. a failed gdFsSeek is
 * reported unless it returned GDD_ERR_BUSY */
Sint32 gdCiSeek(struct gdci_obj *obj, Sint32 sct, Sint32 type);
/* the file's read position, in sectors from its start */
Sint32 gdCiTell(struct gdci_obj *obj);
/* ask for nsct sectors of the file, from its read position, into buf. buf
 * must lie on a 32-byte boundary. the call returns the sectors gdFsReqRd32
 * asked the drive for, which can be fewer than nsct, or 0. it marks the file
 * GDCI_STAT_TRANS for any such count.
 *
 * where gdFsReqRd32 fails, the call returns 0 and leaves the file's state as
 * it was. it reports each failure but GDD_ERR_BUSY */
Sint32 gdCiReqRd(struct gdci_obj *obj, Sint32 nsct, void *buf);
/* stop the file's read. the call's one argument is the file, a struct
 * gdci_obj *. the file is then GDCI_STAT_IDLE, even where gdFsStopRd fails.
 * the declaration has no prototype because gdCiClose calls it with no
 * argument */
void gdCiStopTr(); /* NOLINT(clang-diagnostic-deprecated-non-prototype) */
/* update the file's state from the file system's status, and return the
 * state, a GDCI_STAT_ value. only GDD_STAT_COMPLETE and GDD_STAT_ERR change
 * the state, to GDCI_STAT_COMPLETE and GDCI_STAT_ERR */
Sint32 gdCiGetStat(struct gdci_obj *obj);
/* GDD_FS_SCTSIZE, the bytes of a sector, whatever obj is */
Sint32 gdCiGetSctLen(struct gdci_obj *obj);
/* the bytes the file's read has transferred */
Sint32 gdCiGetNumTr(struct gdci_obj *obj);
/* make the directory dirname the current one. it returns 0, or -1 with
 * the error reported */
Sint32 gdCiChangeDir(const char *dirname);
/* how many entries of the current directory are files */
Sint32 gdCiGetNumFiles(void);
/* whether the current directory has an entry fname. a null fname, or a
 * failed gdFsGetDirInfo, gives FALSE. the call reports each failure but
 * GDD_ERR_BUSY, even a missing entry's GDD_ERR_NOTFOUND */
Sint32 gdCiIsExistFile(const char *fname);
/* store in info the name and size of each file of the current directory,
 * up to max_ent of them, and return how many. a null info or a max_ent of
 * 0 returns 0 with the error reported */
Sint32 gdCiLoadDirInfo(struct gdci_dirinfo *info, Sint32 max_ent);
/* GDCI_MAX_BYTE_RATE, whatever obj is */
Sint32 gdCiGetMaxByteRate(struct gdci_obj *obj);

#endif
