/* Maple bus driver under Kaede, as far as the sources use it */
#ifndef MAPLE_DRIVER_H
#define MAPLE_DRIVER_H
#include <kaede.h>
#include <sg_xpt.h>

/* the console's controller ports, A to D */
#define MPDRV_PORTS 4
/* how many device numbers each port takes. its root device takes the first,
 * and the devices in its expansion sockets the rest */
#define MPDRV_PORT_DEVICES (KDD_DEV_MAX / MPDRV_PORTS)

/* standing commands. each frame sends every enabled device its standing
 * command, unless another is queued for it. MPDRV_CMD_NONE sends nothing */
#define MPDRV_CMD_NONE 0
#define MPDRV_CMD_DREQ 1
#define MPDRV_CMD_ALLSTATREQ 2
#define MPDRV_CMD_DRESET 3
#define MPDRV_CMD_DKILL 4
#define MPDRV_CMD_GETCOND 9
/* the standing command of an expansion device the driver takes as absent.
 * a frame sends such a device nothing. a device goes absent while its socket
 * is empty, and after a frame sends it MPDRV_CMD_DKILL. it also goes absent
 * once its retries pass _mp->dreqcount */
#define MPDRV_CMD_ABSENT 0xff
/* commands that only their own mpdrv_set_ calls queue, never standing */
#define MPDRV_CMD_GETMINFO 10
#define MPDRV_CMD_BREAD 11
#define MPDRV_CMD_BWRITE 12
#define MPDRV_CMD_GETLERR 13
#define MPDRV_CMD_SETCOND 14
#define MPDRV_CMD_FT4CTRL 15
#define MPDRV_CMD_ARCTRL 16
#define MPDRV_CMD_CAMERA 17
#define MPDRV_CMD_TRANSAGAIN 0xfc

/* a frame is a run of commands and patterns, each opened by a header word.
 * a header holds its port in two bits from MPDRV_PORT_SHIFT. its pattern
 * lies in three bits from MPDRV_PATTERN_SHIFT. pattern 0 opens a command.
 * its header's low byte counts the words after the command word. the
 * response address, the command word and those words follow the header.
 * each pattern below, MPDRV_PATTERN_GUN_ON to MPDRV_PATTERN_NOP, is a header
 * alone. MPDRV_LAST_HEADER marks the frame's last header */
#define MPDRV_PORT_SHIFT 16
#define MPDRV_PATTERN_SHIFT 8
#define MPDRV_HEADER_PORT(port) ((port) << MPDRV_PORT_SHIFT)
#define MPDRV_PATTERN_GUN_ON (2 << MPDRV_PATTERN_SHIFT)
#define MPDRV_PATTERN_RESET (3 << MPDRV_PATTERN_SHIFT)
#define MPDRV_PATTERN_GUN_OFF (4 << MPDRV_PATTERN_SHIFT)
#define MPDRV_PATTERN_NOP (7 << MPDRV_PATTERN_SHIFT)
#define MPDRV_LAST_HEADER 0x80000000

/* a device as the driver keeps it */
struct maple_device {
    Uint8 on;
    Uint8 old;
    Uint8 connect;
    Uint8 remove;
    /* MPDRV_CMD_ codes. command is the standing command, and old_command its
     * value before the last response. queued is the command queued for the
     * next frame, and sent the one the last frame sent. each is
     * MPDRV_CMD_NONE for none */
    Uint8 command;
    Uint8 old_command;
    Uint8 queued;
    Uint8 sent;
    Uint8 response;
    Uint8 dst_ap;
    Uint8 src_ap;
    Uint8 datasize;
    Uint8 changed; /* a response showed it connected or removed */
    /* set by mpdrv_recognize. the next response clears it and changed */
    Uint8 recognized;
    Uint8 unk_0e[2];
    void *recvdata;
    Sint32 errcode;
    /* queued_seq is the queued command's sequence number, and sent_seq the sent
     * command's. done_seq is that of the last command whose frame was
     * received */
    Sint32 queued_seq;
    Sint32 sent_seq;
    Sint32 done_seq;
    /* an expansion device's retries. a socket newly in use adds 1, and so
     * does each MPDRV_CMD_DREQ whose response isn't KDD_RES_DEVICESTATUS.
     * past _mp->dreqcount, the device goes MPDRV_CMD_ABSENT and the count
     * clears. the count also clears when the socket empties, and after a
     * frame sends MPDRV_CMD_DKILL */
    Uint32 retries;
    Uint8 unk_28[4];
};

/* driver state. a four-character tag starts each part: 'PORT', 'DEV ', 'INFO',
 * 'ENBL', 'STAT', 'RADR', 'TOUT', 'DRQC', 'FUNC' and 'END ' */
struct mp_context {
    Uint32 tag_port;
    KDS_PORT port[MPDRV_PORTS];
    Uint32 tag_dev;
    struct maple_device device[KDD_DEV_MAX];
    Uint32 tag_info;
    KDS_DEVICEINFO info[KDD_DEV_MAX];
    Uint32 tag_enbl;
    /* nonzero for each device the driver polls. only these get standing
     * commands, and the driver reads only their responses. mpdrv_init_globals
     * writes 'R' for a port's root device and 'E' for an expansion device */
    char enable[KDD_DEV_MAX];
    Uint32 tag_stat;
    /* the ports in light gun mode, a bit for each. gunmode holds the ports
     * mpdrv_gunmode asks for. active_gunmode holds the ports in effect, and
     * mpdrv_get_gunmode returns it. where the two differ, mpdrv_frame_send
     * copies gunmode to active_gunmode and sets the bus's timeout to match */
    Sint32 gunmode;
    Sint32 active_gunmode;
    Uint8 unk_fb4[8];
    /* the color the screen flashes before a light gun is read, or 0 for no
     * flash */
    Sint32 autoflash;
    Sint32 received; /* 0 from a frame's send until its receive, and 1 after */
    /* the buffers mpdrv_init was given, sendbuf to send frames from and
     * recvbuf to receive responses into. each address is rounded up to a
     * multiple of 32, and reached through P2, the uncached area */
    Uint32 sendbuf;
    Uint32 recvbuf;
    /* the first word of the command, pattern or run of NOPs written last to
     * the frame being built. last_cmd_words counts its words, and the next is
     * written after them */
    Uint32 *last_cmd;
    Uint8 unk_fd0[8];
    Sint32 last_cmd_words;
    Sint32 send_size; /* the bytes of the frame being built */
    /* the next command's sequence number, which counts from 1 to 0x7fffffff
     * and wraps to 1 */
    Sint32 next_seq;
    Uint32 tag_radr;
    /* each device's two response areas, one for each value of recv_side. a
     * command takes the area of the recv_side it is queued under.
     * mpdrv_init_globals gives each area 1 KB of recvbuf. recv_side 0's areas
     * fill its first 24 KB, and recv_side 1's the next */
    Uint32 *recv_data[KD_FRAMES][KDD_DEV_MAX];
    /* the frame buffers: one is built while the other is sent */
    Uint8 *send_frame[KD_FRAMES];
    void *last_send;
    Sint32 recv_side;
    Sint32 send_side;
    Uint32 tag_tout;
    /* the bus's timeout, as the Maple system register holds it: the count
     * from MPDRV_TIMEOUT_SHIFT up */
    Sint32 timeout;
    /* which words the driver byte-reverses, KDD_MODE_DEVENDIAN's setting.
     * MPDRV_SWAP_DATA reverses a command's data words. MPDRV_SWAP_COMMAND
     * reverses the command word and the extra words, and reads each word of
     * a response reversed */
    Sint32 swapmode;
    Sint32 debugmode;
    /* the NOPs that go ahead of a command where the frame's last header names
     * the command's port */
    Sint32 nopcount;
    Sint32 speed;
    Uint32 tag_drqc;
    /* the retries past which an expansion device goes MPDRV_CMD_ABSENT */
    Uint32 dreqcount;
    /* how many frames in a row a port's root device can leave unanswered
     * before the port reads as empty. the port's playcount counts them */
    Uint32 rootplay;
    /* KDD_MODE_OPTIMIZE's setting. mpdrv_optimize writes it to optimize and
     * to nops, and the driver reads only nops. each frame gets nops NOPs after
     * its queued commands, ahead of its standing commands */
    Sint32 optimize;
    Sint32 nops;
    /* kdRecvFrame sets recv_pending when it finds the semaphore held, and
     * its next receive clears it. while it is set, each of Kaede's command
     * calls runs kdRecvFrame before it returns. an Ex call that finds the
     * semaphore held returns before that */
    Bool recv_pending;
    Uint32 unk_10ec;
    Uint32 tag_func;
    volatile Sint32 *reg; /* the Maple bus registers */
    Uint32 tag_end;
};
/* swapmode's settings */
#define MPDRV_SWAP_DATA 0
#define MPDRV_SWAP_COMMAND 1
/* indexes into reg, each the word of the Maple bus register it names */
/* the address of the table the DMA sends: a frame, or a pattern alone */
#define MPDRV_REG_DMA_TABLE 1
/* 0 lets a write to MPDRV_REG_DMA_START start the DMA */
#define MPDRV_REG_DMA_TRIGGER 4
#define MPDRV_REG_DMA_ENABLE 5 /* 1 enables the DMA */
/* a 1 written starts the DMA, and the register reads 1 until the DMA ends */
#define MPDRV_REG_DMA_START 6
#define MPDRV_REG_SYSTEM 32 /* the bus's timeout and speed */
#define MPDRV_REG_DMA_RANGE 35 /* the addresses the DMA may write */
/* mp_context's timeout and the MPDRV_REG_SYSTEM register hold the bus's
 * timeout count in their bits from MPDRV_TIMEOUT_SHIFT up. a count is
 * MPDRV_TIMEOUT_UNIT nanoseconds. the driver starts at MPDRV_START_TIMEOUT, a
 * millisecond. mpdrv_frame_send goes back to it once no port is in light gun
 * mode */
#define MPDRV_TIMEOUT_SHIFT 16
#define MPDRV_TIMEOUT(count) ((Uint32)(count) << MPDRV_TIMEOUT_SHIFT)
#define MPDRV_TIMEOUT_UNIT 20
#define MPDRV_START_TIMEOUT 50000
/* the bytes of each response area in recv_data */
#define MPDRV_RESPONSE_SIZE 0x400
/* MPDRV_DMA_BUSY is a frame's send or receive while the Maple DMA runs, and
 * MPDRV_NOT_READY a send while a frame waits to be received or a receive
 * while none does. MPDRV_SEMAPHORE_TAKEN is a call that finds the semaphore
 * held */
#define MPDRV_DMA_BUSY (-1)
#define MPDRV_NOT_READY (-3)
#define MPDRV_SEMAPHORE_TAKEN (-2)

/* the semaphore byte's address. tas takes the byte, and 0 frees it. a caller
 * must hold it while it builds, sends or receives a frame */
extern char *_mp_SemaphoreAddress;

/* _mp_Context is the state mpdrv_init points _mp at.
 * _mp_RootDeviceNo holds each port's root device number, and _mp_Per2APTable
 * each device's Maple address. _mp_MajorVersion and _mp_MinorVersion are the
 * driver's version */
extern struct mp_context _mp_Context;
extern struct mp_context *_mp;
extern const Uint8 _mp_Per2APTable[KDD_DEV_MAX];
extern const Sint32 _mp_RootDeviceNo[MPDRV_PORTS];
extern const Sint32 _mp_MajorVersion;
extern const Sint32 _mp_MinorVersion;

/* the driver's interface, which Kaede's calls forward to. each does what
 * kaede.h says of the kd call that forwards to it. mpdrv_init leaves
 * send_bytes and recv_bytes unread. mpdrv_frame_send returns kdSendFrame's
 * codes.
 *
 * a receive takes three calls. mpdrv_frame_recv_pre takes the semaphore and
 * returns 0, or returns kdRecvFrame's codes. mpdrv_frame_recv_main then reads
 * the frame's responses. mpdrv_frame_recv_post marks the frame received and
 * frees the semaphore */
Sint32 mpdrv_init(void *sendbuf, Sint32 send_bytes, void *recvbuf,
                  Sint32 recv_bytes);
Sint32 mpdrv_init_ex(Sint32 *devmap, void **recvbuf, void *sendbuf, Sint32 num);
Sint32 mpdrv_exit(void);
Sint32 mpdrv_frame_send(void);
Sint32 mpdrv_frame_recv_pre(void);
Sint32 mpdrv_frame_recv_main(void);
Sint32 mpdrv_frame_recv_post(void);
void mpdrv_timeout(Sint32 value);
void mpdrv_nopcount(Sint32 value);
void mpdrv_speed(Sint32 value);
void mpdrv_debugmode(Sint32 value);
void mpdrv_dreqcount(Sint32 value);
void mpdrv_optimize(Sint32 value);
void mpdrv_rootplay(Sint32 value);
void mpdrv_gunmode(Sint32 value);
void mpdrv_autoflash(Sint32 value);
Sint32 mpdrv_get_timeout(void);
Sint32 mpdrv_get_swapmode(void);
Sint32 mpdrv_get_nopcount(void);
Sint32 mpdrv_get_speed(void);
Sint32 mpdrv_get_debugmode(void);
Sint32 mpdrv_get_dreqcount(void);
Sint32 mpdrv_get_optimize(void);
Sint32 mpdrv_get_rootplay(void);
Sint32 mpdrv_get_gunmode(void);
Sint32 mpdrv_get_autoflash(void);
const KDS_DEVICEINFO *mpdrv_get_deviceinfo(Sint32 device);
const KDS_PORT *mpdrv_get_portstatus(Sint32 port);
Sint32 mpdrv_stat(Sint32 device, Sint32 seqno);
Sint32 mpdrv_ischanged(Sint32 device);
void mpdrv_recognize(Sint32 device);
void mpdrv_get_version(Sint32 *major, Sint32 *minor);

/* the driver's steps below its interface, which no kd call forwards to */

/* start the driver on the buffers at _mp->sendbuf and _mp->recvbuf. it clears
 * every port and device, and gives each setting its starting value. a system
 * bus of revision 1 or below leaves port D's devices disabled */
void mpdrv_init_globals(void);
/* add count NOPs to the frame being built, and return 0. the caller must
 * hold the semaphore */
Sint32 mpdrv_insertnop(Sint32 count);
/* set MPDRV_LAST_HEADER in the header written last to the frame being
 * built, and return 0. after a run of NOPs, that header is the run's first.
 * mpdrv_init and mpdrv_init_ex call it without the semaphore, before they
 * enable the Maple DMA. any other caller must hold the semaphore */
Sint32 mpdrv_set_end_ns(void);
/* update port's state from its root device's response to the frame last
 * received. the first _mp->rootplay unanswered frames in a row change only
 * playcount. past them, each unanswered frame sets lmon to 0, and the call
 * returns -1. it returns 0 for any other frame */
Sint32 mpdrv_check_port(Sint32 port);
/* update device, one of port's, from its response to the frame last
 * received, and return 0. an expansion device's update reads port's state as
 * mpdrv_check_port last left it */
Sint32 mpdrv_make_devicedata(Sint32 device, Sint32 port);
/* addr's physical address, which the Maple DMA takes */
void *mpdrv_v2p(void *addr);

/* while _mp->debugmode is nonzero, format the text as vsprintf does and pass
 * it to _mpdrv_dbg_out, unless that is NULL. the text and its NUL must fit in
 * 256 bytes */
void mpdrv_dpf(const char *format, ...);
/* reverse the byte order of each of count words at words, in place. count
 * must be 1 or more. returns 0 */
Sint32 mpdrv_swaplong(Uint32 *words, Sint32 count);

/* set _mp->swapmode to value's low bit */
void mpdrv_swapmode(Sint32 value);

/* the two calls below queue command for device in the frame being built. for
 * both, the command carries count data words from data, then extra_count
 * extra words from extra. _mp->swapmode says which of them are byte-reversed.
 * device's response goes to its area in _mp->recv_data.
 *
 * a NOP is a header word of port A. where the frame's last header names
 * device's port, each call first writes _mp->nopcount NOPs.
 *
 * for both, device must be below KDD_DEV_MAX, the caller must hold the
 * semaphore, and the frame must have room for the command and its NOPs */

/* queue command as mpdrv_setcmd_force does, make it device's queued command,
 * and return its sequence number. where device has a command queued already,
 * queue nothing and return -1 */
Sint32 mpdrv_setcmd(Sint32 device, Uint8 command, const void *data,
                    Sint32 count, const void *extra, Sint32 extra_count);
/* queue command whatever device has queued already, and return 0. the call
 * gives the command no sequence number, and leaves device's queued command as
 * it is */
Sint32 mpdrv_setcmd_force(Sint32 device, Uint8 command, const void *data,
                          Sint32 count, const void *extra, Sint32 extra_count);

/* the calls below queue their command for device as mpdrv_setcmd_force does,
 * and make it device's queued command. each returns the command's sequence
 * number, or -1 when device has a command queued already.
 *
 * a call without _ns takes the semaphore around the queueing. where the
 * semaphore is already held, the call says so through mpdrv_dpf and returns
 * MPDRV_SEMAPHORE_TAKEN. an _ns call leaves the semaphore to its caller.
 *
 * size counts the 4-byte words of src or buf. mpdrv_set_arctrl and
 * mpdrv_set_ft4ctrl send one word of buf */
Sint32 mpdrv_set_dreq(Sint32 device);
Sint32 mpdrv_set_allstatreq(Sint32 device);
Sint32 mpdrv_set_dreset(Sint32 device);
Sint32 mpdrv_set_dkill(Sint32 device);
Sint32 mpdrv_set_getcond(Sint32 device, Sint32 func);
Sint32 mpdrv_set_getminfo(Sint32 device, Sint32 func, Sint32 pt);
Sint32 mpdrv_set_bread(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                       Sint32 block);
Sint32 mpdrv_set_bwrite(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                        Sint32 block, const void *src, Sint32 size);
Sint32 mpdrv_set_getlerr(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                         Sint32 block);
Sint32 mpdrv_set_arctrl(Sint32 device, const void *buf);
Sint32 mpdrv_set_ft4ctrl(Sint32 device, const void *buf);
Sint32 mpdrv_set_setcond(Sint32 device, Sint32 func, const void *buf,
                         Sint32 size);
Sint32 mpdrv_set_transagain(Sint32 device);
Sint32 mpdrv_set_dreq_ns(Sint32 device);
Sint32 mpdrv_set_allstatreq_ns(Sint32 device);
Sint32 mpdrv_set_dreset_ns(Sint32 device);
Sint32 mpdrv_set_dkill_ns(Sint32 device);
Sint32 mpdrv_set_getcond_ns(Sint32 device, Sint32 func);
Sint32 mpdrv_set_getminfo_ns(Sint32 device, Sint32 func, Sint32 pt);
Sint32 mpdrv_set_bread_ns(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                          Sint32 block);
Sint32 mpdrv_set_bwrite_ns(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                           Sint32 block, const void *src, Sint32 size);
Sint32 mpdrv_set_getlerr_ns(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                            Sint32 block);
Sint32 mpdrv_set_arctrl_ns(Sint32 device, const void *buf);
Sint32 mpdrv_set_ft4ctrl_ns(Sint32 device, const void *buf);
Sint32 mpdrv_set_setcond_ns(Sint32 device, Sint32 func, const void *buf,
                            Sint32 size);
/* add port's reset pattern to the frame being built, and return 0. the
 * caller must hold the semaphore */
Sint32 mpdrv_set_hreset(Sint32 port);

/* the extended commands. each call without _ns queues and returns as
 * mpdrv_set_getcond does, and each _ns call as mpdrv_set_getcond_ns does.
 * words counts the 4-byte words of data, and extra_words those of extra.
 * mpdrv_set_cameracommand sends one word of data */
Sint32 mpdrv_set_getcond2(Sint32 device, Sint32 func, const void *data,
                          Sint32 words);
Sint32 mpdrv_set_getcond2_ns(Sint32 device, Sint32 func, const void *data,
                             Sint32 words);
Sint32 mpdrv_set_cameracommand(Sint32 device, const void *data);
Sint32 mpdrv_set_cameracommand_ns(Sint32 device, const void *data);
Sint32 mpdrv_set_unicommand(Sint32 device, Sint32 func, Uint8 command,
                            const void *data, Sint32 words);
Sint32 mpdrv_set_unicommand_ns(Sint32 device, Sint32 func, Uint8 command,
                               const void *data, Sint32 words);
Sint32 mpdrv_set_unicommand2(Sint32 device, Uint8 command, const void *data,
                             Sint32 words, const void *extra,
                             Sint32 extra_words);
Sint32 mpdrv_set_unicommand2_ns(Sint32 device, Uint8 command, const void *data,
                                Sint32 words, const void *extra,
                                Sint32 extra_words);

#endif
