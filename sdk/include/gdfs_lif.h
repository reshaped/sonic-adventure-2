/* the file system's interface: the table its public calls are made through */
#ifndef GDFS_LIF_H
#define GDFS_LIF_H
#include <sg_xpt.h>
#include <shinobi.h>

/* the library's interface. each member after unk_00 is the function behind
 * one gdFs call, and takes that call's name in lowercase words, as read_w is
 * gdFsReadW's. gdFsInitOvl calls init with the table itself as lif. no call
 * reads unk_00, which gdg_fs_lif_tbl leaves null */
struct GDS_FS_LIF {
    void *unk_00[3];
    Sint32 (*init)(Sint32 max_open, void *gdfs_work, Sint32 max_dirent,
                   void *dirbuf, GDFS_LIF *lif);
    Sint32 (*reinit)(void);
    void (*finish)(void);
    Sint32 (*set_global_err)(Sint32 err);
    Sint32 (*read_w)(Sint32 fad, Sint32 nsct, void *buf);
    Sint32 (*load_dir)(const char *dirname, GDFS_DIRREC gf_dirrec);
    Sint32 (*load_dir_fast)(const char *dirname, GDFS_DIRREC gf_dirrec);
    Sint32 (*set_dir)(GDFS_DIRREC gf_dirrec);
    Sint32 (*get_dir_info)(const char *name, GDFS_DIRINFO *dirinfo);
    GDFS (*get_work_hn)(void);
    GDFS (*get_sys_hn)(void);
    GDFS (*create_hn)(GDFS gdfs, Sint32 fid, Sint32 fad, Sint32 fsize);
    GDFS (*open)(const char *fname, GDFS_DIRREC gf_dirrec);
    GDFS (*open_fast)(const char *fname, GDFS_DIRREC gf_dirrec);
    GDFS (*open_range)(Sint32 stsct, Sint32 nsct);
    void (*exec_server)(void);
    Sint32 (*get_global_err)(void);
    Sint32 (*get_toc)(Sint32 type, void *buf);
    Sint32 (*get_drv_stat)(void);
    Sint32 (*req_drv_stat)(void);
    Sint32 (*da_play)(Sint32 st_track, Sint32 end_track, Sint32 reptime);
    Sint32 (*da_play_sct)(Sint32 st_sct, Sint32 end_sct, Sint32 reptime);
    Sint32 (*da_stop)(void);
    Sint32 (*da_pause)(void);
    Sint32 (*da_release)(Sint32 reptime);
    Sint32 (*da_get_info)(GDFS_DAINFO *dainfo);
    void (*entry_err_func_all)(GDFS_ERRFUNC erfunc, void *obj);
    Bool (*is_tray_open)(void);
};

/* the table of the library's own functions, which gdFsInitOvl installs */
extern const GDFS_LIF gdg_fs_lif_tbl;

/* gdg_fs_lif_tbl's read_w, with gdFsReadW's contract. the file system must be
 * started: the call reads its work untested */
Sint32 gdFsReadWG(Sint32 fad, Sint32 nsct, void *buf);
/* the little-endian 32-bit value at bytes, as ISO 9660 records hold it */
Sint32 gdFsGetLE4(const Uint8 *bytes);

#endif
