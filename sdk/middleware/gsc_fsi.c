/* the GSC's calls on a file system, through the table mwGscFsiInit sets */
#include <sg_mwgsc.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the member exports these functions, though no header of the SDK declares
 * them. each but mwGscFsiEntryErrFunc calls the table's call of its name, as
 * sg_mwgsc.h's mwGscFsi calls do. without a table, mwGscFsiCreateDirhn
 * returns NULL, mwGscFsiTell 0 and mwGscFsiGetFileSctSize FALSE. the others
 * return GSC_ERR_NOFSI */
/* NOLINTBEGIN(misc-use-internal-linkage) */
GDFS_DIRREC mwGscFsiCreateDirhn(void *dirbuf, Sint32 max_dirent);
Sint32 mwGscFsiLoadDir(const char *dirname, GDFS_DIRREC dirrec);
Sint32 mwGscFsiSetDir(GDFS_DIRREC dirrec);
Sint32 mwGscFsiChangeDir(const char *dirname);
Sint32 mwGscFsiTell(GDFS gdfs);
Sint32 mwGscFsiCheckEof(GDFS gdfs, Bool *iseof);
Bool mwGscFsiGetFileSctSize(GDFS gdfs, Sint32 *fsctsize);
Sint32 mwGscFsiGetErrStat(GDFS gdfs);
Sint32 mwGscFsiStopRd(GDFS gdfs);
/* pass the arguments on to mwGscFsgEntryErrFunc, whatever the table */
void mwGscFsiEntryErrFunc(GDFS gdfs, GDFS_ERRFUNC erfunc, void *obj);
/* NOLINTEND(misc-use-internal-linkage) */

/* the table the calls go through, or null */
static struct gsc_fs_if *gsc_fsi = NULL;

Sint32 mwGscFsiInit(void)
{
    gsc_fsi = &mwg_gsc_fsg_i;
    /* @hack redundant: the test of the table's address, which is never
     * null */
    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    return 0;
}

Sint32 mwGscFsiReinit(void)
{
    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    return 0;
}

void mwGscFsiFinish(void)
{
    /* @hack redundant: the test of the table, where a store of null either
     * way would do */
    if (gsc_fsi != NULL) {
        gsc_fsi = NULL;
    }
}

GDFS_DIRREC mwGscFsiCreateDirhn(void *dirbuf, Sint32 max_dirent)
{
    GDFS_DIRREC dirrec;

    if (gsc_fsi == NULL) {
        return NULL;
    }
    /* @hack spelling: dirrec, a local for the result, where a return of
     * the call would do */
    dirrec = gsc_fsi->create_dirhn(dirbuf, max_dirent);
    return dirrec;
}

Sint32 mwGscFsiLoadDir(const char *dirname, GDFS_DIRREC dirrec)
{
    Sint32 err;

    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    /* @hack spelling: err, a local for the result, where a return of
     * the call would do */
    err = gsc_fsi->load_dir(dirname, dirrec);
    return err;
}

Sint32 mwGscFsiSetDir(GDFS_DIRREC dirrec)
{
    Sint32 err;

    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    /* @hack spelling: err, a local for the result, where a return of
     * the call would do */
    err = gsc_fsi->set_dir(dirrec);
    return err;
}

Sint32 mwGscFsiChangeDir(const char *dirname)
{
    Sint32 err;

    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    /* @hack spelling: err, a local for the result, where a return of
     * the call would do */
    err = gsc_fsi->change_dir(dirname);
    return err;
}

GDFS mwGscFsiOpen(const char *fname, GDFS_DIRREC dirrec)
{
    GDFS gdfs;

    if (gsc_fsi == NULL) {
        return NULL;
    }
    /* @hack spelling: gdfs, a local for the result, where a return of
     * the call would do */
    gdfs = gsc_fsi->open(fname, dirrec);
    return gdfs;
}

GDFS mwGscFsiOpenRange(Sint32 stsct, Sint32 nsct)
{
    GDFS gdfs;

    if (gsc_fsi == NULL) {
        return NULL;
    }
    /* @hack spelling: gdfs, a local for the result, where a return of
     * the call would do */
    gdfs = gsc_fsi->open_range(stsct, nsct);
    return gdfs;
}

void mwGscFsiClose(GDFS gdfs)
{
    if (gsc_fsi != NULL) {
        gsc_fsi->close(gdfs);
    }
}

Sint32 mwGscFsiSeek(GDFS gdfs, Sint32 sctno, Sint32 type)
{
    Sint32 err;

    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    /* @hack spelling: err, a local for the result, where a return of
     * the call would do */
    err = gsc_fsi->seek(gdfs, sctno, type);
    return err;
}

Sint32 mwGscFsiTell(GDFS gdfs)
{
    Sint32 sct;

    if (gsc_fsi == NULL) {
        return 0;
    }
    /* @hack spelling: sct, a local for the result, where a return of
     * the call would do */
    sct = gsc_fsi->tell(gdfs);
    return sct;
}

Sint32 mwGscFsiCheckEof(GDFS gdfs, Bool *iseof)
{
    Sint32 err;

    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    /* @hack spelling: err, a local for the result, where a return of
     * the call would do */
    err = gsc_fsi->check_eof(gdfs, iseof);
    return err;
}

Bool mwGscFsiGetFileSize(GDFS gdfs, Sint32 *fsize)
{
    Bool ok;

    if (gsc_fsi == NULL) {
        return FALSE;
    }
    /* @hack spelling: ok, a local for the result, where a return of
     * the call would do */
    ok = gsc_fsi->get_file_size(gdfs, fsize);
    return ok;
}

Bool mwGscFsiGetFileSctSize(GDFS gdfs, Sint32 *fsctsize)
{
    Bool ok;

    if (gsc_fsi == NULL) {
        return FALSE;
    }
    /* @hack spelling: ok, a local for the result, where a return of
     * the call would do */
    ok = gsc_fsi->get_file_sct_size(gdfs, fsctsize);
    return ok;
}

Sint32 mwGscFsiReqRd32(GDFS gdfs, Sint32 nsct, void *buf)
{
    Sint32 nreq;

    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    /* @hack spelling: nreq, a local for the result, where a return of
     * the call would do */
    nreq = gsc_fsi->req_rd32(gdfs, nsct, buf);
    return nreq;
}

Sint32 mwGscFsiGetStat(GDFS gdfs)
{
    Sint32 stat;

    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    /* @hack spelling: stat, a local for the result, where a return of
     * the call would do */
    stat = gsc_fsi->get_stat(gdfs);
    return stat;
}

Sint32 mwGscFsiGetErrStat(GDFS gdfs)
{
    Sint32 err;

    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    /* @hack spelling: err, a local for the result, where a return of
     * the call would do */
    err = gsc_fsi->get_err_stat(gdfs);
    return err;
}

Sint32 mwGscFsiGetNumRd(GDFS gdfs)
{
    Sint32 nbyte;

    if (gsc_fsi == NULL) {
        return 0;
    }
    /* @hack spelling: nbyte, a local for the result, where a return of
     * the call would do */
    nbyte = gsc_fsi->get_num_rd(gdfs);
    return nbyte;
}

Sint32 mwGscFsiStopRd(GDFS gdfs)
{
    Sint32 err;

    if (gsc_fsi == NULL) {
        return GSC_ERR_NOFSI;
    }
    /* @hack spelling: err, a local for the result, where a return of
     * the call would do */
    err = gsc_fsi->stop_rd(gdfs);
    return err;
}

void mwGscFsiEntryErrFunc(GDFS gdfs, GDFS_ERRFUNC erfunc, void *obj)
{
    mwGscFsgEntryErrFunc(gdfs, erfunc, obj);
}

void mwGscFsiExecServer(void)
{
    if (gsc_fsi != NULL) {
        gsc_fsi->exec_server();
    }
}
