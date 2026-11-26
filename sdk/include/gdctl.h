/* the drive's functions, which the file system calls through the table
 * gdFsGetGdDcf returns */
#ifndef GDCTL_H
#define GDCTL_H
#include <sg_xpt.h>
#include <shinobi.h>

/* request states that get_cmd_stat gives beyond the GDD_STAT_ ones */
#define GDFS_STAT_CONTINUE 65 /* a transfer runs, which check_trans follows */
/* gdGdcGetCmdStat's request state 4, which gdctl.c names REQUEST_ABORTED.
 * gdFsGetStatG leaves the handle as it was, so its next call asks for the
 * same request's state */
#define GDFS_STAT_ABORTED 67

/* indexes into get_cmd_stat's info. the request's state says which of
 * REQ_TRNSED and REQ_ERR the first word holds */
#define REQ_TRNSED 0 /* the bytes a busy or complete request has moved */
#define REQ_ERR 0 /* a failed request's GDD_ERR_ code */
#define REQ_EX_ERRCODE 1 /* a failed request's extended error code */
#define REQ_WAIT 2 /* a busy request's wait value */
/* indexes into get_drv_stat's info: the drive state, and the disc type,
 * which is DISC_GDROM for a GD-ROM */
#define DRV_STAT 0
#define DRV_DISC 1
#define DISC_GDROM 0x80

/* the drive's functions. read, req_gd_read, seek, init_drive, the four da_
 * calls, get_toc, get_scd, get_pos and get_ver each issue a drive command.
 * each returns at once with the command's request number, which
 * get_cmd_stat follows. each returns 0 where the drive refuses the
 * command */
struct GDS_DCF {
    Sint32 (*init_system)(void);
    /* read nsct sectors from frame address fad into buf. GDD_FS_TMODE_DMA
     * reads by DMA, and GDD_FS_TMODE_CPU by the CPU through P2. any other
     * tmode issues nothing and returns GDD_ERR_ILLTMODE */
    Sint32 (*read)(Sint32 fad, Sint32 nsct, void *buf, Sint32 tmode);
    /* read nsct sectors from frame address fad into the drive, for trans to
     * move */
    Sint32 (*req_gd_read)(Sint32 fad, Sint32 nsct);
    /* move nbytes of gdchn's read into buf by DMA. it returns 0 where the
     * drive takes the transfer, and another value where it refuses it */
    Sint32 (*trans)(Sint32 gdchn, void *buf, Sint32 nbytes);
    /* a request's state, a GDD_STAT_ or GDFS_STAT_ value, with info filled
     * at the REQ_ indexes. a state the drive gives that has no such value
     * is returned as the drive gives it */
    Sint32 (*get_cmd_stat)(Sint32 gdchn, Sint32 *info);
    void (*read_abort)(Sint32 gdchn);
    /* move the drive's head to frame address fad */
    Sint32 (*seek)(Sint32 fad);
    Sint32 (*init_drive)(void);
    /* FALSE when the drive can't report its state. otherwise it fills info
     * at the DRV_ indexes */
    Bool (*get_drv_stat)(Sint32 *info);
    /* 1 while gdchn's transfer runs, and 0 once it has ended. while it runs,
     * size receives the bytes it has moved */
    Sint32 (*check_trans)(Sint32 gdchn, Sint32 *size);
    Sint32 (*da_play)(Sint32 st_track, Sint32 end_track, Sint32 reptime);
    Sint32 (*da_play_sct)(Sint32 st_sct, Sint32 end_sct, Sint32 reptime);
    Sint32 (*da_pause)(void);
    Sint32 (*da_release)(Sint32 reptime);
    Sint32 (*get_toc)(Sint32 type, void *buf);
    Sint32 (*get_scd)(Sint32 type, Sint32 size, void *buf);
    /* run the drive's server once, and return TRUE */
    Bool (*exec_server)(void);
    void (*reset)(void);
    /* request the head's position. once the request completes, *stat holds
     * the drive state. *track, *fad and *index hold the head's track, frame
     * address and index */
    Sint32 (*get_pos)(Sint32 *stat, Sint32 *track, Sint32 *fad, Sint32 *index);
    Sint32 (*get_ver)(char *ver);
};

/* the drive's functions. the table is constant, and no caller writes it */
GD_DCF *gdFsGetGdDcf(void);

#endif
