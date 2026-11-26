/* GDFS as a file system of the GSC: its table of calls, mwg_gsc_fsg_i, and
 * the calls. each call passes its arguments to the gdFs call of its name, and
 * returns its result. mwGscFsgInit, mwGscFsgReinit and mwGscFsgFinish call
 * nothing */
#include <sg_mwgsc.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 mwGscFsgInit(void);
Sint32 mwGscFsgReinit(void);
void mwGscFsgFinish(void);
GDFS_DIRREC mwGscFsgCreateDirhn(void *dirbuf, Sint32 max_dirent);
Sint32 mwGscFsgLoadDir(const char *dirname, GDFS_DIRREC dirrec);
Sint32 mwGscFsgSetDir(GDFS_DIRREC dirrec);
Sint32 mwGscFsgChangeDir(const char *dirname);
GDFS mwGscFsgOpen(const char *fname, GDFS_DIRREC dirrec);
GDFS mwGscFsgOpenRange(Sint32 stsct, Sint32 nsct);
void mwGscFsgClose(GDFS gdfs);
Sint32 mwGscFsgSeek(GDFS gdfs, Sint32 sctno, Sint32 type);
Sint32 mwGscFsgTell(GDFS gdfs);
Sint32 mwGscFsgCheckEof(GDFS gdfs, Bool *iseof);
Bool mwGscFsgGetFileSize(GDFS gdfs, Sint32 *fsize);
Bool mwGscFsgGetFileSctSize(GDFS gdfs, Sint32 *fsctsize);
Sint32 mwGscFsgReqRd32(GDFS gdfs, Sint32 nsct, void *buf);
Sint32 mwGscFsgGetStat(GDFS gdfs);
Sint32 mwGscFsgGetErrStat(GDFS gdfs);
Sint32 mwGscFsgGetNumRd(GDFS gdfs);
Sint32 mwGscFsgStopRd(GDFS gdfs);
void mwGscFsgExecServer(void);

const char mwg_gsc_fsi_mod[] = "FSI Module: FSG / SHINOBI(GDFS)";
/* NOLINTEND(misc-use-internal-linkage) */

struct gsc_fs_if mwg_gsc_fsg_i = {
    {NULL, NULL, NULL},     mwGscFsgInit,        mwGscFsgReinit,
    mwGscFsgFinish,         mwGscFsgCreateDirhn, mwGscFsgLoadDir,
    mwGscFsgSetDir,         mwGscFsgChangeDir,   mwGscFsgOpen,
    mwGscFsgOpenRange,      mwGscFsgClose,       mwGscFsgSeek,
    mwGscFsgTell,           mwGscFsgCheckEof,    mwGscFsgGetFileSize,
    mwGscFsgGetFileSctSize, mwGscFsgReqRd32,     mwGscFsgGetStat,
    mwGscFsgGetErrStat,     mwGscFsgGetNumRd,    mwGscFsgStopRd,
    mwGscFsgExecServer,
};

Sint32 mwGscFsgInit(void)
{
    Sint32 ret;

    /* @bug uninitialized: returns ret, which nothing sets. the stack pointer
     * becomes the result (MOV R15,R0 at 0x8c15a652) */
#ifdef NONMATCHING
    /* this side repairs the result: it returns 0. the console returns its
     * stack pointer, which C can't give */
    ret = 0;
#endif
    return ret; /* NOLINT(clang-diagnostic-uninitialized) */
}

Sint32 mwGscFsgReinit(void)
{
    Sint32 ret;

    /* @bug uninitialized: returns ret, which nothing sets. the stack pointer
     * becomes the result (MOV R15,R0 at 0x8c15a656) */
#ifdef NONMATCHING
    /* this side repairs the result: it returns 0. the console returns its
     * stack pointer, which C can't give */
    ret = 0;
#endif
    return ret; /* NOLINT(clang-diagnostic-uninitialized) */
}

void mwGscFsgFinish(void)
{
}

GDFS_DIRREC mwGscFsgCreateDirhn(void *dirbuf, Sint32 max_dirent)
{
    GDFS_DIRREC ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsCreateDirhn(dirbuf, max_dirent);
    return ret;
}

Sint32 mwGscFsgLoadDir(const char *dirname, GDFS_DIRREC dirrec)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsLoadDir(dirname, dirrec);
    return ret;
}

Sint32 mwGscFsgSetDir(GDFS_DIRREC dirrec)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsSetDir(dirrec);
    return ret;
}

Sint32 mwGscFsgChangeDir(const char *dirname)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsChangeDir(dirname);
    return ret;
}

GDFS mwGscFsgOpen(const char *fname, GDFS_DIRREC dirrec)
{
    GDFS ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsOpen(fname, dirrec);
    return ret;
}

GDFS mwGscFsgOpenRange(Sint32 stsct, Sint32 nsct)
{
    GDFS ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsOpenRange(stsct, nsct);
    return ret;
}

void mwGscFsgClose(GDFS gdfs)
{
    gdFsClose(gdfs);
}

Sint32 mwGscFsgSeek(GDFS gdfs, Sint32 sctno, Sint32 type)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsSeek(gdfs, sctno, type);
    return ret;
}

Sint32 mwGscFsgTell(GDFS gdfs)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsTell(gdfs);
    return ret;
}

Sint32 mwGscFsgCheckEof(GDFS gdfs, Bool *iseof)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsCheckEof(gdfs, iseof);
    return ret;
}

Bool mwGscFsgGetFileSize(GDFS gdfs, Sint32 *fsize)
{
    Bool ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsGetFileSize(gdfs, fsize);
    return ret;
}

Bool mwGscFsgGetFileSctSize(GDFS gdfs, Sint32 *fsctsize)
{
    Bool ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsGetFileSctSize(gdfs, fsctsize);
    return ret;
}

Sint32 mwGscFsgReqRd32(GDFS gdfs, Sint32 nsct, void *buf)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsReqRd32(gdfs, nsct, buf);
    return ret;
}

Sint32 mwGscFsgGetStat(GDFS gdfs)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsGetStat(gdfs);
    return ret;
}

Sint32 mwGscFsgGetErrStat(GDFS gdfs)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsGetErrStat(gdfs);
    return ret;
}

Sint32 mwGscFsgGetNumRd(GDFS gdfs)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsGetNumRd(gdfs);
    return ret;
}

Sint32 mwGscFsgStopRd(GDFS gdfs)
{
    Sint32 ret;

    /* @hack spelling: ret, a local for the result, where a return of the
     * call would do */
    ret = gdFsStopRd(gdfs);
    return ret;
}

void mwGscFsgEntryErrFunc(GDFS gdfs, GDFS_ERRFUNC erfunc, void *obj)
{
    gdFsEntryErrFunc(gdfs, erfunc, obj);
}

void mwGscFsgExecServer(void)
{
    gdFsExecServer();
}
