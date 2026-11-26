/* Kaede, the Maple bus driver's interface. kdVbInHandler sends a frame at
 * each vertical blank, and kdDmaEndHandler0 receives the frame's responses
 * when its Maple DMA ends */
#include <kaede.h>
#include <machine.h>
#include <maple_driver.h>
#include <sg_chain.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

/* the system bus's interrupt registers: normal status, error status, and the
 * level 4 mask of normal interrupts. writing 1 to a status bit clears it */
#define INT_STATUS (*(volatile Uint32 *)0xa05f6900)
#define INT_ERROR_STATUS (*(volatile Uint32 *)0xa05f6908)
#define INT_LEVEL4_MASK (*(volatile Uint32 *)0xa05f6920)
/* the Maple bits of INT_STATUS and INT_LEVEL4_MASK */
#define INT_MAPLE_DMA_END 0x1000
#define INT_MAPLE_VBLANK_OVER 0x2000
/* INT_ERROR_STATUS's four Maple error bits, and the bit of error n */
#define INT_MAPLE_ERRORS 0xf00
#define INT_MAPLE_ERROR(n) (0x800 >> (n))

/* the video chip's registers. BORDER_COLOR fills the screen while
 * VIDEO_CONTROL has VIDEO_BLANK set */
#define BORDER_COLOR (*(volatile Uint32 *)0xa05f8040)
#define VIDEO_CONTROL (*(volatile Uint32 *)0xa05f80e8)
#define VIDEO_BLANK 0x8
/* where a light gun last saw the beam. x is in bits 0 to 9, and y in the 10
 * bits from GUN_Y_SHIFT */
#define GUN_POSITION (*(volatile Uint32 *)0xa05f80c4)
#define GUN_Y_SHIFT 16
/* bits 0 to 9: the line a vertical blank starts on */
#define SYNC_VBLANK (*(volatile Uint32 *)0xa05f80dc)
/* bits 0 to 9: the line the video output is on now */
#define SYNC_STATUS (*(volatile Uint32 *)0xa05f810c)
/* the mask of each 10-bit field above */
#define FIELD_MASK 0x3ff

/* an address's physical bits */
#define PHYSICAL_MASK 0x1fffffff
/* kdExecGun puts its table on the first TABLE_ALIGN-byte boundary past the
 * start of kdgSDCKBEnableWork. the array's GUN_WORK_WORDS words leave room
 * for it */
#define TABLE_ALIGN 32
#define GUN_WORK_WORDS 16

/* the size kdInitSystem passes mpdrv_init for each buffer */
#define BUFFER_SIZE 0xc000

/* the slots of kdgHandler */
#define HANDLERS 32
/* the slots of kdgCbTbl each device takes, one for each value of
 * _mp->received */
#define SLOTS_PER_DEVICE 2

/* failed sends in a row past which kdVbInHandler receives by force */
#define CANT_SEND_LIMIT 60

/* what the Maple DMA under way carries, as kdgDmaType holds it: a frame, or
 * kdExecGun's light gun pattern */
#define DMA_FRAME 0
#define DMA_GUN 1

/* a light gun's trigger bit, GUN_TRIGGER, lies in byte TRIGGER_BYTE of its
 * response. a pull of the trigger clears it */
#define GUN_TRIGGER 0x04
#define TRIGGER_BYTE 8

/* an fd word of a storage or an LCD holds its partition count, less 1, in
 * byte 0. byte 1 holds its block size in FD_BLOCK_UNIT-byte units, less 1 */
#define FD_BLOCK_UNIT 32

/* the type of kdgErrFunc, which Kaede calls when it meets an error. where is
 * a KD_IN_ value, and err a KD_ERR_ value. vbin_count is the count of
 * vertical blanks at the error. detail is INT_ERROR_STATUS for KD_ERR_BUS,
 * and the result of the mpdrv_ call that failed for the rest */
typedef void (*kd_err_func)(Sint32 where, Sint32 err, Sint32 vbin_count,
                            Sint32 detail);
#define KD_IN_SEND 1
#define KD_IN_RECV 2
#define KD_ERR_BUS(n) (-2 - (n))
#define KD_ERR_SEND_AGAIN (-6)
#define KD_ERR_SEND (-7)
#define KD_ERR_RECV (-8)

/* a callback slot. once device has answered the command with sequence number
 * seqno, kdRecvFrame clears func and calls it. func is null in a slot that
 * waits for nothing */
struct kd_callback {
    Sint32 device;
    Sint32 seqno;
    KD_CALLBACK func;
};

/* the member exports these functions and data, the statics aside, though no
 * header of the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void kdVbInHandler(void *arg);
void kdDmaEndHandler0(void *arg);
void kdExecGun(void);
void kdDmaEndHandler1(void *arg);
void kdVbOverHandler(void *arg);

/* the chain priorities kd_setup gives kdVbInHandler, kdDmaEndHandler0,
 * kdDmaEndHandler1 and kdVbOverHandler, in that order. a chain calls its
 * lowest priority first, so kdDmaEndHandler0 at 1 runs ahead of
 * kdDmaEndHandler1 at 64 */
const Uint32 kdgChLevel[4] = {56, 1, 64, 1};
const Sint32 kdgGunModeTimeout = 300;
static const Uint8 kd_root_devices[MPDRV_PORTS] = {KDD_DEV_A0, KDD_DEV_B0,
                                                   KDD_DEV_C0, KDD_DEV_D0};
static const char kd_no_semaphore[] = "***FATAL ERROR:Cannot get semaphore!\n";
/* Kaede's version and build time */
char *kdgBuild = "\nkd Ver 1.60 Build:Jul 17 2000 11:09:57\n";

/* syChainAddHandler's numbers for the four handlers kd_setup chains, in
 * kdgChLevel's order */
SYCHAIN kdgChID[4];
void (*kdgHandler[HANDLERS])(void); /* kdSetHandler's handlers */
KDS_DEVICE kdgDevice[KDD_DEV_MAX];
KDS_SYSTEMINFO kdgSystemInfo;
/* the function Kaede reports each error to, or null. kd_setup clears it, and
 * nothing in Kaede sets another */
kd_err_func kdgErrFunc;
Sint32 (*kdgGunCallback)(void); /* kdSetGunCallback's function */
/* kdSetCallback fills slot SLOTS_PER_DEVICE * device + _mp->received */
struct kd_callback kdgCbTbl[SLOTS_PER_DEVICE * KDD_DEV_MAX];
/* TRUE once kdInitSystem or kdInitSystemEx has started the driver. nothing
 * clears it */
Bool kdgInitialized;
/* which entry, 0 or 1, of kdgSystemInfo's send_line, recv_line and
 * vover_count the frame under way fills. kdRecvFrame flips it at each frame
 * it receives */
Uint32 kdgSurface;
/* kdgBorder holds the border color a light gun's flash replaced. kdgFlashFlag
 * is 1 while the flash shows, and 0 otherwise */
Uint32 kdgBorder;
/* @hack type: kdgFlashFlag as a Uint32 of 1 and 0, where a Bool of TRUE and
 * FALSE would do */
Uint32 kdgFlashFlag;
/* the port whose light gun kdExecGun is to read, plus 1, or 0 for none */
Uint32 kdgGunReadPort;
/* the port of the gun read kdExecGun started before the last send, or -1
 * for none */
Sint32 kdgGunLatchedPort;
/* the port of the gun read kdExecGun started since the last send, plus 1,
 * or 0 for none */
Sint32 kdgGunReqPort;
/* the last position a send read from a gun, x then y */
Sint32 kdgGunPosition[2];
Sint32 kdgGunFlashTime[2];
Uint32 kdgGunFlashCount; /* the light gun flashes kdRecvFrame has started */
Uint32 kdgDmaType; /* a DMA_ value */
/* the DMAs of kdExecGun that have ended. each sends MPDRV_PATTERN_GUN_ON
 * alone, the pattern mpdrv_sdckbenable writes */
Uint32 kdgSDCKBCount;
/* the sends that failed since the last that worked, counted as
 * kdgSystemInfo.cannot_send_count counts them. a forced receive clears it */
Uint32 kdgCantSendCount;
Uint32 kdgFatalErrorCount; /* the receives kdVbInHandler has forced */
/* room for kdExecGun's table of one header word */
Uint32 kdgSDCKBEnableWork[GUN_WORK_WORDS];
/* each port's light gun trigger bit, GUN_TRIGGER or 0. kdgTriggerNew holds
 * the bit last read. each scan of the port first copies kdgTriggerNew to
 * kdgTriggerOld */
Uint8 kdgTriggerOld[MPDRV_PORTS];
Uint8 kdgTriggerNew[MPDRV_PORTS];
/* NOLINTEND(misc-use-internal-linkage) */

/* send a frame at each vertical blank. after more than CANT_SEND_LIMIT
 * failed sends in a row, receive by force. a receive refuses while
 * _mp->received is 1 or the semaphore is held. so the handler first clears
 * _mp->received and frees the semaphore */
void kdVbInHandler(void *arg)
{
    kdgSystemInfo.vbin_count++;
    kdSendFrame();
    if (kdgCantSendCount > CANT_SEND_LIMIT) {
        _mp->received = 0;
        *_mp_SemaphoreAddress = 0;
        kdRecvFrame();
        kdgCantSendCount = 0;
        kdgFatalErrorCount++;
    }
}

/* when a frame's Maple DMA ends, receive the frame and call kdgHandler's
 * handlers. kdExecGun starts a light gun's read between the two. when
 * kdExecGun's DMA ends, only count it */
void kdDmaEndHandler0(void *arg)
{
    Sint32 slot;
    /* @hack spelling: handler, a local for the slot's handler, not called in
     * place */
    void (*handler)(void);

    set_imask(IMASK_ALL);
    if (kdgDmaType == DMA_FRAME) {
        kdgSystemInfo.dmaend_count++;
        kdRecvFrame();
        kdExecGun();
        for (slot = 0; slot < HANDLERS; slot++) {
            if (kdgHandler[slot] != NULL) {
                handler = kdgHandler[slot];
                handler();
            }
        }
    } else {
        kdgDmaType = DMA_FRAME;
        kdgSDCKBCount++;
    }
}

/* run kdDmaEndHandler0, then kdVbInHandler, as a frame's Maple DMA end and
 * the next vertical blank would */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
void kd_dma_end_then_vblank(void) /* NOLINT(misc-use-internal-linkage) */
{
    kdDmaEndHandler0((void *)1);
    kdVbInHandler((void *)1);
}

/* for the light gun kdgGunReadPort names, start a Maple DMA of the port's
 * MPDRV_PATTERN_GUN_ON alone. the pattern lets the gun latch GUN_POSITION.
 * then queue a get condition for the root device's controller function in
 * the frame being built */
void kdExecGun(void)
{
    Sint32 port;
    Uint32 table;
    /* @hack spelling: command, a local for the table's uncached address, not
     * written through the cast in place */
    Uint32 *command;
    Uint32 func;

    if (kdgGunReadPort == 0) {
        return;
    }
    port = kdgGunReadPort - 1;
    table =
        ((Uint32)kdgSDCKBEnableWork & PHYSICAL_MASK & ~(TABLE_ALIGN - 1UL)) +
        TABLE_ALIGN;
    command = (Uint32 *)(table | P2_BASE);
    *command =
        MPDRV_LAST_HEADER | MPDRV_PATTERN_GUN_ON | MPDRV_HEADER_PORT(port);
    kdgDmaType = DMA_GUN;
    _mp->reg[MPDRV_REG_DMA_TABLE] = table;
    _mp->reg[MPDRV_REG_DMA_START] = 1;
    func = KDD_FUNC_CONTROLLER;
    mpdrv_setcmd_force(port * MPDRV_PORT_DEVICES, MPDRV_CMD_GETCOND, &func, 1,
                       NULL, 0);
    kdgGunReqPort = kdgGunReadPort;
}

/* clear the Maple DMA end interrupt, once kdDmaEndHandler0 has run on the
 * same chain */
void kdDmaEndHandler1(void *arg)
{
    INT_STATUS = INT_MAPLE_DMA_END;
}

/* count a vertical blank that came during a frame's transfer, and clear its
 * interrupt */
void kdVbOverHandler(void *arg)
{
    kdgSystemInfo.total_vover_count++;
    kdgSystemInfo.vover_count[kdgSurface]++;
    INT_STATUS = INT_MAPLE_VBLANK_OVER;
}

/* copy the driver's device state to kdgDevice, for kdGetDevice */
static void kd_publish_devices(void)
{
    Sint32 i;
    KDS_DEVICE *dev = kdgDevice;
    const struct maple_device *device = _mp->device;

    /* @hack spelling: i, a count that ends the loop beside dev and device,
     * where dev < kdgDevice + KDD_DEV_MAX would do */
    for (i = 0; i < KDD_DEV_MAX; i++, dev++, device++) {
        dev->on = device->on;
        dev->old = device->old;
        dev->connect = device->connect;
        dev->remove = device->remove;
        /* @hack spelling: recvdata copied ahead of response, dst_ap, src_ap
         * and datasize, not in the fields' order */
        dev->recvdata = device->recvdata;
        dev->response = device->response;
        dev->dst_ap = device->dst_ap;
        dev->src_ap = device->src_ap;
        dev->datasize = device->datasize;
        dev->errcode = device->errcode;
    }
}

/* chain the four handlers, and reset Kaede's globals for kdInitSystem and
 * kdInitSystemEx. it leaves kdgInitialized, kdgGunCallback, kdgGunPosition
 * and the trigger bits as they are */
static void kd_setup(void)
{
    kdgChID[0] = syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK, kdVbInHandler,
                                   kdgChLevel[0], NULL);
    kdgChID[1] = syChainAddHandler(SYD_CHAIN_EVENT_IML4_MPDMAE,
                                   kdDmaEndHandler0, kdgChLevel[1], NULL);
    kdgChID[2] = syChainAddHandler(SYD_CHAIN_EVENT_IML4_MPDMAE,
                                   kdDmaEndHandler1, kdgChLevel[2], NULL);
    kdgChID[3] = syChainAddHandler(SYD_CHAIN_EVENT_IML4_MPVOVR, kdVbOverHandler,
                                   kdgChLevel[3], NULL);
    kdgErrFunc = NULL;
    kdgSurface = 0;
    kdgBorder = 0;
    kdgFlashFlag = 0;
    kdgGunReadPort = 0;
    /* @hack spelling: kdgGunReqPort cleared ahead of kdgGunLatchedPort, not
     * in their declared order */
    kdgGunReqPort = 0;
    kdgGunLatchedPort = -1;
    kdgGunFlashCount = 0;
    kdgDmaType = DMA_FRAME;
    kdgSDCKBCount = 0;
    kdgCantSendCount = 0;
    kdgFatalErrorCount = 0;
    memset(&kdgSystemInfo, 0, sizeof(kdgSystemInfo));
    memset(kdgDevice, 0, sizeof(kdgDevice));
    memset(kdgCbTbl, 0, sizeof(kdgCbTbl));
    memset(kdgHandler, 0, sizeof(kdgHandler));
}

Sint32 kdInitSystem(void *recvbuf, void *sendbuf)
{
    Uint32 mask = get_imask();
    Sint32 err;

    set_imask(IMASK_ALL);
    _mp_SemaphoreAddress = (char *)((Uint32)_mp_SemaphoreAddress | P2_BASE);
    kd_setup();
    err = mpdrv_init(sendbuf, BUFFER_SIZE, recvbuf, BUFFER_SIZE);
    kd_publish_devices();
    kdgInitialized = TRUE;
    set_imask(mask);
    return err;
}

Sint32 kdInitSystemEx(Sint32 *devmap, void **recvbuf, void *sendbuf, Sint32 num)
{
    Uint32 mask = get_imask();
    Sint32 err;

    set_imask(IMASK_ALL);
    _mp_SemaphoreAddress = (char *)((Uint32)_mp_SemaphoreAddress | P2_BASE);
    kd_setup();
    err = mpdrv_init_ex(devmap, recvbuf, sendbuf, num);
    kd_publish_devices();
    kdgInitialized = TRUE;
    set_imask(mask);
    return err;
}

Sint32 kdSendFrame(void)
{
    Sint32 err;

    if (!kdgInitialized) {
        return 0;
    }
    kdgGunLatchedPort = kdgGunReqPort - 1;
    kdgGunReqPort = 0;
    if (kdgGunReadPort != 0) {
        kdgGunPosition[0] = GUN_POSITION & FIELD_MASK;
        /* @hack type: y's half cast to Sint16 ahead of the mask, where
         * (GUN_POSITION >> GUN_Y_SHIFT) & FIELD_MASK would do */
        kdgGunPosition[1] = (Sint16)(GUN_POSITION >> GUN_Y_SHIFT) & FIELD_MASK;
    }
    kdgGunReadPort = 0;
    if (kdgFlashFlag != 0) {
        BORDER_COLOR = kdgBorder;
        VIDEO_CONTROL &= ~VIDEO_BLANK;
        kdgFlashFlag = 0;
    }
    if (INT_ERROR_STATUS & INT_MAPLE_ERRORS) {
        if (INT_ERROR_STATUS & INT_MAPLE_ERROR(0)) {
            kdgSystemInfo.error_count[0]++;
            if (kdgErrFunc != NULL) {
                kdgErrFunc(KD_IN_SEND, KD_ERR_BUS(0), kdgSystemInfo.vbin_count,
                           INT_ERROR_STATUS);
            }
        }
        if (INT_ERROR_STATUS & INT_MAPLE_ERROR(1)) {
            kdgSystemInfo.error_count[1]++;
            if (kdgErrFunc != NULL) {
                kdgErrFunc(KD_IN_SEND, KD_ERR_BUS(1), kdgSystemInfo.vbin_count,
                           INT_ERROR_STATUS);
            }
        }
        if (INT_ERROR_STATUS & INT_MAPLE_ERROR(2)) {
            kdgSystemInfo.error_count[2]++;
            if (kdgErrFunc != NULL) {
                kdgErrFunc(KD_IN_SEND, KD_ERR_BUS(2), kdgSystemInfo.vbin_count,
                           INT_ERROR_STATUS);
            }
        }
        if (INT_ERROR_STATUS & INT_MAPLE_ERROR(3)) {
            kdgSystemInfo.error_count[3]++;
            if (kdgErrFunc != NULL) {
                kdgErrFunc(KD_IN_SEND, KD_ERR_BUS(3), kdgSystemInfo.vbin_count,
                           INT_ERROR_STATUS);
            }
        }
        INT_ERROR_STATUS = INT_MAPLE_ERRORS;
        kdRecvFrame();
        err = mpdrv_frame_send();
        if (err >= 0) {
            kd_publish_devices();
        } else if (kdgErrFunc != NULL) {
            kdgErrFunc(KD_IN_SEND, KD_ERR_SEND_AGAIN, kdgSystemInfo.vbin_count,
                       err);
        }
    } else {
        err = mpdrv_frame_send();
        if (err >= 0) {
            kdgDmaType = DMA_FRAME;
            kdgSystemInfo.vover_count[kdgSurface] = 0;
            kdgSystemInfo.send_line[kdgSurface] = SYNC_STATUS & FIELD_MASK;
            kd_publish_devices();
            kdgSystemInfo.send_count++;
            kdgCantSendCount = 0;
        } else {
            kdRecvFrame();
            kdgSystemInfo.cannot_send_count++;
            kdgCantSendCount++;
            if (kdgErrFunc != NULL) {
                kdgErrFunc(KD_IN_SEND, KD_ERR_SEND, kdgSystemInfo.vbin_count,
                           err);
            }
        }
    }
    return err;
}

/* scan the triggers of the ports in light gun mode. return 1 plus the first
 * port whose trigger bit has cleared since the bit was last read. return 0
 * for none. the ports after that one wait for the next call */
static Sint32 kd_scan_triggers(void)
{
    Sint32 port;
    Uint8 trigger;
    /* @hack spelling: changed, a local for the trigger bit's change, not
     * tested in place */
    Uint8 changed;
    /* @hack type: ports declared Uint8, where a Sint32 would do */
    Uint8 ports;
    const KDS_DEVICE *dev;
    const KDS_DEVICEINFO *info;
    const Uint8 *response;

    /* @hack spelling: ports, a copy of _mp->active_gunmode, not read in
     * place */
    ports = _mp->active_gunmode;
    for (port = 0; port < MPDRV_PORTS; port++) {
        if (!(ports & (1 << port))) {
            continue;
        }
        dev = kdGetDevice(kd_root_devices[port]);
        info = kdGetDeviceInfo(kd_root_devices[port]);
        kdgTriggerOld[port] = kdgTriggerNew[port];
        if ((info->function & KDD_FUNC_LIGHTGUN) && !dev->connect &&
            dev->response == KDD_RES_DATATRANSFER) {
            /* @hack spelling: response stepped to byte TRIGGER_BYTE and read
             * there, not as response[TRIGGER_BYTE] */
            response = dev->recvdata;
            response += TRIGGER_BYTE;
            trigger = *response & GUN_TRIGGER;
            kdgTriggerNew[port] = trigger;
            changed = kdgTriggerOld[port] ^ trigger;
            if (!trigger && changed) {
                return port + 1;
            }
        }
    }
    return 0;
}

Sint32 kdRecvFrame(void)
{
    Uint32 mask = get_imask();
    Sint32 err;
    Sint32 i;
    struct kd_callback *callback;
    KD_CALLBACK func;

    set_imask(IMASK_ALL);
    err = mpdrv_frame_recv_pre();
    if (err == MPDRV_SEMAPHORE_TAKEN) {
        _mp->recv_pending = 1;
    } else if (err >= 0) {
        mpdrv_frame_recv_main();
        kdgSystemInfo.recv_line[kdgSurface] =
            (SYNC_STATUS & FIELD_MASK) +
            (kdgSystemInfo.vover_count[kdgSurface] *
             (SYNC_VBLANK & FIELD_MASK));
        kd_publish_devices();
        mpdrv_frame_recv_post();
        if (mpdrv_get_gunmode()) {
            /* @hack spelling: the scan written in both arms of a test of
             * kdgGunCallback, where kdgGunCallback == NULL ||
             * kdgGunCallback() would do */
            if (kdgGunCallback != NULL) {
                if (kdgGunCallback()) {
                    kdgGunReadPort = kd_scan_triggers();
                }
            } else {
                kdgGunReadPort = kd_scan_triggers();
            }
            /* @hack spelling: the flash's two tests nested, not joined by
             * && */
            if (_mp->autoflash != 0) {
                if (kdgGunReadPort != 0) {
                    kdgBorder = BORDER_COLOR;
                    BORDER_COLOR = _mp->autoflash;
                    VIDEO_CONTROL |= VIDEO_BLANK;
                    kdgFlashFlag = 1;
                    kdgGunFlashCount++;
                }
            }
        }
        callback = kdgCbTbl;
        /* @hack spelling: i, a count that ends the loop beside callback,
         * where callback < kdgCbTbl + SLOTS_PER_DEVICE * KDD_DEV_MAX would
         * do */
        for (i = 0; i < SLOTS_PER_DEVICE * KDD_DEV_MAX; i++, callback++) {
            if (callback->func != NULL &&
                kdGetStatus(callback->device, callback->seqno)) {
                func = callback->func;
                callback->func = NULL;
                func(callback->device, callback->seqno,
                     (KDS_DEVICE *)kdGetDevice(callback->device),
                     (KDS_DEVICEINFO *)kdGetDeviceInfo(callback->device));
            }
        }
        _mp->recv_pending = 0;
        kdgSurface ^= 1;
        kdgSystemInfo.recv_count++;
    } else if (kdgErrFunc != NULL) {
        kdgErrFunc(KD_IN_RECV, KD_ERR_RECV, kdgSystemInfo.vbin_count, err);
    }
    set_imask(mask);
    return err;
}

void kdExitSystem(void)
{
    Uint32 mask = get_imask();

    set_imask(IMASK_ALL);
    INT_LEVEL4_MASK &= ~(INT_MAPLE_DMA_END | INT_MAPLE_VBLANK_OVER);
    syChainDeleteHandler(kdgChID[3]);
    /* @bug memory: reads kdgChID[4], past the array's end, where kdgChID[0]
     * was meant. syChainDeleteHandler takes the next word, kdgHandler[0], as
     * a chain number, and kdVbInHandler stays chained. MOV.L @(16,R14),R4 at
     * 0x8c1364e6 reads that word, and no load reads kdgChID[0] */
#ifdef NONMATCHING
    /* this side keeps the result of the bug tagged above: it passes
     * kdgHandler[0], the word the image reads, as a chain number. GCC need not
     * place kdgHandler right after kdgChID */
    syChainDeleteHandler((SYCHAIN)kdgHandler[0]);
#else
    syChainDeleteHandler(
        kdgChID[4]); /* NOLINT(clang-diagnostic-array-bounds) */
#endif
    syChainDeleteHandler(kdgChID[2]);
    syChainDeleteHandler(kdgChID[1]);
    mpdrv_exit();
    memset(kdgDevice, 0, sizeof(kdgDevice));
    set_imask(mask);
}

const KDS_DEVICE *kdGetDevice(Sint32 device)
{
    return &kdgDevice[device];
}

const KDS_DEVICEINFO *kdGetDeviceInfo(Sint32 device)
{
    /* @hack spelling: the info through a local, not returned in place */
    const KDS_DEVICEINFO *info = mpdrv_get_deviceinfo(device);

    return info;
}

const KDS_PORT *kdGetPortStatus(Sint32 port)
{
    /* @hack spelling: the status through a local, not returned in place */
    const KDS_PORT *status = mpdrv_get_portstatus(port);

    return status;
}

Sint32 kdGetStatus(Sint32 device, Sint32 seqno)
{
    return mpdrv_stat(device, seqno);
}

Sint32 kdSetDeviceRequest(Sint32 device)
{
    Sint32 seqno = mpdrv_set_dreq(device);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetAllStatusRequest(Sint32 device)
{
    Sint32 seqno = mpdrv_set_allstatreq(device);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetDeviceReset(Sint32 device)
{
    Sint32 seqno = mpdrv_set_dreset(device);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetDeviceKill(Sint32 device)
{
    Sint32 seqno = mpdrv_set_dkill(device);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetGetCondition(Sint32 device, Sint32 func)
{
    Sint32 seqno = mpdrv_set_getcond(device, func);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetGetMediaInfo(Sint32 device, Sint32 func, Sint32 pt)
{
    Sint32 seqno = mpdrv_set_getminfo(device, func, pt);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetBlockRead(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                      Sint32 block)
{
    Sint32 seqno = mpdrv_set_bread(device, func, pt, phase, block);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetBlockWrite(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                       Sint32 block, const void *src, Sint32 size)
{
    Sint32 seqno = mpdrv_set_bwrite(device, func, pt, phase, block, src, size);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetGetLastError(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                         Sint32 block)
{
    Sint32 seqno = mpdrv_set_getlerr(device, func, pt, phase, block);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetARControl(Sint32 device, const void *buf)
{
    Sint32 seqno = mpdrv_set_arctrl(device, buf);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetFT4Control(Sint32 device, const void *buf)
{
    Sint32 seqno = mpdrv_set_ft4ctrl(device, buf);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetSetCondition(Sint32 device, Sint32 func, const void *buf,
                         Sint32 size)
{
    Sint32 seqno = mpdrv_set_setcond(device, func, buf, size);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetTransmitAgain(Sint32 device)
{
    Sint32 seqno = mpdrv_set_transagain(device);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetResetPattern(Sint32 port)
{
    Sint32 seqno = mpdrv_set_hreset(port);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

void kdGetVersion(Sint32 *major, Sint32 *minor)
{
    mpdrv_get_version(major, minor);
}

Sint32 kdIsChanged(Sint32 device)
{
    return mpdrv_ischanged(device);
}

void kdRecognizeChanged(Sint32 device)
{
    mpdrv_recognize(device);
}

const Uint8 *kdDevGetFD(const KDS_DEVICEINFO *dev, Uint32 func)
{
    Sint32 word = 0;
    Uint32 bit;
    Sint32 i;
    const Uint8 *fd = dev->fd[0];
    Uint32 function = dev->function;

    /* @hack spelling: i, a count that ends the loop beside bit, where bit !=
     * 0 would do */
    for (i = 0, bit = 0x80000000; i < 32; i++, bit >>= 1) {
        if (function & bit) {
            if (func == bit) {
                return fd;
            }
            word++;
            fd = dev->fd[word];
        }
    }
    return NULL;
}

Sint32 kdDevGetControllerInfo(KDS_CONTROLLERINFO *sinfo,
                              const KDS_DEVICEINFO *info)
{
    const Uint8 *fd = kdDevGetFD(info, KDD_FUNC_CONTROLLER);

    if (fd == NULL) {
        return -1;
    }
    sinfo->button = ((Uint32)fd[0] << 24) | ((Uint32)fd[1] << 16) |
                    ((Uint32)fd[2] << 8) | fd[3];
    return 0;
}

Sint32 kdDevGetStorageInfo(KDS_STORAGEINFO *sinfo, const KDS_DEVICEINFO *info)
{
    const Uint8 *fd = kdDevGetFD(info, KDD_FUNC_STORAGE);

    if (fd == NULL) {
        return -1;
    }
    sinfo->partition = fd[0] + 1;
    sinfo->blocksize = (fd[1] + 1) * FD_BLOCK_UNIT;
    sinfo->writecount = (fd[2] >> 4) & 15;
    sinfo->readcount = fd[2] & 15;
    sinfo->removable = (fd[3] >> 7) & 1;
    return 0;
}

Sint32 kdDevGetSIPInfo(KDS_SIPINFO *sinfo, const KDS_DEVICEINFO *info)
{
    const Uint8 *fd = kdDevGetFD(info, KDD_FUNC_SOUNDINPUT);

    if (fd == NULL) {
        return -1;
    }
    sinfo->function = ((Uint32)fd[0] << 24) | ((Uint32)fd[1] << 16) |
                      ((Uint32)fd[2] << 8) | fd[3];
    return 0;
}

Sint32 kdDevGetLCDInfo(KDS_LCDINFO *sinfo, const KDS_DEVICEINFO *info)
{
    const Uint8 *fd = kdDevGetFD(info, KDD_FUNC_LCD);

    if (fd == NULL) {
        return -1;
    }
    sinfo->partition = fd[0] + 1;
    sinfo->blocksize = (fd[1] + 1) * FD_BLOCK_UNIT;
    sinfo->writecount = (fd[2] >> 4) & 15;
    sinfo->direction = (fd[3] >> 6) & 3;
    sinfo->normally = (fd[3] >> 5) & 1;
    return 0;
}

Sint32 kdDevGetTimerInfo(KDS_TIMERINFO *sinfo, const KDS_DEVICEINFO *info)
{
    const Uint8 *fd = kdDevGetFD(info, KDD_FUNC_TIMER);

    if (fd == NULL) {
        return -1;
    }
    sinfo->write_timer = fd[0];
    sinfo->read_timer = fd[1];
    sinfo->button = fd[2];
    sinfo->alarm = fd[3];
    return 0;
}

Sint32 kdDevGetGunInfo(KDS_GUNINFO *sinfo, const KDS_DEVICEINFO *info)
{
    const Uint8 *fd = kdDevGetFD(info, KDD_FUNC_ARGUN);

    if (fd == NULL) {
        return -1;
    }
    sinfo->button = ((Uint32)fd[0] << 24) | ((Uint32)fd[1] << 16) |
                    ((Uint32)fd[2] << 8) | fd[3];
    return 0;
}

Sint32 kdDevGetVibrationInfo(KDS_VIBRATIONINFO *sinfo,
                             const KDS_DEVICEINFO *info)
{
    const Uint8 *fd = kdDevGetFD(info, KDD_FUNC_VIBRATION);

    if (fd == NULL) {
        return -1;
    }
    sinfo->vn = fd[0];
    sinfo->se = fd[1];
    return 0;
}

Sint32 kdSetDriverMode(Sint32 type, Sint32 mode)
{
    /* @hack spelling: the cases in the order 1, 2, 4, 3, 6, 5, 0, 7, 8 and
     * 9, not in numeric order */
    switch (type) {
    case KDD_MODE_NOPCOUNT:
        mpdrv_nopcount(mode);
        break;
    case KDD_MODE_TIMEOUT:
        mpdrv_timeout(mode);
        break;
    case KDD_MODE_DREQCOUNT:
        mpdrv_dreqcount(mode);
        break;
    case KDD_MODE_SPEED:
        mpdrv_speed(mode);
        break;
    case KDD_MODE_DEBUG:
        mpdrv_debugmode(mode);
        break;
    case KDD_MODE_DEVENDIAN:
        mpdrv_swapmode(mode);
        break;
    case KDD_MODE_OPTIMIZE:
        mpdrv_optimize(mode);
        break;
    case KDD_MODE_ROOTPLAY:
        mpdrv_rootplay(mode);
        break;
    case KDD_MODE_GUNMODE:
        mpdrv_gunmode(mode);
        break;
    case KDD_MODE_AUTOFLASH:
        mpdrv_autoflash(mode);
        break;
    default:
        return -1;
    }
    return 0;
}

Sint32 kdGetDriverMode(Sint32 type)
{
    /* @hack spelling: the cases in the order 1, 2, 4, 3, 6, 5, 0, 7, 8 and
     * 9, not in numeric order */
    switch (type) {
    case KDD_MODE_NOPCOUNT:
        return mpdrv_get_nopcount();
    case KDD_MODE_TIMEOUT:
        return mpdrv_get_timeout();
    case KDD_MODE_DREQCOUNT:
        return mpdrv_get_dreqcount();
    case KDD_MODE_SPEED:
        return mpdrv_get_speed();
    case KDD_MODE_DEBUG:
        return mpdrv_get_debugmode();
    case KDD_MODE_DEVENDIAN:
        return mpdrv_get_swapmode();
    case KDD_MODE_OPTIMIZE:
        return mpdrv_get_optimize();
    case KDD_MODE_ROOTPLAY:
        return mpdrv_get_rootplay();
    case KDD_MODE_GUNMODE:
        return mpdrv_get_gunmode();
    case KDD_MODE_AUTOFLASH:
        return mpdrv_get_autoflash();
    default:
        return -1;
    }
}

const KDS_SYSTEMINFO *kdGetSystemInfo(void)
{
    return &kdgSystemInfo;
}

void kdGetGunPosition(Sint32 *x, Sint32 *y)
{
    *x = kdgGunPosition[0];
    *y = kdgGunPosition[1];
}

Sint32 kdGetGunLatchedPort(void)
{
    return kdgGunLatchedPort;
}

void kdSetGunTrigger(Uint32 port)
{
    kdgGunReadPort = port + 1;
}

void kdSetGunCallback(Sint32 (*func)(void))
{
    kdgGunCallback = func;
}

void kdSetHandler(Uint32 no, void (*handler)(void))
{
    /* @hack spelling: the test written as no <= HANDLERS - 1, where
     * no < HANDLERS would do */
    if (no <= HANDLERS - 1) {
        kdgHandler[no] = handler;
    }
}

Sint32 kdSetCallback(Sint32 device, KD_CALLBACK func, Sint32 seqno)
{
    struct kd_callback *callback =
        &kdgCbTbl[(device * SLOTS_PER_DEVICE) + _mp->received];

    callback->func = func;
    callback->seqno = seqno;
    callback->device = device;
    return 0;
}

Sint32 kdSetDeviceRequestEx(Sint32 device, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_dreq_ns(device);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetAllStatusRequestEx(Sint32 device, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_allstatreq_ns(device);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetDeviceResetEx(Sint32 device, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_dreset_ns(device);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetDeviceKillEx(Sint32 device, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_dkill_ns(device);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetGetConditionEx(Sint32 device, Sint32 func, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_getcond_ns(device, func);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetGetMediaInfoEx(Sint32 device, Sint32 func, Sint32 pt,
                           KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_getminfo_ns(device, func, pt);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetBlockReadEx(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                        Sint32 block, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_bread_ns(device, func, pt, phase, block);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetBlockWriteEx(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                         Sint32 block, const void *src, Sint32 size,
                         KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_bwrite_ns(device, func, pt, phase, block, src, size);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetGetLastErrorEx(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                           Sint32 block, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_getlerr_ns(device, func, pt, phase, block);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetARControlEx(Sint32 device, const void *buf, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_arctrl_ns(device, buf);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetFT4ControlEx(Sint32 device, const void *buf, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_ft4ctrl_ns(device, buf);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetSetConditionEx(Sint32 device, Sint32 func, const void *buf,
                           Sint32 size, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_setcond_ns(device, func, buf, size);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}
