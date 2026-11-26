/* the Maple bus driver's start and stop, its frames' send and receive, and
 * its settings */
#include <kaede.h>
#include <machine.h>
#include <maple_driver.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdarg.h>
#include <stdio.h>

/* what the driver writes to MPDRV_REG_DMA_RANGE. the top half is the
 * register's key, 0x6155. the low half, 0x40ff, gives the addresses the DMA
 * may write */
#define DRIVER_RANGE 0x615540ff
/* the nanoseconds in each microsecond of kdgGunModeTimeout */
#define NS_PER_US 1000
/* where mpdrv_exit points MPDRV_REG_DMA_TABLE: the last 64 KB of main
 * memory */
#define PARKED_FRAME 0x0cff0000
/* how long mpdrv_exit waits for a DMA to end, in syTmrGetCount's counts. it
 * waits EXIT_WAIT before it stops the bus, and STOP_WAIT after */
#define EXIT_WAIT 100000
#define STOP_WAIT 17000

/* the boundary mpdrv_init and mpdrv_init_ex round each buffer up to */
#define BUFFER_ALIGN 32
/* what mpdrv_init_ex writes to _mp->enable for a root device and for an
 * expansion device */
#define ENABLE_ROOT 'r'
#define ENABLE_EXPANSION 'e'

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the driver's record of device */
const struct maple_device *mpdrv_get_device(Sint32 device);
/* the frame mpdrv_frame_send sent last */
void *mpdrv_get_lastsendaddr(void);
/* the function mpdrv_dpf passes its text to, or null */
void (*_mpdrv_dbg_out)(const char *text) = NULL;
/* NOLINTEND(misc-use-internal-linkage) */

static const char maple_no_semaphore[] =
    "***FATAL ERROR:Cannot get semaphore!\n";

/* mpdrv_dpf's text */
static char mpdrv_dpf_buffer[256];

Sint32 mpdrv_init(void *sendbuf, Sint32 send_bytes, void *recvbuf,
                  Sint32 recv_bytes)
{
    Sint32 port;

    _mp = &_mp_Context;
    _mp->sendbuf = (Uint32)sendbuf;
    _mp->recvbuf = (Uint32)recvbuf;
    if (_mp->sendbuf & (BUFFER_ALIGN - 1)) {
        _mp->sendbuf += BUFFER_ALIGN;
        _mp->sendbuf &= ~(BUFFER_ALIGN - 1);
    }
    if (_mp->recvbuf & (BUFFER_ALIGN - 1)) {
        _mp->recvbuf += BUFFER_ALIGN;
        _mp->recvbuf &= ~(BUFFER_ALIGN - 1);
    }
    _mp->sendbuf |= P2_BASE;
    _mp->recvbuf |= P2_BASE;
    mpdrv_init_globals();
    for (port = 0; port < MPDRV_PORTS; port++) {
        if (_mp->enable[_mp_RootDeviceNo[port]]) {
            mpdrv_set_dreq_ns(_mp_RootDeviceNo[port]);
        }
    }
    mpdrv_set_end_ns();
    _mp->received = 1;
    _mp->reg[MPDRV_REG_DMA_RANGE] = DRIVER_RANGE;
    _mp->reg[MPDRV_REG_DMA_TRIGGER] = 0;
    _mp->reg[MPDRV_REG_SYSTEM] = _mp->timeout | _mp->speed;
    _mp->reg[MPDRV_REG_DMA_ENABLE] = 1;
    return 0;
}

/* NOLINTNEXTLINE(readability-non-const-parameter) */
Sint32 mpdrv_init_ex(Sint32 *devmap, void **recvbuf, void *sendbuf, Sint32 num)
{
    Sint32 i;
    Sint32 device;
    Uint32 area;

    _mp = &_mp_Context;
    _mp->sendbuf = (Uint32)sendbuf;
    /* _mp->recvbuf takes the address of recvbuf, a table of buffers, not of
     * a buffer. the loops below replace each response area
     * mpdrv_init_globals points into it */
    _mp->recvbuf = (Uint32)recvbuf;
    if (_mp->sendbuf & (BUFFER_ALIGN - 1)) {
        _mp->sendbuf += BUFFER_ALIGN;
        _mp->sendbuf &= ~(BUFFER_ALIGN - 1);
    }
    if (_mp->recvbuf & (BUFFER_ALIGN - 1)) {
        _mp->recvbuf += BUFFER_ALIGN;
        _mp->recvbuf &= ~(BUFFER_ALIGN - 1);
    }
    _mp->sendbuf |= P2_BASE;
    _mp->recvbuf |= P2_BASE;
    mpdrv_init_globals();
    for (i = 0; i < KDD_DEV_MAX; i++) {
        _mp->recv_data[0][i] = NULL;
        _mp->recv_data[1][i] = NULL;
    }
    for (i = 0; i < num; i++) {
        area = (Uint32)recvbuf[i];
        if (area & (BUFFER_ALIGN - 1)) {
            area += BUFFER_ALIGN;
            area &= ~(BUFFER_ALIGN - 1);
        }
        area |= P2_BASE;
        device = devmap[i];
        _mp->recv_data[0][device] = (Uint32 *)area;
        _mp->recv_data[1][device] = (Uint32 *)(area + MPDRV_RESPONSE_SIZE);
    }
    /* each frame takes MPDRV_RESPONSE_SIZE bytes for each of the num
     * devices */
    _mp->send_frame[0] = (Uint8 *)_mp->sendbuf;
    _mp->send_frame[1] = (Uint8 *)(_mp->sendbuf + (num * MPDRV_RESPONSE_SIZE));
    _mp->last_cmd = (Uint32 *)_mp->send_frame[0];
    for (i = 0; i < KDD_DEV_MAX; i++) {
        _mp->enable[i] = 0;
    }
    for (i = 0; i < num; i++) {
        device = devmap[i];
        switch (device) {
        case KDD_DEV_A0:
        case KDD_DEV_B0:
        case KDD_DEV_C0:
        case KDD_DEV_D0:
            _mp->enable[device] = ENABLE_ROOT;
            break;
        default:
            _mp->enable[device] = ENABLE_EXPANSION;
            break;
        }
    }
    for (i = 0; i < MPDRV_PORTS; i++) {
        if (_mp->enable[_mp_RootDeviceNo[i]]) {
            mpdrv_set_dreq_ns(_mp_RootDeviceNo[i]);
        }
    }
    mpdrv_set_end_ns();
    _mp->received = 1;
    _mp->reg[MPDRV_REG_DMA_RANGE] = DRIVER_RANGE;
    _mp->reg[MPDRV_REG_DMA_TRIGGER] = 0;
    _mp->reg[MPDRV_REG_SYSTEM] = _mp->timeout | _mp->speed;
    _mp->reg[MPDRV_REG_DMA_ENABLE] = 1;
    return 0;
}

/* add _mp->nops NOPs to the frame being built, then each enabled device's
 * standing command, and end the frame. a device with a command queued gets
 * no standing command. a get condition asks a controller for its controller
 * function's condition alone. it returns 0 */
static Sint32 frame_make(void)
{
    Sint32 device;
    Sint32 func;
    struct maple_device *dev = _mp->device;
    KDS_DEVICEINFO *info = _mp->info;

    if (_mp->nops > 0) {
        mpdrv_insertnop(_mp->nops);
    }
    for (device = 0; device < KDD_DEV_MAX; device++, dev++, info++) {
        if (!_mp->enable[device]) {
            continue;
        }
        if (dev->queued) {
            continue;
        }
        switch (dev->command) {
        case MPDRV_CMD_DREQ:
            mpdrv_set_dreq_ns(device);
            break;
        case MPDRV_CMD_ALLSTATREQ:
            mpdrv_set_allstatreq_ns(device);
            break;
        case MPDRV_CMD_DRESET:
            mpdrv_set_dreset_ns(device);
            break;
        case MPDRV_CMD_DKILL:
            mpdrv_set_dkill_ns(device);
            break;
        case MPDRV_CMD_GETCOND:
            func = info->function;
            if (func & KDD_FUNC_CONTROLLER) {
                func = KDD_FUNC_CONTROLLER;
            }
            mpdrv_set_getcond_ns(device, func);
            break;
        /* @hack redundant: the cases of the commands that send nothing, and
         * an empty default, which the switch's end serves */
        case MPDRV_CMD_GETMINFO:
        case MPDRV_CMD_BREAD:
        case MPDRV_CMD_BWRITE:
        case MPDRV_CMD_GETLERR:
        case MPDRV_CMD_NONE:
        case MPDRV_CMD_ABSENT:
            break;
        default:
            break;
        }
    }
    mpdrv_set_end_ns();
    return 0;
}

Sint32 mpdrv_frame_send(void)
{
    Sint32 device;
    struct maple_device *dev;

    if (_mp->received != 1) {
        return MPDRV_NOT_READY;
    }
    if (_mp->reg[MPDRV_REG_DMA_START] & 1) {
        return MPDRV_DMA_BUSY;
    }
    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    if (_mp->active_gunmode != _mp->gunmode) {
        _mp->active_gunmode = _mp->gunmode;
        /* @hack spelling: the three register writes in each branch, where
         * once after the branches would do */
        if (_mp->gunmode) {
            _mp->timeout = MPDRV_TIMEOUT((kdgGunModeTimeout * NS_PER_US) /
                                         MPDRV_TIMEOUT_UNIT);
            _mp->reg[MPDRV_REG_DMA_ENABLE] = 0;
            _mp->reg[MPDRV_REG_SYSTEM] = _mp->timeout | _mp->speed;
            _mp->reg[MPDRV_REG_DMA_ENABLE] = 1;
        } else {
            _mp->timeout = MPDRV_TIMEOUT(MPDRV_START_TIMEOUT);
            _mp->reg[MPDRV_REG_DMA_ENABLE] = 0;
            _mp->reg[MPDRV_REG_SYSTEM] = _mp->timeout | _mp->speed;
            _mp->reg[MPDRV_REG_DMA_ENABLE] = 1;
        }
    }
    frame_make();
    _mp->last_send = _mp->send_frame[_mp->send_side];
    _mp->reg[MPDRV_REG_DMA_TABLE] =
        (Sint32)mpdrv_v2p(_mp->send_frame[_mp->send_side]);
    _mp->reg[MPDRV_REG_DMA_START] = 1;
    _mp->last_cmd_words = 0;
    _mp->send_side ^= 1;
    _mp->last_cmd = (Uint32 *)_mp->send_frame[_mp->send_side];
    _mp->recv_side ^= 1;
    _mp->send_size = 0;
    dev = _mp->device;
    for (device = 0; device < KDD_DEV_MAX; device++, dev++) {
        if (_mp->enable[device]) {
            dev->sent = dev->queued;
            dev->queued = MPDRV_CMD_NONE;
            dev->sent_seq = dev->queued_seq;
            dev->queued_seq = 0;
        }
    }
    _mp->received = 0;
    *_mp_SemaphoreAddress = 0;
    return 0;
}

Sint32 mpdrv_frame_recv_pre(void)
{
    if (_mp->received) {
        return MPDRV_NOT_READY;
    }
    if (_mp->reg[MPDRV_REG_DMA_START] & 1) {
        return MPDRV_DMA_BUSY;
    }
    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    return 0;
}

Sint32 mpdrv_frame_recv_post(void)
{
    _mp->received = 1;
    *_mp_SemaphoreAddress = 0;
    return 0;
}

Sint32 mpdrv_frame_recv_main(void)
{
    Sint32 port;
    Sint32 socket;

    for (port = 0; port < MPDRV_PORTS; port++) {
        if (!_mp->enable[_mp_RootDeviceNo[port]]) {
            continue;
        }
        mpdrv_check_port(port);
        for (socket = 0; socket < MPDRV_PORT_DEVICES; socket++) {
            if (_mp->enable[(port * MPDRV_PORT_DEVICES) + socket]) {
                mpdrv_make_devicedata((port * MPDRV_PORT_DEVICES) + socket,
                                      port);
            }
        }
    }
    return 0;
}

Sint32 mpdrv_exit(void)
{
    Uint32 start = syTmrGetCount();

    while (_mp->reg[MPDRV_REG_DMA_START] & 1) {
        /* @hack type: the counts compared as a Sint32 in both waits, where
         * syTmrDiffCount's Uint32 would do */
        if ((Sint32)syTmrDiffCount(start, syTmrGetCount()) > EXIT_WAIT) {
            break;
        }
    }
    _mp->reg[MPDRV_REG_DMA_START] = 0;
    _mp->reg[MPDRV_REG_DMA_ENABLE] = 0;
    _mp->reg[MPDRV_REG_DMA_TRIGGER] = 0;
    _mp->reg[MPDRV_REG_DMA_RANGE] = DRIVER_RANGE;
    _mp->reg[MPDRV_REG_SYSTEM] = MPDRV_TIMEOUT(MPDRV_START_TIMEOUT);
    _mp->reg[MPDRV_REG_DMA_TABLE] = PARKED_FRAME;
    start = syTmrGetCount();
    while (_mp->reg[MPDRV_REG_DMA_START] & 1) {
        if ((Sint32)syTmrDiffCount(start, syTmrGetCount()) > STOP_WAIT) {
            break;
        }
    }
    return 0;
}

void mpdrv_dpf(const char *format, ...)
{
    va_list args;

    if (_mp->debugmode) {
        va_start(args, format);
        vsprintf(mpdrv_dpf_buffer, format, args);
        if (_mpdrv_dbg_out != NULL) {
            _mpdrv_dbg_out(mpdrv_dpf_buffer);
        }
    }
}

const struct maple_device *mpdrv_get_device(Sint32 device)
{
    return &_mp->device[device];
}

const KDS_DEVICEINFO *mpdrv_get_deviceinfo(Sint32 device)
{
    return &_mp->info[device];
}

const KDS_PORT *mpdrv_get_portstatus(Sint32 port)
{
    return &_mp->port[port];
}

Sint32 mpdrv_stat(Sint32 device, Sint32 seqno)
{
    const struct maple_device *dev = mpdrv_get_device(device);

    if (dev->done_seq == seqno) {
        return 1;
    }
    return 0;
}

Sint32 mpdrv_ischanged(Sint32 device)
{
    const struct maple_device *dev = mpdrv_get_device(device);

    return dev->changed;
}

void mpdrv_recognize(Sint32 device)
{
    struct maple_device *dev = (struct maple_device *)mpdrv_get_device(device);

    dev->recognized = 1;
}

void mpdrv_timeout(Sint32 value)
{
    _mp->timeout = MPDRV_TIMEOUT(value);
}

Sint32 mpdrv_get_timeout(void)
{
    return (Uint16)(_mp->timeout >> MPDRV_TIMEOUT_SHIFT);
}

void mpdrv_swapmode(Sint32 value)
{
    _mp->swapmode = value & 1;
}

Sint32 mpdrv_get_swapmode(void)
{
    return _mp->swapmode;
}

void mpdrv_nopcount(Sint32 value)
{
    _mp->nopcount = value;
}

Sint32 mpdrv_get_nopcount(void)
{
    return _mp->nopcount;
}

void mpdrv_speed(Sint32 value)
{
    _mp->speed = value;
}

Sint32 mpdrv_get_speed(void)
{
    return _mp->speed;
}

void mpdrv_get_version(Sint32 *major, Sint32 *minor)
{
    if (major != NULL) {
        *major = _mp_MajorVersion;
    }
    if (minor != NULL) {
        *minor = _mp_MinorVersion;
    }
}

void mpdrv_debugmode(Sint32 value)
{
    _mp->debugmode = value;
}

Sint32 mpdrv_get_debugmode(void)
{
    return _mp->debugmode;
}

void mpdrv_dreqcount(Sint32 value)
{
    _mp->dreqcount = value;
}

Sint32 mpdrv_get_dreqcount(void)
{
    return _mp->dreqcount;
}

void mpdrv_optimize(Sint32 value)
{
    _mp->optimize = value;
    _mp->nops = value;
}

Sint32 mpdrv_get_optimize(void)
{
    return _mp->nops;
}

void *mpdrv_get_lastsendaddr(void)
{
    return _mp->last_send;
}

void mpdrv_rootplay(Sint32 value)
{
    _mp->rootplay = value;
}

Sint32 mpdrv_get_rootplay(void)
{
    return _mp->rootplay;
}

void mpdrv_gunmode(Sint32 value)
{
    _mp->gunmode = value;
}

Sint32 mpdrv_get_gunmode(void)
{
    return _mp->active_gunmode;
}

void mpdrv_autoflash(Sint32 value)
{
    _mp->autoflash = value;
}

Sint32 mpdrv_get_autoflash(void)
{
    return _mp->autoflash;
}
