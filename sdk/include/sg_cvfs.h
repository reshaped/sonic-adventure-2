/* cvfs, CRI's file layer. a device is a table of calls under a name, and
 * cvfs makes a file's calls through the table of the file's device */
#ifndef SG_CVFS_H
#define SG_CVFS_H
#include <sg_xpt.h>

/* how many devices cvfs can hold, and how many files it can have open */
#define CVFS_DEV_NUM 32
#define CVFS_FILE_NUM 32
/* the bytes of room cvfs has for a device's name, with the terminator */
#define CVFS_DEVNAME_MAX 9
/* the bytes of room cvfs has for the device's part of a file's name, and
 * again for its path, each with its terminator */
#define CVFS_FNAME_MAX 36
/* cvFsSeek's origin for the file's start */
#define CVFS_SEEK_SET 0
/* cvFsGetStat's states for a read that has ended and for one that failed */
#define CVFS_STAT_COMPLETE 1
#define CVFS_STAT_ERROR 3

/* an open file */
struct cvfs_file;

/* a file's name has two parts, as "GD:FILE.BIN" has: the device's, up to
 * the colon, and the path, which goes to the device as it is. a name
 * without a colon is a path on the default device. the device's part may be
 * in small letters. it may also be cut short: the first device whose name
 * starts with it is the one taken. each part must be shorter than
 * CVFS_FNAME_MAX bytes.
 *
 * a call reports a name it can't use as an error. a name can't be used where
 * it is null or its path is empty. nor can it where it has no device's part
 * and there is no default. nor can it where no device's name starts with its
 * device's part.
 *
 * where a call's comment says it reports an error, the call passes a text
 * such as "cvFsOpen #4:device not found" to the function cvFsEntryErrFunc
 * was given, if any */

/* cvfs counts the calls of cvFsInit that no cvFsFinish has answered. the
 * cvFsInit that finds the count at 0 frees every file and device, and
 * leaves no default device. the cvFsFinish that brings the count back to 0
 * closes each file still open, by its device's Close. it then frees every
 * file and device too, and leaves no default device. for a file open on a
 * device without a Close, cvFsFinish calls through null */
void cvFsInit(void);
void cvFsFinish(void);
/* add a device named devname, whose table of calls getdevif returns. the
 * table is laid out as gdci.h's struct gdci_if, and must not be null.
 * devname is put in capitals in place, and must be shorter than
 * CVFS_DEVNAME_MAX bytes. a null devname or getdevif is reported.
 *
 * the device isn't added where a device's name already starts with devname.
 * nor is it where CVFS_DEV_NUM devices are held. neither case is reported.
 * either way, the table's EntryErrFunc, if it has one, is given
 * cvFsCallUsrErrFn. the device's errors then go to the function
 * cvFsEntryErrFunc was given */
void cvFsAddDev(char *devname, void *(*getdevif)(void), void *unused);
/* make the device devname the default. devname is put in capitals in place.
 * an empty devname leaves no default. a null devname is reported, and so is
 * one that no device's name starts with: both leave the default as it was */
void cvFsSetDefDev(char *devname);
/* open the file fname. dir and rw go to the device's Open as they are. it
 * returns the file, or null after reporting the error. the errors are a
 * name it can't use and CVFS_FILE_NUM files open. they are also a device
 * without an Open, and an Open that returns null */
struct cvfs_file *cvFsOpen(const char *fname, void *dir, Sint32 rw);
/* close the file by its device's Close, and free it. a null file is
 * reported. so is a device without a Close, and the file then stays open */
void cvFsClose(struct cvfs_file *file);
/* each of these makes the call of its name in the file's device, such as Tell
 * for cvFsTell. it passes the device's own handle on the file. the file must be
 * open: closing it nulls the table each call reads. each but cvFsStopTr returns
 * the call's result. gdci.h says what the disc's calls do. cvFsSeek's sct and
 * origin go to the device as they are, and so do cvFsReqRd's nsct and buf. a
 * null file is reported, and so is a device without the call. after either
 * error, cvFsGetStat returns CVFS_STAT_ERROR, and cvFsTell, cvFsSeek and
 * cvFsReqRd return 0 */
Sint32 cvFsTell(struct cvfs_file *file);
Sint32 cvFsSeek(struct cvfs_file *file, Sint32 sct, Sint32 origin);
Sint32 cvFsReqRd(struct cvfs_file *file, Sint32 nsct, void *buf);
void cvFsStopTr(struct cvfs_file *file);
Sint32 cvFsGetStat(struct cvfs_file *file);
/* the size of the file fname, as its device's GetFileSize gives it: for the
 * disc, in bytes. cvFsGetFileSizeEx asks GetFileSizeEx instead, and passes
 * it dir as it is. both return 0 after reporting a name they can't use, or
 * a device without the call */
Sint32 cvFsGetFileSize(const char *fname);
Sint32 cvFsGetFileSizeEx(const char *fname, void *dir);

#endif
