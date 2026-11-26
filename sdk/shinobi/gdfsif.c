/* the file system's public calls, each passed on to its function in the
 * interface */
#include <gdfs_lif.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the member exports these functions, though no header of the SDK declares
 * them. without an interface each does nothing, and returns null for a
 * handle and 0 otherwise */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* read nsct sectors from frame address fad into buf through the system
 * handle, and wait for them. it returns GDD_ERR_BUSY while the system
 * handle's request runs, and gdFsRead's result otherwise */
Sint32 gdFsReadW(Sint32 fad, Sint32 nsct, void *buf);
/* gdFsSetGlobalErr sets the library's global error, a GDD_ERR_ code, and
 * returns TRUE. gdFsGetGlobalErr returns the global error */
Sint32 gdFsSetGlobalErr(Sint32 err);
Sint32 gdFsGetGlobalErr(void);
/* open the closed handle gdfs on fsize bytes from frame address fad, with fid
 * its entry number. it returns gdfs, or null for an open gdfs. the handle
 * gets no callbacks, and gdg_fs_def_tmode's transfer mode */
GDFS gdFsCreateHn(GDFS gdfs, Sint32 fid, Sint32 fad, Sint32 fsize);
/* gdFsLoadDirFast does what gdFsLoadDir does, and gdFsOpenFast what gdFsOpen
 * does */
Sint32 gdFsLoadDirFast(const char *dirname, GDFS_DIRREC gf_dirrec);
GDFS gdFsOpenFast(const char *fname, GDFS_DIRREC gf_dirrec);
/* NOLINTEND(misc-use-internal-linkage) */

/* the interface the calls are made through, or null before gdFsInitOvl and
 * after gdFsFinish */
static GDFS_LIF *gdfs_lif = NULL;

Sint32 gdFsReadW(Sint32 fad, Sint32 nsct, void *buf)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->read_w(fad, nsct, buf);
    }
    return 0;
}

Sint32 gdFsSetGlobalErr(Sint32 err)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->set_global_err(err);
    }
    return 0;
}

Sint32 gdFsGetGlobalErr(void)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->get_global_err();
    }
    return 0;
}

Sint32 gdFsInitOvl(Sint32 max_open, void *gdfs_work, Sint32 max_dirent,
                   void *dirbuf)
{
    Sint32 ret;

    gdfs_lif = (GDFS_LIF *)&gdg_fs_lif_tbl;
    /* @hack spelling: ret, a local for the result, where return
     * gdfs_lif->init(...) would do */
    ret = gdfs_lif->init(max_open, gdfs_work, max_dirent, dirbuf, gdfs_lif);
    return ret;
}

Sint32 gdFsReinit(void)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->reinit();
    }
    return GDD_ERR_INIT;
}

void gdFsFinish(void)
{
    if (gdfs_lif != NULL) {
        gdfs_lif->finish();
        gdfs_lif = NULL;
    }
}

GDFS gdFsCreateHn(GDFS gdfs, Sint32 fid, Sint32 fad, Sint32 fsize)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->create_hn(gdfs, fid, fad, fsize);
    }
    return NULL;
}

Sint32 gdFsLoadDir(const char *dirname, GDFS_DIRREC gf_dirrec)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->load_dir(dirname, gf_dirrec);
    }
    return 0;
}

Sint32 gdFsLoadDirFast(const char *dirname, GDFS_DIRREC gf_dirrec)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->load_dir_fast(dirname, gf_dirrec);
    }
    return 0;
}

Sint32 gdFsSetDir(GDFS_DIRREC gf_dirrec)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->set_dir(gf_dirrec);
    }
    return 0;
}

Sint32 gdFsGetDirInfo(const char *name, GDFS_DIRINFO *dirinfo)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->get_dir_info(name, dirinfo);
    }
    return 0;
}

GDFS gdFsOpen(const char *fname, GDFS_DIRREC gf_dirrec)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->open(fname, gf_dirrec);
    }
    return NULL;
}

GDFS gdFsOpenFast(const char *fname, GDFS_DIRREC gf_dirrec)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->open_fast(fname, gf_dirrec);
    }
    return NULL;
}

GDFS gdFsOpenRange(Sint32 stsct, Sint32 nsct)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->open_range(stsct, nsct);
    }
    return NULL;
}

GDFS gdFsGetWorkHn(void)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->get_work_hn();
    }
    return NULL;
}

GDFS gdFsGetSysHn(void)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->get_sys_hn();
    }
    return NULL;
}

void gdFsExecServer(void)
{
    if (gdfs_lif != NULL) {
        gdfs_lif->exec_server();
    }
}

void gdFsEntryErrFuncAll(GDFS_ERRFUNC erfunc, void *obj)
{
    if (gdfs_lif != NULL) {
        gdfs_lif->entry_err_func_all(erfunc, obj);
    }
}

Sint32 gdFsGetToc(Sint32 type, void *buf)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->get_toc(type, buf);
    }
    return 0;
}

Sint32 gdFsGetDrvStat(void)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->get_drv_stat();
    }
    return 0;
}

Sint32 gdFsReqDrvStat(void)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->req_drv_stat();
    }
    return 0;
}

Sint32 gdFsDaPlay(Sint32 st_track, Sint32 end_track, Sint32 reptime)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->da_play(st_track, end_track, reptime);
    }
    return 0;
}

Sint32 gdFsDaPlaySct(Sint32 st_sct, Sint32 end_sct, Sint32 reptime)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->da_play_sct(st_sct, end_sct, reptime);
    }
    return 0;
}

Sint32 gdFsDaStop(void)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->da_stop();
    }
    return 0;
}

Sint32 gdFsDaPause(void)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->da_pause();
    }
    return 0;
}

Sint32 gdFsDaRelease(Sint32 reptime)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->da_release(reptime);
    }
    return 0;
}

Sint32 gdFsDaGetInfo(GDFS_DAINFO *dainfo)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->da_get_info(dainfo);
    }
    return 0;
}

Bool gdFsIsTrayOpen(void)
{
    if (gdfs_lif != NULL) {
        return gdfs_lif->is_tray_open();
    }
    return FALSE;
}
