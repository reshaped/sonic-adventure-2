/* Kaede Maple bus driver, as far as the sources use it */
#ifndef KAEDE_H
#define KAEDE_H
#include <sg_xpt.h>

/* devices are numbered from 0, six to each of the four ports */
#define KDD_DEV_MAX 24
/* each port's root device, the first of its six */
#define KDD_DEV_A0 0
#define KDD_DEV_B0 6
#define KDD_DEV_C0 12
#define KDD_DEV_D0 18
/* the devices in the first and second expansion sockets of each port's root
 * device */
#define KDD_DEV_A1 1
#define KDD_DEV_A2 2
#define KDD_DEV_B1 7
#define KDD_DEV_B2 8
#define KDD_DEV_C1 13
#define KDD_DEV_C2 14
#define KDD_DEV_D1 19
#define KDD_DEV_D2 20

/* function bits, as in KDS_DEVICEINFO's function */
#define KDD_FUNC_CONTROLLER 0x00000001
#define KDD_FUNC_STORAGE 0x00000002
#define KDD_FUNC_LCD 0x00000004
#define KDD_FUNC_TIMER 0x00000008
#define KDD_FUNC_SOUNDINPUT 0x00000010
#define KDD_FUNC_ARGUN 0x00000020
#define KDD_FUNC_KEYBOARD 0x00000040
#define KDD_FUNC_LIGHTGUN 0x00000080
#define KDD_FUNC_VIBRATION 0x00000100
#define KDD_FUNC_CAMERA 0x00000800

/* response codes, as in KDS_DEVICE's response */
#define KDD_RES_DEVICESTATUS 0x05
#define KDD_RES_DEVICEALLSTATUS 0x06
#define KDD_RES_DEVICEREPLY 0x07
#define KDD_RES_DATATRANSFER 0x08
#define KDD_RES_NORESPONSE 0xff
#define KDD_RES_TRANSMITAGAIN 0xfc
#define KDD_RES_FILEERROR 0xfb

/* a port's state, from its root device's responses */
typedef struct {
    /* the source address of the root device's last response, or 0 once the
     * port reads as empty. the root device sets in it the bit of each
     * expansion socket in use */
    Uint8 lmon;
    Uint8 lmold; /* lmon as it was before its last update */
    /* the bits that lmon gained and lost at its last update */
    Uint8 lmcon;
    Uint8 lmremv;
    /* the frames in a row that the root device has left unanswered */
    Uint32 playcount;
} KDS_PORT;

/* a device's state. a command's callback finds the code of the device's
 * response in response. recvdata points at the whole response, header
 * first */
typedef struct {
    Uint8 on;
    Uint8 old;
    Uint8 connect;
    Uint8 remove;
    Uint8 response;
    Uint8 dst_ap;
    Uint8 src_ap;
    Uint8 datasize;
    void *recvdata;
    Sint32 errcode;
    Uint32 reserved[2];
} KDS_DEVICE;

/* a device's description of itself, which the driver reads from its status
 * response when it connects. a removed device's is zeros, but for a
 * product_name of "(none)" */
typedef struct {
    Uint32 function;
    /* a 4-byte word for each of its functions, up to three. the words go in
     * the order of the function bits, highest first */
    Uint8 fd[3][4];
    Uint8 is_root;
    Uint8 area_code;
    Uint8 connector_dir[2];
    char product_name[32];
    char license[64];
    Uint16 stdby_pow;
    Uint16 max_pow;
} KDS_DEVICEINFO;
/* the directions a connector_dir can name, 0 to 3 */
#define KD_CONNECTOR_DIRS 4

/* the values of KDS_LCDINFO's normally and direction that the sources
 * name */
#define KDD_LCD_NORMALLYWHITE 0
#define KDD_LCD_HV_00 0
/* an LCD's description of itself, from its fd word. writecount is its
 * accesses to write a block */
typedef struct {
    Uint16 partition;
    Uint16 blocksize; /* in bytes */
    Uint16 writecount;
    Uint8 direction; /* a KDD_LCD_HV_ value */
    Uint8 normally; /* a KDD_LCD_NORMALLY value */
} KDS_LCDINFO;

/* a storage's description of itself: its accesses to write a block, and to
 * read one */
typedef struct {
    Uint16 partition;
    Uint16 blocksize;
    Uint16 writecount;
    Uint16 readcount;
    Uint16 removable;
    Uint16 reserved;
} KDS_STORAGEINFO;

/* a timer's description of itself: the four bytes of its fd word */
typedef struct {
    Uint8 write_timer;
    Uint8 read_timer;
    Uint8 button;
    Uint8 alarm;
} KDS_TIMERINFO;

/* a controller's, a sound input's and an AR gun's fd word, its first byte
 * highest */
typedef struct {
    Uint32 button;
} KDS_CONTROLLERINFO;
typedef struct {
    Uint32 function;
} KDS_SIPINFO;
typedef struct {
    Uint32 button;
} KDS_GUNINFO;
/* the first two bytes of a vibration unit's fd word */
typedef struct {
    Uint32 vn;
    Uint32 se;
} KDS_VIBRATIONINFO;

/* kdGetDeviceInfo returns device's description of itself, and kdGetDevice
 * Kaede's record of device */
const KDS_DEVICEINFO *kdGetDeviceInfo(Sint32 device);
const KDS_DEVICE *kdGetDevice(Sint32 device);
/* 0 while the command of seqno is still under way on device */
Sint32 kdGetStatus(Sint32 device, Sint32 seqno);
/* whether device has changed since kdRecognizeChanged last took note */
Sint32 kdIsChanged(Sint32 device);
void kdRecognizeChanged(Sint32 device);
/* the storage commands. each queues a command to device's function func,
 * and returns its sequence number. the number is above 0, and kdGetStatus
 * takes it as seqno. each returns -1 where device already has a command
 * queued, and -2 where the driver's semaphore is held.
 *
 * pt is a partition of the storage, and block a block of it. a block read or
 * write takes KDS_STORAGEINFO's readcount or writecount accesses, and phase
 * numbers the access from 0. a write's size counts the 4-byte words at src.
 *
 * kdSetGetMediaInfo asks for partition pt's media information.
 * kdSetGetLastError asks for the last write's error. kdSetUniCommand sends
 * command, and words counts the 4-byte words at data */
Sint32 kdSetGetMediaInfo(Sint32 device, Sint32 func, Sint32 pt);
Sint32 kdSetBlockRead(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                      Sint32 block);
Sint32 kdSetBlockWrite(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                       Sint32 block, const void *src, Sint32 size);
Sint32 kdSetGetLastError(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                         Sint32 block);
Sint32 kdSetUniCommand(Sint32 device, Sint32 func, char command,
                       const void *data, Sint32 words);
/* make handler the handler that Kaede calls in slot no, or clear the slot
 * with null. Kaede calls the handler of each slot, from 0 to 31, at the end of
 * each frame's Maple DMA. it calls them after it tries to receive the frame,
 * whether or not it does. a no above 31 changes nothing */
void kdSetHandler(Uint32 no, void (*handler)(void));

/* each decodes its function's fd word into *sinfo and returns 0, or returns
 * -1 when the device lacks the function */
Sint32 kdDevGetControllerInfo(KDS_CONTROLLERINFO *sinfo,
                              const KDS_DEVICEINFO *info);
Sint32 kdDevGetLCDInfo(KDS_LCDINFO *sinfo, const KDS_DEVICEINFO *info);
Sint32 kdDevGetStorageInfo(KDS_STORAGEINFO *sinfo, const KDS_DEVICEINFO *info);
Sint32 kdDevGetTimerInfo(KDS_TIMERINFO *sinfo, const KDS_DEVICEINFO *info);
Sint32 kdDevGetSIPInfo(KDS_SIPINFO *sinfo, const KDS_DEVICEINFO *info);
Sint32 kdDevGetGunInfo(KDS_GUNINFO *sinfo, const KDS_DEVICEINFO *info);
Sint32 kdDevGetVibrationInfo(KDS_VIBRATIONINFO *sinfo,
                             const KDS_DEVICEINFO *info);
/* the fd word of func, a single function bit, or null when the device lacks
 * the function */
const Uint8 *kdDevGetFD(const KDS_DEVICEINFO *dev, Uint32 func);

/* an Ex call queues its command as the call of its name without Ex does. it
 * also registers cbfunc for the command. kdRecvFrame calls cbfunc once the
 * command is done. it passes the device's number, the command's sequence
 * number, and the device's state and description.
 *
 * a kdRecvFrame that finds the semaphore taken leaves its frame unreceived.
 * Kaede's next command call then receives the frame before it returns. so a
 * callback can run inside a command call */
typedef void (*KD_CALLBACK)(Sint32 device, Sint32 seqno, KDS_DEVICE *dev,
                            KDS_DEVICEINFO *info);
Sint32 kdSetBlockReadEx(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                        Sint32 block, KD_CALLBACK cbfunc);
Sint32 kdSetBlockWriteEx(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                         Sint32 block, const void *src, Sint32 size,
                         KD_CALLBACK cbfunc);

/* the frames Kaede sends in turn */
#define KD_FRAMES 2
/* the driver statistics. each array of KD_FRAMES entries holds an entry for
 * each of the last frames, filled in turn */
typedef struct {
    /* frames sent, except those sent after a bus error */
    Uint32 send_count;
    Uint32 recv_count; /* frames kdRecvFrame has received */
    Uint32 vbin_count; /* vertical blank interrupts */
    Uint32 dmaend_count; /* ends of a frame's Maple DMA */
    /* sends that failed, except those after a bus error */
    Uint32 cannot_send_count;
    Uint32 blocked_count; /* always 0, since nothing adds to it */
    Uint32 total_vover_count; /* vertical blanks during a frame's transfer */
    Uint32 error_count[4]; /* Maple bus errors, by kind */
    Uint16 send_line[KD_FRAMES]; /* the scan line at the frame's send */
    /* the scan line at the frame's receive. each vertical blank during the
     * transfer adds the number of the line a vertical blank starts on */
    Uint16 recv_line[KD_FRAMES];
    Uint16 vover_count[KD_FRAMES]; /* vertical blanks during its transfer */
} KDS_SYSTEMINFO;

/* start Kaede, and return 0. Kaede then sends a frame at each vertical blank,
 * and receives it when the frame's Maple DMA ends. the call clears every
 * device state, command callback, handler slot and statistic. it keeps the
 * function kdSetGunCallback gave.
 *
 * recvbuf takes the devices' responses, 0xc000 bytes: 0x400 for each device in
 * each of two frames. sendbuf takes the two frames, which go out in turn. the
 * second lies 0x3000 bytes past the first. Kaede uses each buffer from its
 * first 32-byte boundary */
Sint32 kdInitSystem(void *recvbuf, void *sendbuf);
/* start Kaede as kdInitSystem does, for only the num devices devmap lists.
 * recvbuf[i] takes the responses of device devmap[i], 0x400 bytes in each of
 * two frames. the second frame in sendbuf lies num * 0x400 bytes past the
 * first */
Sint32 kdInitSystemEx(Sint32 *devmap, void **recvbuf, void *sendbuf,
                      Sint32 num);
/* stop Kaede: disable the two Maple interrupts, stop the driver and clear
 * every device state. the call takes kdInitSystem's handlers off their
 * chains, but for the vertical blank's. by a bug, it passes
 * syChainDeleteHandler the handler in kdSetHandler's slot 0, not that one's
 * chain number. so the vertical blank's handler goes on calling
 * kdSendFrame */
void kdExitSystem(void);
/* Kaede's own record of its statistics, which it updates in place as it
 * sends and receives */
const KDS_SYSTEMINFO *kdGetSystemInfo(void);
/* send the commands queued since the last frame, and return 0. it returns -3
 * while the last frame sent waits to be received. it returns -1 while the Maple
 * DMA is busy. it returns -2 while the semaphore is taken. before Kaede starts
 * it sends nothing and returns 0, but after kdExitSystem it still tries to
 * send. after a Maple bus error, or a send that fails, the call receives the
 * last frame sent */
Sint32 kdSendFrame(void);
/* the Maple bus timeout, in microseconds, while any port is in light gun
 * mode. mpdrv_frame_send sets it when it changes those ports */
extern const Sint32 kdgGunModeTimeout;
/* receive the frame sent, with every interrupt masked, and return 0. the call
 * updates each device's state, and scans the light guns' triggers. it calls
 * back each command the frame answered. it returns -3 when no frame waits, and
 * -1 while the Maple DMA is busy. it returns -2 while the semaphore is taken */
Sint32 kdRecvFrame(void);

/* the driver's settings, by type. kdSetDriverMode sets the setting of type
 * to mode and returns 0, and kdGetDriverMode returns the setting. for any
 * other type, each returns -1 and changes nothing */
#define KDD_MODE_OPTIMIZE 0
#define KDD_MODE_NOPCOUNT 1
#define KDD_MODE_TIMEOUT 2
#define KDD_MODE_SPEED 3
#define KDD_MODE_DREQCOUNT 4
#define KDD_MODE_DEVENDIAN 5
#define KDD_MODE_DEBUG 6
#define KDD_MODE_ROOTPLAY 7
/* the ports to put in light gun mode, a bit for each. the next send puts them
 * there, and kdGetDriverMode returns the ports the last send put there.
 *
 * a send that changes those ports also sets the bus's timeout, which replaces
 * KDD_MODE_TIMEOUT's setting. while a port is in light gun mode, the timeout
 * is 300 microseconds. otherwise it is the driver's starting timeout */
#define KDD_MODE_GUNMODE 8
/* the color the screen flashes in before a light gun is read, or 0 for no
 * flash */
#define KDD_MODE_AUTOFLASH 9
Sint32 kdSetDriverMode(Sint32 type, Sint32 mode);
Sint32 kdGetDriverMode(Sint32 type);

/* kdGetPortStatus returns the driver's record of port. kdGetVersion stores
 * the driver's version in *major and *minor */
const KDS_PORT *kdGetPortStatus(Sint32 port);
void kdGetVersion(Sint32 *major, Sint32 *minor);

/* the other commands. each queues its command for device, and returns as
 * the storage commands do. kdSetResetPattern queues port's reset pattern
 * instead, without taking the semaphore, and returns 0 */
Sint32 kdSetDeviceRequest(Sint32 device);
Sint32 kdSetAllStatusRequest(Sint32 device);
Sint32 kdSetDeviceReset(Sint32 device);
Sint32 kdSetDeviceKill(Sint32 device);
Sint32 kdSetGetCondition(Sint32 device, Sint32 func);
Sint32 kdSetARControl(Sint32 device, const void *buf);
Sint32 kdSetFT4Control(Sint32 device, const void *buf);
Sint32 kdSetSetCondition(Sint32 device, Sint32 func, const void *buf,
                         Sint32 size);
Sint32 kdSetTransmitAgain(Sint32 device);
Sint32 kdSetResetPattern(Sint32 port);

/* register func for device's command seqno and return 0. registering
 * another for the device before the next send or receive replaces it */
Sint32 kdSetCallback(Sint32 device, KD_CALLBACK func, Sint32 seqno);
/* the Ex calls of the commands above, and three commands more with theirs:
 * kdSetGetCondition2, kdSetCameraCommand and kdSetUniCommand2. each plain
 * call returns as the storage commands do. each Ex call registers cbfunc only
 * for a positive sequence number. one that finds the semaphore taken returns
 * -2 and queues nothing. the camera command carries one word of data, and the
 * universal commands the word counts given */
Sint32 kdSetGetCondition2(Sint32 device, Sint32 func, const void *data,
                          Sint32 words);
Sint32 kdSetGetCondition2Ex(Sint32 device, Sint32 func, const void *data,
                            Sint32 words, KD_CALLBACK cbfunc);
Sint32 kdSetCameraCommand(Sint32 device, const void *data);
Sint32 kdSetCameraCommandEx(Sint32 device, const void *data,
                            KD_CALLBACK cbfunc);
Sint32 kdSetUniCommandEx(Sint32 device, Sint32 func, char command,
                         const void *data, Sint32 words, KD_CALLBACK cbfunc);
Sint32 kdSetUniCommand2(Sint32 device, char command, const void *data,
                        Sint32 words, const void *extra, Sint32 extra_words);
Sint32 kdSetUniCommand2Ex(Sint32 device, char command, const void *data,
                          Sint32 words, const void *extra, Sint32 extra_words,
                          KD_CALLBACK cbfunc);
Sint32 kdSetDeviceRequestEx(Sint32 device, KD_CALLBACK cbfunc);
Sint32 kdSetAllStatusRequestEx(Sint32 device, KD_CALLBACK cbfunc);
Sint32 kdSetDeviceResetEx(Sint32 device, KD_CALLBACK cbfunc);
Sint32 kdSetDeviceKillEx(Sint32 device, KD_CALLBACK cbfunc);
Sint32 kdSetGetConditionEx(Sint32 device, Sint32 func, KD_CALLBACK cbfunc);
Sint32 kdSetGetMediaInfoEx(Sint32 device, Sint32 func, Sint32 pt,
                           KD_CALLBACK cbfunc);
Sint32 kdSetGetLastErrorEx(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                           Sint32 block, KD_CALLBACK cbfunc);
Sint32 kdSetARControlEx(Sint32 device, const void *buf, KD_CALLBACK cbfunc);
Sint32 kdSetFT4ControlEx(Sint32 device, const void *buf, KD_CALLBACK cbfunc);
Sint32 kdSetSetConditionEx(Sint32 device, Sint32 func, const void *buf,
                           Sint32 size, KD_CALLBACK cbfunc);

/* light guns, on the ports KDD_MODE_GUNMODE puts in light gun mode. after
 * each frame, Kaede scans their triggers, unless the function
 * kdSetGunCallback gave returns 0. the first gun whose trigger was just
 * pulled is read: Kaede lets it latch where it sees the beam, and the next
 * send reads the position. kdSetGunTrigger asks for port's gun to be read the
 * same way, and a scan replaces the request. kdGetGunPosition stores the last
 * position read in *x and *y. kdGetGunLatchedPort returns its port, or -1
 * when the last send read none */
void kdSetGunCallback(Sint32 (*func)(void));
void kdSetGunTrigger(Uint32 port);
void kdGetGunPosition(Sint32 *x, Sint32 *y);
Sint32 kdGetGunLatchedPort(void);

#endif
