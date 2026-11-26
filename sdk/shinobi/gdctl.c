/* the table of drive functions gdFsGetGdDcf returns, each wrapping a boot
 * ROM drive call */
#include <gdctl.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the drive's commands, gdGdcReqCmd's cmd */
#define CMD_PIOREAD 16
#define CMD_DMAREAD 17
#define CMD_GETTOC2 19
#define CMD_PLAY 20
#define CMD_PLAY2 21
#define CMD_PAUSE 22
#define CMD_RELEASE 23
#define CMD_INIT 24
#define CMD_SEEK 27
#define CMD_DMAREAD_STREAM 28
#define CMD_GETSCD 34
#define CMD_GET_POS 36
#define CMD_GET_VERS 40

/* gdGdcGetCmdStat's request states */
#define REQUEST_FAILED (-1)
#define REQUEST_NONE 0
#define REQUEST_RUNNING 1
#define REQUEST_DONE 2
#define REQUEST_STREAMING 3
#define REQUEST_ABORTED 4

/* gdGdcGetDrvStat's result where it can't give the drive's state */
#define DRIVE_STATE_FAILED 1

/* the drive's errors, in a failed request's err. each takes its name from
 * the GDD_ERR_ code that gdc_get_cmd_stat gives for it */
#define DRIVE_NO_ERROR 0
#define DRIVE_RECOVERED 1
#define DRIVE_NOT_READY 2
#define DRIVE_MEDIA 3
#define DRIVE_HARDWARE 4
#define DRIVE_ILLEGAL_REQUEST 5
#define DRIVE_UNIT_ATTENTION 6
#define DRIVE_PROTECTED 7
#define DRIVE_ABORTED 11
#define DRIVE_NOT_READABLE 16
#define DRIVE_BUS_SEMAPHORE 32

/* an address's top three bits select its area. AREA_MASK clears them, and
 * P2_BASE sets those of P2 */
#define AREA_MASK 0x1fffffff
/* an address shifted right by NIBBLE_SHIFT holds its top four bits at the
 * bottom. the mask NIBBLE_AREA keeps the three of them that select the area,
 * and NIBBLE_ALL keeps all four. NIBBLE_P2 is P2's top four bits */
#define NIBBLE_SHIFT 28
#define NIBBLE_AREA 0x0e
#define NIBBLE_ALL 0x0f
#define NIBBLE_P2 (P2_BASE >> NIBBLE_SHIFT)
/* the mask of an address's offset within its 32-byte operand cache line */
#define LINE_MASK 0x1f

static Sint32 gdc_init_system(void)
{
    gdGdcInitSystem();
    return TRUE;
}

/* once the drive takes the read, drop buf's operand cache lines, so the CPU
 * reads the new data. where buf starts inside a line, write the lines back
 * before dropping them. a buf in P2 needs neither, since P2 isn't cached */
static Sint32 gdc_read(Sint32 fad, Sint32 nsct, void *buf, Sint32 tmode)
{
    Sint32 param[4];
    Sint32 cmd;
    Sint32 gdchn;

    param[0] = fad;
    param[1] = nsct;
    param[3] = 0;
    if (tmode == GDD_FS_TMODE_DMA) {
        cmd = CMD_DMAREAD;
        param[2] = (Sint32)buf & AREA_MASK;
    } else if (tmode == GDD_FS_TMODE_CPU) {
        cmd = CMD_PIOREAD;
        param[2] = ((Sint32)buf & AREA_MASK) | P2_BASE;
    } else {
        return GDD_ERR_ILLTMODE;
    }
    gdchn = gdGdcReqCmd(cmd, param);
    if (gdchn != 0 &&
        (((Sint32)buf >> NIBBLE_SHIFT) & NIBBLE_AREA) != NIBBLE_P2) {
        /* @hack spelling: size, the bytes through a local of their own,
         * where each call could take nsct * GDD_FS_SCTSIZE */
        Uint32 size = nsct * GDD_FS_SCTSIZE;

        if (((Sint32)buf & LINE_MASK) == 0) {
            syCacheOCBI(buf, size);
        } else {
            syCacheOCBP(buf, size);
        }
    }
    return gdchn;
}

static Sint32 gdc_req_gd_read(Sint32 fad, Sint32 nsct)
{
    Sint32 param[2];

    param[0] = fad;
    param[1] = nsct;
    return gdGdcReqCmd(CMD_DMAREAD_STREAM, param);
}

/* where the drive takes the transfer and nbytes isn't 0, treat buf's lines
 * as gdc_read does. unlike gdc_read's, its P2 test compares all four top
 * bits. a buf in P2 from 0xb0000000 up still takes a cache operation */
static Sint32 gdc_trans(Sint32 gdchn, void *buf, Sint32 nbytes)
{
    Sint32 param[2];
    Sint32 result;

    param[0] = (Sint32)buf;
    param[1] = nbytes;
    result = gdGdcReqDmaTrans(gdchn, param);
    if (result == 0 && nbytes != 0 &&
        (((Sint32)buf >> NIBBLE_SHIFT) & NIBBLE_ALL) != NIBBLE_P2) {
        if (((Sint32)buf & LINE_MASK) == 0) {
            syCacheOCBI(buf, nbytes);
        } else {
            syCacheOCBP(buf, nbytes);
        }
    }
    return result;
}

static Sint32 gdc_get_cmd_stat(Sint32 gdchn, Sint32 *info)
{
    struct {
        Sint32 err;
        Sint32 ex_errcode;
        Sint32 trnsed;
        Sint32 wait;
    } stat;
    Sint32 result = gdGdcGetCmdStat(gdchn, &stat);

    switch (result) {
    case REQUEST_NONE:
        result = GDD_STAT_IDLE;
        break;
    case REQUEST_RUNNING:
        result = GDD_STAT_BUSY;
        info[REQ_TRNSED] = stat.trnsed;
        info[REQ_WAIT] = stat.wait;
        break;
    case REQUEST_DONE:
        result = GDD_STAT_COMPLETE;
        info[REQ_TRNSED] = stat.trnsed;
        break;
    case REQUEST_STREAMING:
        result = GDFS_STAT_CONTINUE;
        break;
    case REQUEST_ABORTED:
        result = GDFS_STAT_ABORTED;
        break;
    case REQUEST_FAILED:
        info[REQ_EX_ERRCODE] = stat.ex_errcode;
        result = GDD_STAT_ERR;
        switch (stat.err) {
        case DRIVE_NO_ERROR:
            info[REQ_ERR] = GDD_ERR_NOERR;
            break;
        case DRIVE_RECOVERED:
            info[REQ_ERR] = GDD_ERR_RECOVER;
            break;
        case DRIVE_NOT_READY:
            info[REQ_ERR] = GDD_ERR_NOTREADY;
            break;
        case DRIVE_MEDIA:
            info[REQ_ERR] = GDD_ERR_MEDIA;
            break;
        case DRIVE_HARDWARE:
            info[REQ_ERR] = GDD_ERR_HWARE;
            break;
        case DRIVE_ILLEGAL_REQUEST:
            info[REQ_ERR] = GDD_ERR_ILLREQ;
            break;
        case DRIVE_UNIT_ATTENTION:
            info[REQ_ERR] = GDD_ERR_UNITATTENT;
            break;
        case DRIVE_PROTECTED:
            info[REQ_ERR] = GDD_ERR_PROTECT;
            break;
        case DRIVE_ABORTED:
            info[REQ_ERR] = GDD_ERR_ABORT;
            break;
        case DRIVE_NOT_READABLE:
            info[REQ_ERR] = GDD_ERR_NOREADABLE;
            break;
        case DRIVE_BUS_SEMAPHORE:
            info[REQ_ERR] = GDD_ERR_BUSSMPH;
            break;
        default:
            info[REQ_ERR] = GDD_ERR_UNDEF;
            break;
        }
        break;
    }
    return result;
}

static void gdc_read_abort(Sint32 gdchn)
{
    gdGdcReadAbort(gdchn);
}

static Sint32 gdc_seek(Sint32 fad)
{
    Sint32 param[2];

    param[0] = fad;
    param[1] = 0;
    return gdGdcReqCmd(CMD_SEEK, param);
}

static Sint32 gdc_init_drive(void)
{
    /* @hack spelling: the request's number through gdchn, where return
     * gdGdcReqCmd(...) would do */
    Sint32 gdchn = gdGdcReqCmd(CMD_INIT, NULL);

    return gdchn;
}

static Bool gdc_get_drv_stat(Sint32 *info)
{
    /* @hack spelling: the call's result through result, where the test could
     * take the call */
    Sint32 result = gdGdcGetDrvStat(info);

    if (result == DRIVE_STATE_FAILED) {
        return FALSE;
    }
    return TRUE;
}

static Sint32 gdc_check_trans(Sint32 gdchn, Sint32 *size)
{
    /* @hack spelling: the call's result through result, where return
     * gdGdcCheckDmaTrans(...) would do */
    Sint32 result = gdGdcCheckDmaTrans(gdchn, size);

    return result;
}

static Sint32 gdc_da_play(Sint32 st_track, Sint32 end_track, Sint32 reptime)
{
    Sint32 param[4];

    param[0] = st_track;
    param[1] = end_track;
    param[2] = reptime;
    param[3] = 0;
    return gdGdcReqCmd(CMD_PLAY, param);
}

static Sint32 gdc_da_play_sct(Sint32 st_sct, Sint32 end_sct, Sint32 reptime)
{
    Sint32 param[4];

    param[0] = st_sct;
    param[1] = end_sct;
    param[2] = reptime;
    param[3] = 0;
    return gdGdcReqCmd(CMD_PLAY2, param);
}

static Sint32 gdc_da_pause(void)
{
    /* @hack spelling: the request's number through gdchn, where return
     * gdGdcReqCmd(...) would do */
    Sint32 gdchn = gdGdcReqCmd(CMD_PAUSE, NULL);

    return gdchn;
}

static Sint32 gdc_da_release(Sint32 reptime)
{
    Sint32 param[2];

    param[0] = reptime;
    param[1] = 0;
    return gdGdcReqCmd(CMD_RELEASE, param);
}

static Sint32 gdc_get_toc(Sint32 type, void *buf)
{
    Sint32 param[2];

    param[0] = type;
    param[1] = (Sint32)buf;
    return gdGdcReqCmd(CMD_GETTOC2, param);
}

static Sint32 gdc_get_scd(Sint32 type, Sint32 size, void *buf)
{
    Sint32 param[4];

    param[0] = type;
    param[1] = size;
    param[2] = (Sint32)buf;
    param[3] = 0;
    return gdGdcReqCmd(CMD_GETSCD, param);
}

static Bool gdc_exec_server(void)
{
    gdGdcExecServer();
    return TRUE;
}

static void gdc_reset(void)
{
    gdGdcReset();
}

/* NOLINTBEGIN(readability-non-const-parameter) */
static Sint32 gdc_get_pos(Sint32 *stat, Sint32 *track, Sint32 *fad,
                          Sint32 *index)
/* NOLINTEND(readability-non-const-parameter) */
{
    Sint32 param[4];

    param[0] = (Sint32)stat;
    param[1] = (Sint32)track;
    param[2] = (Sint32)fad;
    param[3] = (Sint32)index;
    return gdGdcReqCmd(CMD_GET_POS, param);
}

/* NOLINTNEXTLINE(readability-non-const-parameter) */
static Sint32 gdc_get_ver(char *ver)
{
    Sint32 param[2];

    param[0] = (Sint32)ver;
    param[1] = 0;
    return gdGdcReqCmd(CMD_GET_VERS, param);
}

static const GD_DCF gdc_dcf = {
    gdc_init_system,  gdc_read,        gdc_req_gd_read, gdc_trans,
    gdc_get_cmd_stat, gdc_read_abort,  gdc_seek,        gdc_init_drive,
    gdc_get_drv_stat, gdc_check_trans, gdc_da_play,     gdc_da_play_sct,
    gdc_da_pause,     gdc_da_release,  gdc_get_toc,     gdc_get_scd,
    gdc_exec_server,  gdc_reset,       gdc_get_pos,     gdc_get_ver,
};

GD_DCF *gdFsGetGdDcf(void)
{
    return (GD_DCF *)&gdc_dcf;
}
