/* the Shinobi libraries, as far as the sources use them */
#ifndef SHINOBI_H
#define SHINOBI_H
#include <machine.h>
#include <sg_xpt.h>

/* syMallocInit installs the default heap in the supplied span. The port uses
 * only complete 32-byte units after aligning its start. A span with no unit
 * installs an empty heap. The original requires a 32-byte aligned start.
 * syMallocFinish does nothing. */
void syMallocInit(void *heap, Uint32 size);
void syMallocFinish(void);
/* store the heap's free bytes in *wholeFreeSize, and its largest free block's
 * in *biggestFreeBlockSize, headers excluded */
void syMallocStat(Uint32 *wholeFreeSize, Uint32 *biggestFreeBlockSize);

/* the functions that syMalloc, syFree, syCalloc and syRealloc call. the C
 * library's malloc, free, calloc and realloc call them too */
typedef struct {
    void *(*malloc)(Uint32 size);
    void (*free)(void *ptr);
    void *(*calloc)(Uint32 count, Uint32 size);
    void *(*realloc)(void *ptr, Uint32 size);
} SYS_MALLOC_IF;
/* make malloc_if's functions the ones those calls go through, or the heap's own
 * for a null malloc_if. it returns the functions it replaces */
SYS_MALLOC_IF *syMallocChangeIF(SYS_MALLOC_IF *malloc_if);

/* a block is what syMalloc, syCalloc or syRealloc returns. syMalloc gives one
 * of size bytes, not cleared. syCalloc gives one of count records of size
 * bytes, cleared. The default port heap rejects a count * size exceeding
 * Uint32. Each returns null where the heap can't give the block. syFree takes
 * a block of the same allocator, or null. The caller owns a block until it
 * frees it or successfully reallocates it. Fixed scratch areas, pointers into
 * a block and borrowed task/model data stay with their original owner.
 *
 * syRealloc moves ptr's data, as much as fits, to a block of size bytes, and
 * returns that block. ptr must be a block, never null. where the heap can't
 * give one, it returns null and leaves ptr's block as it was. as shipped, the
 * heap's own realloc can fail to grow a block the heap has room for */
void *syMalloc(Uint32 size);
void *syCalloc(Uint32 count, Uint32 size);
void *syRealloc(void *ptr, Uint32 size);
void syFree(void *ptr);

/* leave the program by the boot ROM's exit, with the video chip's punch-through
 * alpha threshold set to 0xff. where app_root.h's record names an app root, the
 * record's unk_e chooses one of two other exits. the call returns only if what
 * it jumps to returns */
void syBtExit(void);
/* purge the operand cache, then return what the boot ROM's disc check
 * returns */
Sint32 syBtCheckDisc(void);

/* write back and invalidate every operand cache line. with CCR.ORA set, the
 * cache keeps entries 128 to 255 and 384 to 511 as RAM. the purge leaves
 * them */
void syCachePurgeAll(void);

/* write back the operand cache lines over the size bytes at address, and keep
 * them in the cache. bits 29 and 30 of address are ignored, so a P2 address
 * stands for its P1 twin. a size of 0 does nothing */
void syCacheOCWB(void *address, Uint32 size);
#define OCBI_WALK_MIN 0x30000
#define WRITE_BACK_WALK_MIN 0x10000
/* syCacheOCBI drops the operand cache lines over the size bytes at address, and
 * writes none of them back. syCacheOCBP writes back each line the cache
 * changed, then drops it. each takes address and size as syCacheOCWB does.
 *
 * from a size of WRITE_BACK_WALK_MIN on, syCacheOCWB and syCacheOCBP walk
 * every cache entry instead. syCacheOCBI does so from a size of
 * OCBI_WALK_MIN. a walk masks every interrupt. with CCR.ORA set, it leaves the
 * entries the cache keeps as RAM */
void syCacheOCBI(void *address, Uint32 size);
void syCacheOCBP(void *address, Uint32 size);
/* prefetch the operand cache lines over the size bytes at address, whose
 * bits 29 and 30 are ignored. a size of 0 prefetches address's line, or 2^32
 * lines where address starts a line */
void syCachePREF(void *address, Uint32 size);
/* store data at address, which must lie on 4 bytes, by MOVCA.L. in copy-back
 * mode, a miss claims the line without reading memory. the line's other bytes
 * are then undefined until written */
void syCacheMOVCAL(void *address, Uint32 data);
/* the operand cache's RAM: 8 pages of 1 KB from 0x7c000000, which RAM mode
 * takes from the cache. syCacheAllocCacheRAM claims a run of pages, as many as
 * pages says. it stores the run's start in *address, and counts its pages in
 * use. as shipped, a request that reaches the 8th page stores nothing, so no
 * request gets that page. syCacheFreeCacheRAM takes pages off the count only
 * for an address in page 0 */
void syCacheAllocCacheRAM(void **address, Uint32 pages);
void syCacheFreeCacheRAM(void *address);

/* the video cable, as syCblCheckCable reports it */
typedef enum SYE_CBL_CABLE_TAG {
    SYE_CBL_CABLE_VGA = 0,
    SYE_CBL_CABLE_RESERVED = 1,
    SYE_CBL_CABLE_RGB = 2,
    SYE_CBL_CABLE_VBS = 3
} SYE_CBL_CABLE;
SYE_CBL_CABLE syCblCheckCable(void);
/* the console's broadcast standard, as syCblCheckBroadcast reads it from
 * kbCfgGetId's record. a record that names none of PAL, PAL-M and PAL-N reads
 * as NTSC */
typedef enum SYE_CBL_BROADCAST_TAG {
    SYE_CBL_BROADCAST_NTSC = 0,
    SYE_CBL_BROADCAST_PAL = 1,
    SYE_CBL_BROADCAST_PALM = 2,
    SYE_CBL_BROADCAST_PALN = 3
} SYE_CBL_BROADCAST;
SYE_CBL_BROADCAST syCblCheckBroadcast(void);
/* the display syCblCheck chooses: VGA on a VGA cable, whatever the standard.
 * on any other cable, it is PAL for the PAL and PAL-N standards, and NTSC
 * for the rest */
typedef enum SYE_CBL_TAG {
    SYE_CBL_NTSC = 0,
    SYE_CBL_VGA = 1,
    SYE_CBL_PAL = 2
} SYE_CBL;
SYE_CBL syCblCheck(void);

/* the boot ROM's record of the console. kbCfgIni has the boot ROM ready it, and
 * returns the boot ROM's result. where the library tests that result, it takes
 * any but 0 as a failure. it always passes KBD_CFGINI as permit. kbCfgGetId
 * returns the record's address */
#define KBD_CFGINI 0x00
Sint32 kbCfgIni(Sint32 permit);
Sint8 *kbCfgGetId(void);
/* copy the console's individual ID, the SYD_CFG_IID_SIZE bytes from byte 2 of
 * kbCfgGetId's record, to id. bytes 0 and 1 of the record check the ID. byte 1
 * must be the low byte of the ID's sum, and byte 0 its complement. the call
 * returns SYD_CFG_IID_OK where both hold, and SYD_CFG_IID_CHKERR otherwise. it
 * copies the ID either way. a null id returns SYD_CFG_IID_NG */
Sint32 syCfgGetIndividualID(Sint8 *id);
/* syCfgGetIndividualID's results, and the ID's size in bytes */
#define SYD_CFG_IID_NG 0
#define SYD_CFG_IID_OK 1
#define SYD_CFG_IID_CHKERR 2
#define SYD_CFG_IID_SIZE 6

/* the console settings: 8 bytes from byte 16 of kbCfgGetId's record. packet 5
 * of flash bank 2 keeps them. syBtCfgSet opens the bank in work as fmMngInit
 * does, and leaves it open. it formats the bank where its header is bad.
 *
 * a permit of SYD_BT_CFGINI copies the packet's settings into the record. any
 * other permit names settings by its SYD_BT_CFGWRT_ bits. AT names byte 7, SM
 * 6, LG 5, GM 4 and TM bytes 0 to 3. syBtCfgSet copies those from the record
 * into the packet, then writes the packet back.
 *
 * it returns SYD_BT_CFG_OK, or SYD_BT_CFG_ERR where a flash call fails. a
 * failure before the copy leaves the record as it was */
#define SYD_BT_CFG_OK 0
#define SYD_BT_CFG_ERR (-1)
#define SYD_BT_CFGINI 0x00
#define SYD_BT_CFGWRT_AT 0x01
#define SYD_BT_CFGWRT_SM 0x02
#define SYD_BT_CFGWRT_LG 0x04
#define SYD_BT_CFGWRT_GM 0x08
#define SYD_BT_CFGWRT_TM 0x10
Sint32 syBtCfgSet(Sint32 permit, void *work);

/* physical addresses lie below PHYSICAL_LIMIT */
#define PHYSICAL_LIMIT 0x20000000
/* the start of P1, the area of the address space that reaches memory through
 * the cache. ORing it into a physical address gives that address in P1 */
#define P1_BASE 0x80000000
/* the start of P2, the area of the address space that reaches memory uncached.
 * ORing it into a physical or P1 address gives that address in P2 */
#define P2_BASE 0xa0000000
/* invalidate the instruction cache. the function must run uncached, so
 * syCacheICI calls it through its P2 address */
void syCacheICI_P2_(void);
#define syCacheICI() ((void (*)(void))(P2_BASE | (Uint32)syCacheICI_P2_))()
/* drop every operand cache line, and write none of them back. the function
 * must run uncached */
void syCacheOCI_P2_(void);

/* syCacheInit's form: the value it writes to CCR, the cache control
 * register */
typedef Uint32 SY_CACHE_FORM;
#define SYD_CACHE_FORM_IC_ENABLE 0x00000100
#define SYD_CACHE_FORM_OC_ENABLE 0x00000001
#define SYD_CACHE_FORM_P1_CB 0x00000004
/* with every interrupt masked, purge the operand cache with
 * syCachePurgeAll, set CCR to form and invalidate both caches. the function
 * must run uncached, so syCacheInit calls it through its P2 address */
void syCacheInit_P2_(SY_CACHE_FORM form);
#define syCacheInit(form)                                                      \
    ((void (*)(SY_CACHE_FORM))(P2_BASE | (Uint32)syCacheInit_P2_))(form)
/* each of these must run uncached, through its P2 address.
 * syCacheFinish_P2_ purges the operand cache, then turns both caches off,
 * with every interrupt masked. syCacheGetForm_P2_ returns CCR.
 * syCacheSetForm_P2_ does what syCacheInit_P2_ does */
void syCacheFinish_P2_(void);
SY_CACHE_FORM syCacheGetForm_P2_(void);
void syCacheSetForm_P2_(SY_CACHE_FORM form);

/* port numbers, 0 to 23: six for each of the connectors A to D. a connector's
 * first port is its controller's. the other five are for the units in the
 * controller's sockets. these name each controller and its first two
 * sockets */
#define PDD_PORT_A0 0
#define PDD_PORT_A1 1
#define PDD_PORT_A2 2
#define PDD_PORT_B0 6
#define PDD_PORT_B1 7
#define PDD_PORT_B2 8
#define PDD_PORT_C0 12
#define PDD_PORT_C1 13
#define PDD_PORT_C2 14
#define PDD_PORT_D0 18
#define PDD_PORT_D1 19
#define PDD_PORT_D2 20

/* button bits, in a peripheral's on, off, press and release masks. TA, TB,
 * TX and TY are the buttons of those letters, and ST is Start. the K bits are
 * the direction pad's: KU is up, KD down, KL left and KR right. TL and TR are
 * the buttons that the triggers l and r press */
#define PDD_DGT_TL (1 << 17)
#define PDD_DGT_TR (1 << 16)
#define PDD_DGT_TX (1 << 10)
#define PDD_DGT_TY (1 << 9)
#define PDD_DGT_KR (1 << 7)
#define PDD_DGT_KL (1 << 6)
#define PDD_DGT_KD (1 << 5)
#define PDD_DGT_KU (1 << 4)
#define PDD_DGT_ST (1 << 3)
#define PDD_DGT_TA (1 << 2)
#define PDD_DGT_TB (1 << 1)

/* bits of a peripheral's support, one for each trigger and stick axis it has.
 * AR is r's, AL l's, and AX1 to AY2 are those of x1 to y2 */
#define PDD_DEV_SUPPORT_AY2 (1 << 21)
#define PDD_DEV_SUPPORT_AX2 (1 << 20)
#define PDD_DEV_SUPPORT_AY1 (1 << 19)
#define PDD_DEV_SUPPORT_AX1 (1 << 18)
#define PDD_DEV_SUPPORT_AL (1 << 17)
#define PDD_DEV_SUPPORT_AR (1 << 16)
/* support's bits for the buttons a peripheral has, each at its PDD_DGT_ bit:
 * X, Y, Start, A and B */
#define PDD_DEV_SUPPORT_TX (1 << 10)
#define PDD_DEV_SUPPORT_TY (1 << 9)
#define PDD_DEV_SUPPORT_ST (1 << 3)
#define PDD_DEV_SUPPORT_TA (1 << 2)
#define PDD_DEV_SUPPORT_TB (1 << 1)

/* bits of a PDS_PERIPHERALINFO's type, one for each function its device has.
 * LCD is a screen, TIMER a clock and SOUNDINPUT a microphone */
#define PDD_DEVTYPE_CONTROLLER (1 << 0)
#define PDD_DEVTYPE_STORAGE (1 << 1)
#define PDD_DEVTYPE_LCD (1 << 2)
#define PDD_DEVTYPE_TIMER (1 << 3)
#define PDD_DEVTYPE_SOUNDINPUT (1 << 4)
#define PDD_DEVTYPE_KEYBOARD (1 << 6)
#define PDD_DEVTYPE_LIGHTGUN (1 << 7)
#define PDD_DEVTYPE_VIBRATION (1 << 8)

/* a port's device, as Kaede's KDS_DEVICEINFO describes it. type is a mask of
 * PDD_DEVTYPE_ bits */
typedef struct PDS_PERIPHERALINFO {
    Uint32 type;
    Uint32 reserved[3];
    Uint8 is_root;
    Uint8 area_code;
    Uint8 connector_dir[2];
    char product_name[32];
    char license[64];
    Uint16 stdby_pow;
    Uint16 max_pow;
} PDS_PERIPHERALINFO;

/* a port's state, as the peripheral server last updated it. on marks each
 * button pressed, under the button logic pdInitPeripheral set, and off is
 * its complement. press marks each button that went down since the update
 * before, and release each that went up. old keeps that update's on. r and l
 * are the triggers, 0 to 255. x1 to y2 are the axes of two sticks, -128 to
 * 127. each of r to y2 reads 0 where support lacks its bit */
typedef struct PDS_PERIPHERAL {
    Uint32 id;
    Uint32 support;
    Uint32 on;
    Uint32 off;
    Uint32 press;
    Uint32 release;
    Uint16 r, l;
    Sint16 x1, y1, x2, y2;
    char *name;
    void *extend;
    Uint32 old;
    struct PDS_PERIPHERALINFO *info;
} PDS_PERIPHERAL;
/* under active logic a pressed button's bit is set, and under negative logic
 * clear */
#define PDD_PLOGIC_ACTIVE 0
/* pdInitPeripheral sets the button logic of every later update to logic. logic
 * is PDD_PLOGIC_ACTIVE, or any other value for negative logic. it sets every
 * port to no device and no error, and the trigger thresholds to
 * DEFAULT_THRESHOLD_ON and DEFAULT_THRESHOLD_OFF. it starts Kaede, the SDK's
 * peripheral driver, with recvbuf and sendbuf, unless either is null.
 * pdInitPeripheralEx does the same, but starts Kaede with kdInitSystemEx.
 *
 * pdExitPeripheral stops Kaede where the last init call started it, though an
 * exit may have stopped it already. it sets every port to no device and no
 * error, with a null info */
void pdInitPeripheral(Sint32 logic, void *recvbuf, void *sendbuf);
void pdInitPeripheralEx(Sint32 logic, Sint32 *permap, void **recvbuf,
                        void *sendbuf, Sint32 num);
void pdExitPeripheral(void);
/* pdGetPeripheral returns port's state, and pdGetPeripheralInfo Kaede's
 * description of its device. port must be 0 to 23, which neither call checks */
const PDS_PERIPHERAL *pdGetPeripheral(Uint32 port);
const PDS_PERIPHERALINFO *pdGetPeripheralInfo(Uint32 port);
/* update every port from what Kaede holds for its device.
 * pdExecPeripheralServer calls pdExecPeripheralServerUser.
 *
 * each port takes Kaede's description of its device into info, and an error
 * code from the device's response. a socket's port takes only its device's
 * name. a controller port takes its device's support, id and name where the
 * device passes a test. the device must have KDD_FUNC_CONTROLLER or
 * KDD_FUNC_ARGUN, and a connect of 0 in its KDS_DEVICE. on a
 * KDD_RES_DATATRANSFER response the port also takes the device's buttons,
 * triggers and axes. on any other, it keeps those it had. a controller port
 * whose device fails the test is set to no device. as shipped, a device with
 * KDD_FUNC_ARGUN alone gives its port a support and an id from uninitialized
 * memory */
void pdExecPeripheralServer(void);
void pdExecPeripheralServerUser(void);

/* a port's error code, by its device's response. it is PDD_ERR_RETRY for
 * KDD_RES_TRANSMITAGAIN. it is PDD_ERR_OK for KDD_RES_NORESPONSE,
 * KDD_RES_DATATRANSFER, KDD_RES_DEVICESTATUS, KDD_RES_DEVICEALLSTATUS and 0.
 * any other response gives PDD_ERR_GENERIC */
#define PDD_ERR_OK 0x00000000
#define PDD_ERR_RETRY 0xff0000fc
#define PDD_ERR_GENERIC 0xff000000
Sint32 pdGetPeripheralError(Uint32 port);

/* pdSetMode and pdGetMode set and read Kaede's KDD_MODE_DEVENDIAN.
 * pdSetOptimize and pdGetOptimize set and read its KDD_MODE_OPTIMIZE.
 *
 * under PDD_MODE_TYPE_A, the peripheral server lays out a controller's support
 * anew, and id with it. the device's bits 18 to 31 move down to bits 8 to 21.
 * an l trigger sets TYPE_A_L, and an r trigger TYPE_A_R. each of r to y2
 * still reads 0 where support lacks its bit, 16 to 21. those bits now hold the
 * device's bits 26 to 31. so r reads 0 unless the device's bit 26 is set, and l
 * unless its bit 27 is. x1, y1, x2 and y2 take bits 28 to 31 the same way, in
 * that order */
#define PDD_MODE_TYPE_A 1
#define TYPE_A_L 0xf0
#define TYPE_A_R 0x0f
void pdSetMode(Sint32 mode);
Sint32 pdGetMode(void);
void pdSetOptimize(Uint32 count);
Uint32 pdGetOptimize(void);
/* a trigger presses its button, PDD_DGT_TL for l or PDD_DGT_TR for r, at on
 * and above. it releases the button at off and below. between the two, the
 * button keeps its state */
#define DEFAULT_THRESHOLD_ON 192
#define DEFAULT_THRESHOLD_OFF 160
void pdSetThreshold(Uint8 on, Uint8 off);

/* the light gun. Kaede's KDD_MODE_GUNMODE holds the ports in light gun mode,
 * and its KDD_MODE_AUTOFLASH the flash color, 0 for none. pdGunEnter sets the
 * gun mode to portbit and the flash color to all ones, unless portbit is 0.
 * pdGunLeave sets both to 0, and empties Kaede's handler slot 10. pdGunCheck
 * returns the gun mode in effect, which changes on Kaede's next frame. the
 * position, flash color and callback calls pass straight to Kaede */
void pdGunEnter(Uint32 portbit);
void pdGunLeave(void);
Sint32 pdGunCheck(void);
void pdGunGetPosition(Sint32 *x, Sint32 *y);
/* the controller port of the connector that kdGetGunLatchedPort returns, or -1
 * where that returns -1 for none. the call doesn't check for a connector above
 * 3 */
Sint32 pdGunGetLatchedPort(void);
void pdGunSetFlashColor(Uint32 color);
Uint32 pdGunGetFlashColor(void);
/* for PDD_PORT_A0, B0, C0 or D0, pass its connector, 0 to 3, to
 * kdSetGunTrigger. any other port does nothing */
void pdGunSetTrigger(Uint32 port);
void pdGunSetCallback(Sint32 (*func)(void));

/* the Ninja graphics library's names for a peripheral's state, and for
 * pdGetPeripheral */
typedef PDS_PERIPHERAL NJS_PERIPHERAL;
/* Ninja's names for each connector's controller port */
#define NJD_PORT_A0 0
#define NJD_PORT_B0 6
#define NJD_PORT_C0 12
#define NJD_PORT_D0 18
const NJS_PERIPHERAL *njGetPeripheral(Uint32 port);
/* print port's support and button masks in hex, and its triggers and axes in
 * decimal. each takes a labeled row. the first row is at loc, a screen position
 * in characters as ninja.h's NJM_LOCATION makes it. the rest follow below it */
void njPrintPeripheralInfo(Int loc, Uint32 port);

/* the CPU timer unit. timer 0 counts from syTmrInit, and timer 1 raises a
 * one-shot interrupt. timer 2 is the general-purpose timer. a SY_TMR_CLOCK
 * selects a timer's clock: SYD_TMR_CLOCK_Pn, the peripheral clock divided by
 * n */
#define SYD_TMR_CLOCK_P4 0
#define SYD_TMR_CLOCK_P16 1
#define SYD_TMR_CLOCK_P64 2
#define SYD_TMR_CLOCK_P256 3
#define SYD_TMR_CLOCK_P1024 4
#define SYD_TMR_ERR_OK 0
#define SYD_TMR_ERR_EXIST (-1)
typedef Uint16 SY_TMR_CLOCK;
void syTmrInit(void);
/* the count since syTmrInit, which rises as timer 0 counts down from
 * 0xffffffff */
Uint32 syTmrGetCount(void);
/* the counts from count1 to a later count2: count2 - count1, modulo 2^32 */
Uint32 syTmrDiffCount(Uint32 count1, Uint32 count2);
/* convert between counts and microseconds. a count is 1.28 microseconds: 64
 * cycles of the 50 MHz peripheral clock. only a result past 32 bits
 * overflows */
Uint32 syTmrCountToMicro(Uint32 count);
Uint32 syTmrMicroToCount(Uint32 micro);
/* start and stop timer 2. syTmrGenStart also clears TOCR, the timer output
 * control register */
void syTmrGenStart(void);
void syTmrGenStop(void);
/* have timer 2's next underflow stop the timer and call func(arg), and leave
 * the timer running or stopped as it was. with no func, start the timer, wait
 * here for the underflow and stop it */
void syTmrGenSetInt(void (*func)(void *), void *arg);
/* disable timer 2's underflow interrupt, and leave the timer running or
 * stopped as it was */
void syTmrGenCancelInt(void);
/* set timer 2's clock, and leave the timer running or stopped as it was.
 * any value but the five SYD_TMR_CLOCK_ ones sets SYD_TMR_CLOCK_P64 */
void syTmrGenSetClock(SY_TMR_CLOCK clock);
/* set timer 2's count so that syTmrGenGetCount reads count - 1, and leave the
 * timer running or stopped as it was */
void syTmrGenSetCount(Uint32 count);
/* timer 2's reading, as syTmrGenSetCount starts it, which rises by one each
 * time the timer counts down by one */
Uint32 syTmrGenGetCount(void);
/* syTmrMicroToCount and syTmrCountToMicro for a timer at clock. any value
 * but the five SYD_TMR_CLOCK_ ones takes SYD_TMR_CLOCK_P64's scale */
Uint32 syTmrGenMicroToCount(Uint32 micro, SY_TMR_CLOCK clock);
Uint32 syTmrGenCountToMicro(Uint32 count, SY_TMR_CLOCK clock);
/* syTmrDiffCount, for timer 2's readings */
Uint32 syTmrGenDiffCount(Uint32 count1, Uint32 count2);
/* syTmrInt1SetEx at SYD_TMR_CLOCK_P64 */
Sint32 syTmrInt1Set(Uint32 count, void (*func)(void *), void *arg);
/* stop timer 1, and disable its underflow interrupt */
void syTmrInt1Cancel(void);
/* start timer 1 counting down from count at clock, to call func(arg) at its
 * underflow, and return SYD_TMR_ERR_OK. with no func, wait here for the
 * underflow and stop the timer. the result is SYD_TMR_ERR_EXIST, with nothing
 * changed, while timer 1 runs */
Sint32 syTmrInt1SetEx(Uint32 count, void (*func)(void *), void *arg,
                      SY_TMR_CLOCK clock);

/* the handler syIntInit gives every exception and interrupt code, and
 * syChainDeleteHandler each event whose chain it empties. it reports the event
 * through syIntChkDump, then ends the event, and never returns to its caller */
void syIntBrkRteRte(void);
/* the word that holds code's handler. the handlers' words start at VBR +
 * VECT_TABLE with code VECT_TABLE's, a word for every 0x20 codes. VECT_SHIFT
 * turns a difference of codes into one of their words' addresses.
 *
 * below VECT_TABLE, the word depends on the type of the argument code. a
 * signed code finds its word below the table, as SHC shifts the negative
 * difference arithmetically. C89 leaves that shift to the implementation. an
 * unsigned code's difference wraps, and gives a word 0x20000000 bytes higher */
#define VECT_TABLE 0x200
#define VECT_SHIFT 3
#define USER_VECT(code)                                                        \
    (*(Uint32 *)((Uint32)get_vbr() + VECT_TABLE +                              \
                 (((code) - VECT_TABLE) >> VECT_SHIFT)))
/* the interrupt codes of timer 1's and timer 2's underflows */
#define SYD_INT_EVENT_TUNI1 0x420
#define SYD_INT_EVENT_TUNI2 0x440

/* syIntInit's modes. under SYD_INT_MODE_INTSTACK, exceptions and interrupts
 * run on the stack that ends at stack. the outermost one starts there, and
 * each one nested in it pushes below. under any other mode they run on the
 * interrupted stack, and stack isn't read */
typedef Uint32 SY_INT_MODE;
#define SYD_INT_MODE_LOCALSTACK 0x00000000
#define SYD_INT_MODE_INTSTACK 0x80000000
extern void *sygIntStack;
/* invalidate the instruction cache, and install the library's exception and
 * interrupt entry at VBR, with every interrupt masked. the entry runs the
 * handler in USER_VECT's word for each code. the call then makes syIntBrkRteRte
 * the handler of every code from 0x40 to 0x800, and clears SR.BL. as shipped,
 * three of the entry's four copies can stay unwritten in the operand cache. the
 * CPU fetches code from memory, where they may not be yet */
void syIntInit(SY_INT_MODE mode, void *stack);
/* set SR's interrupt mask to level, and clear SR.BL. level must be 0 to 15,
 * which the call doesn't check */
void syIntEnableInt(Uint32 level);

/* the store queues' address translation, in 1 MB pages from 0xe0000000.
 * sySqInit maps them to main memory's 16 MB, then to the 9 MB from 0x10000000.
 * it maps nothing unless the MMU translates addresses. sySqFinish invalidates
 * the UTLB entries sySqInit took, or entry 0 where sySqInit mapped nothing */
void sySqInit(void);
void sySqFinish(void);
/* sySqStart gives the store queue address, as sySqInit maps them, that writes
 * to dstArray. sySqEnd gives the address that the store queue address sqArray
 * writes to, in P1 for main memory. each keeps an address's low 24 bits and
 * replaces its high byte. so dstArray must lie in main memory or the 9 MB from
 * 0x10000000, which neither call checks */
void *sySqStart(void *dstArray);
void *sySqEnd(void *sqArray);

/* the UTLB entries the MMU translates by. syMmuInit turns address translation
 * on and invalidates every entry. it sets URB, the replace boundary, to 1, and
 * ignores urb.
 *
 * syMmuAddEntry maps the page of size at vaddress to the one at paddress. the
 * page is copy-back cached, and open to privileged code alone. the call takes
 * the first invalid entry from URB up, and returns its number. it returns
 * MMU_NO_ENTRY where none is invalid. size must be one of the four
 * SYD_MMU_PAGESIZE_ values. as shipped, SYD_MMU_PAGESIZE_1K maps 4 KB, and
 * SYD_MMU_PAGESIZE_64K 1 MB.
 *
 * syMmuDeleteEntry invalidates an entry. syMmuCacheCtrl replaces an entry's C
 * and WT bits with cacheCtrl, sign-extended to 32 bits. C and WT are bits 3 and
 * 0 of the entry's data. cacheCtrl must hold no other bit, which it would set
 * in the data too. each call ignores a number of 64 or more */
typedef Uint32 SYMMU;
typedef Sint8 SY_MMU_PAGESIZE;
typedef Sint8 SY_MMU_CACHE;
#define SYD_MMU_PAGESIZE_1K 1
#define SYD_MMU_PAGESIZE_4K 2
#define SYD_MMU_PAGESIZE_64K 3
#define SYD_MMU_PAGESIZE_1M 4
#define MMU_NO_ENTRY 255
void syMmuInit(Uint8 urb);
SYMMU syMmuAddEntry(void *paddress, void *vaddress, SY_MMU_PAGESIZE size);
void syMmuDeleteEntry(SYMMU mmuPageNum);
void syMmuCacheCtrl(SYMMU mmuPageNum, SY_MMU_CACHE cacheCtrl);

/* the expansion devices on the G2 bus. syExtChkInit reads the modem's bytes at
 * 0xa0600000 and 0xa0600004. it then looks for a device at each of 0xa1000400,
 * 0xa1000800, 0xa1001400 and 0xa1001800. a device there holds "GAPS" in its
 * first word. syExtChkInit writes two words to each device it finds, at bytes
 * 0x14 and 0x18. it keeps what it found for the library's other calls. it
 * returns 0, or -1 where a read of the modem's bytes fails. a second
 * syExtChkInit with no syExtChkExit between returns -1 and checks nothing.
 *
 * syExtChkExit writes the same two words to each device syExtChkInit found,
 * forgets them, and returns 0. it returns -1, and does nothing, unless
 * syExtChkInit has returned 0 since the last syExtChkExit */
Sint32 syExtChkInit(void);
Sint32 syExtChkExit(void);
/* the device codes syExtChkConnect takes. it recognizes codes 5 and 6 too */
#define SYD_EXT_CHK_TYPE_MODEM 1
#define SYD_EXT_CHK_TYPE_MMBZIP 2
#define SYD_EXT_CHK_TYPE_MMBUSB 3
#define SYD_EXT_CHK_TYPE_LAN 4
/* 0 for a device found at initialization, and -1 for an absent device.
 * -2 before initialization, or for a type it doesn't recognize */
Sint32 syExtChkConnect(Uint32 type);
#define SYD_EXT_CHK_MODEM_SPEED_33600 1
#define SYD_EXT_CHK_MODEM_SPEED_56K 2
#define SYD_EXT_CHK_MODEM_COUNTRY_USA 1
#define SYD_EXT_CHK_MODEM_COUNTRY_EU 2
#define SYD_EXT_CHK_MODEM_COUNTRY_JAPAN 3
#define SYD_EXT_CHK_MODEM_COUNTRY_AUSTRALIA 4
/* the code the modem gave, or -1 for a code it doesn't know. -2 where no
 * modem was found */
Sint32 syExtChkModemCountry(void);
Sint32 syExtChkModemSpeed(void);

/* the GD file system. a GDFS is a handle on an open file, and a sector is
 * GDD_FS_SCTSIZE bytes.
 *
 * a call that waits for a handle's request runs the file system's server
 * while it waits. the wait returns GDD_ERR_OK once the request completes,
 * and the handle's err once the request fails. it returns GDD_ERR_TOUT
 * after 20 seconds in which the request moves no byte. a request the drive
 * leaves idle returns GDD_ERR_ILLREQ, and any other state GDD_ERR_FATAL */
typedef struct GDS_FS_HANDLE *GDFS;
typedef struct GDS_FS_DIRREC_TBL *GDFS_DIRREC;
#define GDD_FS_SCTSIZE 2048
#define SCT_SHIFT 11 /* the shift that divides by GDD_FS_SCTSIZE */
/* the GD-ROM high-density area's first frame address, and its first track */
#define GD_START_FAD 45150
#define GD_START_TRACK 3
/* the file system's errors. the file system gives GDD_ERR_CHECKBUSY itself.
 * the other codes from GDD_ERR_UNDEF to GDD_ERR_BUSSMPH are the drive's own,
 * as get_cmd_stat gives them. GDD_ERR_UNDEF stands for a drive error the
 * library doesn't know */
#define GDD_ERR_OK 0
#define GDD_ERR_INIT (-1)
#define GDD_ERR_RESET (-2)
#define GDD_ERR_MOUNT (-4)
#define GDD_ERR_DISC (-5)
#define GDD_ERR_NOTFOUND (-8)
#define GDD_ERR_NOHNDL (-9)
#define GDD_ERR_ILLHNDL (-10)
#define GDD_ERR_NOTDIR (-11)
#define GDD_ERR_DIROVER (-12)
#define GDD_ERR_BUSY (-13)
#define GDD_ERR_32ALIGN (-14)
#define GDD_ERR_SIZE (-15)
#define GDD_ERR_SEEK (-16)
#define GDD_ERR_OFS (-17)
/* a transfer mode that is no GDD_FS_TMODE_ value */
#define GDD_ERR_ILLTMODE (-18)
#define GDD_ERR_READ (-19)
#define GDD_ERR_NOTREAD (-20)
#define GDD_ERR_TOUT (-21)
#define GDD_ERR_TRAYOPEND (-23)
#define GDD_ERR_SIZEOVER (-24)
#define GDD_ERR_FATAL (-25)
#define GDD_ERR_UNDEF (-26)
#define GDD_ERR_NOERR (-27)
#define GDD_ERR_RECOVER (-28)
#define GDD_ERR_NOTREADY (-29)
#define GDD_ERR_MEDIA (-30)
#define GDD_ERR_HWARE (-31)
#define GDD_ERR_ILLREQ (-32)
#define GDD_ERR_UNITATTENT (-33)
#define GDD_ERR_PROTECT (-34)
#define GDD_ERR_ABORT (-35)
#define GDD_ERR_NOREADABLE (-36)
#define GDD_ERR_CHECKBUSY (-37)
#define GDD_ERR_BUSSMPH (-38)
/* gdFsInit's result for a table of contents that puts GD_START_TRACK elsewhere
 * than GD_START_FAD. no GDD_ERR_ name gives it */
#define GDFS_ERR_TOC (-51)
/* a handle's request states */
#define GDD_STAT_IDLE 0
#define GDD_STAT_COMPLETE 1
#define GDD_STAT_READ 2
#define GDD_STAT_SEEK 3
#define GDD_STAT_BUSY 4
#define GDD_STAT_ERR 5
/* a state get_cmd_stat gives beyond those: a failure, as GDD_STAT_ERR is.
 * gdctl.h names the others */
#define GDFS_STAT_FATAL 64
/* request types, in a handle's act. GDFS_ACT_GDREAD is a gdFsReqGdRd read,
 * which gdFsTrans32 moves. GDFS_ACT_STOP is gdFsStopRd's abort of a read.
 * after a GDFS_ACT_DAPOS command, the server requests the head position.
 * GDFS_ACT_SUBCODE is a subcode request, decoded into a GDFS_DAINFO */
#define GDFS_ACT_NONE 0
#define GDFS_ACT_READ 1
#define GDFS_ACT_SEEK 2
#define GDFS_ACT_STOP 3
#define GDFS_ACT_GDREAD 4
#define GDFS_ACT_CMD 6
#define GDFS_ACT_SUBCODE 7
#define GDFS_ACT_DAPOS 8
#define GDFS_ACT_DRVSTAT 10
/* drive states. GDD_DRVSTAT_CANTREAD stands in when the drive can't report
 * one */
#define GDD_DRVSTAT_CANTREAD (-1)
#define GDD_DRVSTAT_BUSY 0x00
#define GDD_DRVSTAT_OPEN 0x06
/* the directory bit of a directory entry's flags */
#define GDD_FF_DIRECTORY 0x02
/* a handle's transfer modes: its data moves by the CPU, or by DMA */
#define GDD_FS_TMODE_CPU 0
#define GDD_FS_TMODE_DMA 1
/* a handle's transfer states */
#define GDD_FS_TRANS_READY 0
#define GDD_FS_TRANS_BUSY 1
#define GDD_FS_TRANS_COMPLETE 2
/* the longest file name's bytes, with its NUL */
#define GDD_FS_FNAMESIZE (30 + 1)
/* the sectors that hold x bytes */
#define gdFsCalcSctSize(x) (((x) + (GDD_FS_SCTSIZE - 1)) >> SCT_SHIFT)

/* names for the library's work area, the drive's functions and the
 * interface the library is started through */
typedef struct GDS_FS_WORK GDFS_WORK;
typedef struct GDS_DCF GD_DCF;
typedef struct GDS_FS_LIF GDFS_LIF;
/* callbacks. each is passed first the argument stored with it */
typedef void (*GDFS_FUNC)(void *obj);
typedef void (*GDFS_ERRFUNC)(void *obj, Sint32 err);

/* a file handle, or the system handle, through which the library makes its
 * own requests. on the system handle, a head position request keeps in fid
 * the place the position goes. a subcode request keeps in fad the
 * GDFS_DAINFO it fills */
typedef struct GDS_FS_HANDLE {
    GDFS_WORK *wk;
    /* the file's entry number in its directory, from 1, or 0 for a handle on
     * no entry */
    Sint32 fid;
    Sint32 fad; /* the file's first frame address */
    Sint32 fsize; /* the file's size in bytes */
    Sint32 fsctsize; /* the file's size in sectors */
    Sint32 ofs; /* the next sector to read, counted from fad */
    Sint32 trnsed; /* bytes the request has transferred */
    Sint32 rsize; /* sectors the read requested */
    Sint32 trsize; /* bytes the transfer requested */
    /* the read end, transfer end and error callbacks, each with the argument
     * it is passed first */
    GDFS_FUNC rdendcb;
    void *rdcb_1st;
    GDFS_FUNC trendcb;
    void *trcb_1st;
    GDFS_ERRFUNC errcb;
    void *errcb_1st;
    Sint32 gdchn; /* the request's number from the drive, 0 for none */
    /* the wait value the drive last reported for the busy request */
    volatile Sint32 gdchn_wait;
    Sint32 ex_errcode; /* the drive's extended error code for err */
    Sint16 act; /* the request type, a GDFS_ACT_ value */
    Sint16 trflag; /* the transfer state, a GDD_FS_TRANS_ value */
    Sint16 used; /* whether the handle is open */
    Sint16 tmode; /* the transfer mode, a GDD_FS_TMODE_ value */
    /* the request state, a GDD_STAT_ value or another the drive gave */
    Sint16 stat;
    Sint16 err; /* the last failure's GDD_ERR_ code */
} GDFS_HANDLE;

/* a directory entry as the library keeps it */
typedef struct GDS_FS_DIRREC_ENT {
    /* the first sector's ISO 9660 number, despite the name: 150 below its
     * frame address */
    Sint32 fad;
    Sint32 fsize; /* the file's size in bytes */
    Uint8 flag; /* GDD_FF_ bits */
    Uint8 sid; /* the name's search ID, which a search compares first */
    Sint8 fname[GDD_FS_FNAMESIZE + 1];
    Sint8 pad[2];
} GDFS_DIRREC_ENT;

/* a directory as the library keeps it, its entries in the disc's order.
 * entry 0 is the directory itself, ".", and entry 1 its parent, ".." */
typedef struct GDS_FS_DIRREC_TBL {
    Sint32 dir_num; /* the entries it holds */
    Sint32 max_ent; /* the entries it has room for */
    Sint32 dir_fad; /* the frame address it was read from, 0 for none */
    Sint32 pad;
    GDFS_DIRREC_ENT dirrec_tbl[1]; /* max_ent entries, declared as one */
} GDFS_DIRREC_TBL;

/* an entry's frame address, size and flags */
typedef struct GDS_FS_DIRINFO {
    Sint32 fad;
    Sint32 fsize;
    Uint8 flag; /* GDD_FF_ bits */
    Uint8 pad[3];
} GDFS_DIRINFO;

/* the head position. min, sec and frame are the time within the track */
typedef struct GDS_FS_DAINFO {
    Sint32 track;
    Sint32 min;
    Sint32 sec;
    Sint32 frame;
    Sint32 fad;
} GDFS_DAINFO;

/* the bytes a path table search keeps ahead of its sectors in sctbuf. a record
 * can run past the end of the sector the search read. the search copies its
 * first part there. the next sector then lands after it, and the record reads
 * whole */
#define PATHTBL_ROOM 64

/* the library's work area, which gdFsInit takes. hndlist[0] is the system
 * handle, which syshdl points to. hndtbl points to the max_open file handles
 * that follow it */
struct GDS_FS_WORK {
    Sint32 max_open;
    Sint32 pathtbl_fad; /* the path table's frame address */
    Sint32 pathtbl_size; /* the path table's size in bytes */
    GD_DCF *dcf; /* the drive's functions */
    GDFS_DIRREC curdir; /* the current directory */
    GDFS syshdl;
    GDFS hndtbl;
    volatile GDFS now_work; /* the handle whose request is running, or null */
    volatile Sint32 f_svr; /* TRUE while gdFsGetStatG runs */
    /* the global error callback, and the argument it is passed first */
    GDFS_ERRFUNC g_errcb;
    void *g_errcb_1st;
    /* the drive's version, a digit to a byte from bit 16 down: version 1.10
     * is 0x010100. it is 0 where the drive doesn't give it */
    Sint32 gdc_ver;
    Sint32 errstat; /* the global error, a GDD_ERR_ code */
    /* TRUE once gdFsIsTrayOpen finds the tray open, or a request fails with
     * GDD_ERR_TRAYOPEND or GDD_ERR_UNITATTENT. a start clears it */
    Sint32 istray;
    Sint16 f_init; /* TRUE once a start has succeeded */
    Sint16 daplayed; /* TRUE while a CD-DA play is kept for gdFsDaRelease */
    GDFS_LIF *liftbl; /* the interface the library was started through */
    /* the buffer for the library's own reads and requests. it holds
     * PATHTBL_ROOM bytes for a path table search, then two sectors */
    Uint32 sctbuf[((GDD_FS_SCTSIZE * 2) + PATHTBL_ROOM) / 4];
    GDFS_HANDLE hndlist[1]; /* max_open + 1 handles, declared as one */
};

/* start the file system in gdfs_work, with max_open file handles. dirbuf holds
 * its current directory, of max_dirent entries. gdfs_work must lie on 32 bytes,
 * else the call returns GDD_ERR_32ALIGN. it returns GDD_ERR_OK once the file
 * system starts, and at once where it has started already. otherwise it returns
 * the error that kept it from starting.
 *
 * a disc's table of contents can put GD_START_TRACK elsewhere than
 * GD_START_FAD. the call then returns GDFS_ERR_TOC in place of either result,
 * though the start goes on */
Sint32 gdFsInit(Sint32 max_open, void *gdfs_work, Sint32 max_dirent,
                void *dirbuf);
/* the bytes gdFsInit's gdfs_work takes for x open files, and its dirbuf for
 * x entries */
#define GDFS_WORK_SIZE(x) (sizeof(GDFS_WORK) + ((x) * sizeof(GDFS_HANDLE)))
#define GDFS_DIRREC_SIZE(x)                                                    \
    (sizeof(GDFS_DIRREC_TBL) - sizeof(GDFS_DIRREC_ENT) +                       \
     ((x) * sizeof(GDFS_DIRREC_ENT)))
/* gdFsInit's own start, which gdFsInit calls. it sets the interface the calls
 * below go through, then starts the file system */
Sint32 gdFsInitOvl(Sint32 max_open, void *gdfs_work, Sint32 max_dirent,
                   void *dirbuf);
/* stop the file system, and clear the interface */
void gdFsFinish(void);

/* the calls from here to gdFsIsTrayOpen go through the interface that
 * gdFsInitOvl sets. before gdFsInitOvl, and after gdFsFinish, there is none,
 * and each call does nothing. gdFsReinit then returns GDD_ERR_INIT, and
 * gdFsIsTrayOpen FALSE. a call that gives a handle returns null, and any
 * other returns 0 */

/* close every handle, and start the file system again in the work area it
 * has. the global error callback stays. it returns as gdFsInit does for a
 * first start */
Sint32 gdFsReinit(void);
/* load the directory dirname into gf_dirrec, or into the current directory
 * for a null gf_dirrec. a dirname that starts with a slash or a backslash is
 * a path from the root. any other names an entry of the current directory.
 * an app root, which app_root.h's record can name, stands in for the root.
 *
 * it returns GDD_ERR_OK, or GDD_ERR_DIROVER where the directory holds more
 * entries than gf_dirrec has room for. it returns GDD_ERR_NOTFOUND for no
 * such directory, and GDD_ERR_NOTDIR for an entry that isn't one. a failed
 * read of the directory returns the read's error */
Sint32 gdFsLoadDir(const char *dirname, GDFS_DIRREC gf_dirrec);
/* copy gf_dirrec into the current directory, as many of its entries as
 * fit. it returns GDD_ERR_DIROVER where some don't fit, and GDD_ERR_OK
 * otherwise */
Sint32 gdFsSetDir(GDFS_DIRREC gf_dirrec);
/* store in *dirinfo the frame address, size and flags of the entry name in
 * the current directory. it returns GDD_ERR_OK, or GDD_ERR_NOTFOUND for no
 * such entry */
Sint32 gdFsGetDirInfo(const char *name, GDFS_DIRINFO *dirinfo);
/* a handle on the file fname of gf_dirrec, or of the current directory for
 * a null gf_dirrec. it returns null where no entry has that name, and where
 * every file handle is open */
GDFS gdFsOpen(const char *fname, GDFS_DIRREC gf_dirrec);
/* a handle on the nsct sectors from frame address stsct. it returns null where
 * every file handle is open. it does so too for a stsct below GD_START_FAD */
GDFS gdFsOpenRange(Sint32 stsct, Sint32 nsct);
/* gdFsGetWorkHn returns the handle whose request is running, or null.
 * gdFsGetSysHn returns the system handle */
GDFS gdFsGetWorkHn(void);
GDFS gdFsGetSysHn(void);
/* run the drive's server once, then bring the running request's handle up to
 * date. a start chains this to the vertical blank, so it also runs there */
void gdFsExecServer(void);
/* make erfunc the callback for every handle's errors, passed obj first. a
 * failed request calls it before its handle's own */
void gdFsEntryErrFuncAll(GDFS_ERRFUNC erfunc, void *obj);
/* gdFsGetToc's type for the table of contents of the disc's high-density
 * area */
#define GDFS_TOC_HD_AREA 1
/* a table of contents holds a word for each of TOC_TRACKS tracks. TOC_SUMMARY
 * words follow: the first track's, the last track's and the lead-out's.
 * GDFS_TOC_SIZE is its size in bytes */
#define TOC_TRACKS 99
#define TOC_SUMMARY 3
#define GDFS_TOC_SIZE ((TOC_TRACKS + TOC_SUMMARY) * sizeof(Uint32))
/* read the table of contents of type into buf through the system handle,
 * and wait for it. it returns GDD_ERR_BUSY while the system handle's
 * request runs, and where the drive refuses the request. otherwise it
 * returns the wait's result */
Sint32 gdFsGetToc(Sint32 type, void *buf);
/* the drive's state as the drive reports it, or GDD_DRVSTAT_CANTREAD */
Sint32 gdFsGetDrvStat(void);
/* ask the drive for a frame's Q channel subcode through the system handle,
 * and return GDD_ERR_OK without waiting. no call reads the answer. it
 * returns GDD_ERR_BUSY as gdFsGetToc does */
Sint32 gdFsReqDrvStat(void);

/* the CD-DA calls below go through the system handle, and none waits for
 * its request. each returns GDD_ERR_OK once the drive takes the request,
 * and GDD_ERR_BUSY as gdFsGetToc does.
 *
 * gdFsDaPlay plays the disc's audio from track st_track to end_track, and
 * gdFsDaPlaySct from frame address st_sct to end_sct. each repeats it
 * reptime times. under an app root, track 4 stands for the app root's
 * track, and the others follow it. gdFsDaPlay returns GDD_ERR_BUSY for a
 * st_track that lands below GD_START_TRACK, and gdFsDaPlaySct for a st_sct
 * below GD_START_FAD */
Sint32 gdFsDaPlay(Sint32 st_track, Sint32 end_track, Sint32 reptime);
Sint32 gdFsDaPlaySct(Sint32 st_sct, Sint32 end_sct, Sint32 reptime);
/* pause the audio. the drive has no stop, so gdFsDaStop pauses it too */
Sint32 gdFsDaStop(void);
Sint32 gdFsDaPause(void);
/* end the pause. a kept play that paused where it started plays again from
 * its start, with its own repeat count. any other play goes on reptime
 * times, and is kept no more */
Sint32 gdFsDaRelease(Sint32 reptime);
/* ask the drive where its head is. once the request completes, the server
 * stores the place in *dainfo, which must stay valid until then */
Sint32 gdFsDaGetInfo(GDFS_DAINFO *dainfo);
/* TRUE once the library has found the tray open */
Bool gdFsIsTrayOpen(void);

/* dirbuf, emptied, as a directory with room for max_dirent entries */
GDFS_DIRREC gdFsCreateDirhn(void *dirbuf, Sint32 max_dirent);
/* make the directory dirname the current one: gdFsLoadDir(dirname, NULL),
 * and its result */
Sint32 gdFsChangeDir(const char *dirname);
/* close the handle. a handle whose state is GDD_STAT_READ, GDD_STAT_SEEK or
 * GDD_STAT_BUSY is first passed to gdFsStopRd. a null or a closed handle is
 * ignored */
void gdFsClose(GDFS gdfs);
/* store the file's size in bytes in *fsize and return TRUE. for a null or a
 * closed handle, it returns FALSE and leaves *fsize as it was */
Bool gdFsGetFileSize(GDFS gdfs, Sint32 *fsize);
/* the same as gdFsGetFileSize, in sectors */
Bool gdFsGetFileSctSize(GDFS gdfs, Sint32 *fsctsize);
/* the same as gdFsGetFileSize, for the file's first frame address */
Bool gdFsGetFileFad(GDFS gdfs, Sint32 *fad);
/* seek origins: the file's start, its next read and its end */
#define GDD_SEEK_SET 0
#define GDD_SEEK_CUR 1
#define GDD_SEEK_END 2
/* set where the file's next read starts: sctno sectors from the origin type.
 * it returns GDD_ERR_OK. a place before the file's start or beyond its end
 * returns GDD_ERR_OFS. a type that is no origin returns GDD_ERR_SEEK. each
 * failure leaves the next read where it was. a null or a closed handle returns
 * GDD_ERR_ILLHNDL */
Sint32 gdFsSeek(GDFS gdfs, Sint32 sctno, Sint32 type);
/* the sector the file's next read starts at, counted from the file's first
 * sector. a null or a closed handle gives 0 */
Sint32 gdFsTell(GDFS gdfs);
/* store in *iseof whether the next read would start past the file's last
 * sector, and return GDD_ERR_OK. a null or a closed handle stores nothing
 * and returns GDD_ERR_ILLHNDL */
Sint32 gdFsCheckEof(GDFS gdfs, Bool *iseof);
/* ask the drive for nsct sectors of the file, from where its next read starts,
 * into buf. the call returns once the drive has the request, and gdFsExecServer
 * follows the read from there. it returns how many sectors it asked for, no
 * more than the file has left.
 *
 * it returns 0 where the file has none left, and where the drive refuses the
 * request. each leaves the handle's state GDD_STAT_COMPLETE. it also returns 0,
 * and changes nothing, while another request runs. a null or a closed handle
 * returns GDD_ERR_ILLHNDL, and a buf off a 32-byte boundary GDD_ERR_32ALIGN.
 * each error is below 0 */
Sint32 gdFsReqRd32(GDFS gdfs, Sint32 nsct, void *buf);
/* how many bytes the handle's request has transferred so far. a null or a
 * closed handle gives 0 */
Sint32 gdFsGetNumRd(GDFS gdfs);
/* stop the request the handle has running, and wait for it to end. it returns
 * the wait's result. before the abort, the call runs the server until the
 * drive's wait value, gdchn_wait, reads 1. it returns GDD_ERR_TOUT where that
 * takes more than 20 seconds.
 *
 * a handle whose state is neither GDD_STAT_READ nor GDD_STAT_BUSY has no
 * request to stop. the call sets its state to GDD_STAT_IDLE, and returns
 * GDD_ERR_OK. it does the same where the request ends before the abort. it
 * returns GDD_ERR_BUSY where the running request is another handle's. a null or
 * a closed handle returns GDD_ERR_ILLHNDL */
Sint32 gdFsStopRd(GDFS gdfs);
/* have the handle's errors call erfunc with obj. every interrupt is masked
 * while erfunc and obj are stored. a null or a closed handle stores
 * nothing */
void gdFsEntryErrFunc(GDFS gdfs, GDFS_ERRFUNC erfunc, void *obj);
/* gdFsEntryRdEndFunc stores func and obj as gdFsEntryErrFunc does. the
 * server calls them once a gdFsReqRd32 or gdFsRead request on the handle has
 * moved every byte. a gdFsReqGdRd read never calls them.
 * gdFsEntryTrEndFunc stores them for the end of each DMA transfer. the
 * drive's DMA end interrupt calls them for the running request's handle */
void gdFsEntryRdEndFunc(GDFS gdfs, GDFS_FUNC func, void *obj);
void gdFsEntryTrEndFunc(GDFS gdfs, GDFS_FUNC func, void *obj);
/* read nsct sectors of the file, from where its next read starts, into buf, and
 * wait for them. nsct is cut to the sectors the file has left. buf needs no
 * 32-byte boundary: a buf off one is read through a request of the library's
 * own.
 *
 * a try asks for no sectors where another request runs or the drive refuses it.
 * while nsct and the file have sectors left, the call tries again, up to 20000
 * tries in all. it returns GDD_ERR_TOUT when every try asks for none. a null or
 * a closed handle returns GDD_ERR_ILLHNDL. otherwise the call returns the
 * wait's result, GDD_ERR_OK for a read of no sectors */
Sint32 gdFsRead(GDFS gdfs, Sint32 nsct, void *buf);
/* the state of the handle's request: a GDD_STAT_ value, or another the
 * drive gave. a null or a closed handle returns GDD_ERR_ILLHNDL */
Sint32 gdFsGetStat(GDFS gdfs);
/* the GDD_ERR_ code of the handle's last failure. a null or a closed handle
 * returns 0 */
Sint32 gdFsGetErrStat(GDFS gdfs);

/* gdFsReqRd32, gdFsReqGdRd, gdFsTrans32 and gdFsMovePickup mask every
 * interrupt while they ask the drive and set the handle's state. once the
 * drive takes the request, each runs the drive's server once before it
 * unmasks.
 *
 * gdFsReqGdRd asks the drive for sectors as gdFsReqRd32 does, and returns as
 * gdFsReqRd32 does. it needs no buf: the drive holds the sectors for
 * gdFsTrans32 to move */
Sint32 gdFsReqGdRd(GDFS gdfs, Sint32 nsct);
/* move nbytes of what gdFsReqGdRd's request holds into buf, by DMA. the call
 * returns GDD_ERR_OK once the drive takes the transfer. buf must lie on 32
 * bytes, else the call returns GDD_ERR_32ALIGN. nbytes must be a multiple of
 * 32, else it returns GDD_ERR_SIZE. a handle whose request isn't
 * gdFsReqGdRd's returns GDD_ERR_NOTREAD. a drive that refuses the transfer
 * with -1 gives GDD_ERR_SIZEOVER, and with any other refusal GDD_ERR_BUSY. a
 * null or a closed handle returns GDD_ERR_ILLHNDL */
Sint32 gdFsTrans32(GDFS gdfs, Sint32 nbytes, void *buf);
/* the handle's transfer state, a GDD_FS_TRANS_ value. a null or a closed
 * handle returns GDD_ERR_ILLHNDL */
Sint32 gdFsGetTransStat(GDFS gdfs);
/* move the drive's head to where the file's next read starts, and wait for
 * it. it returns the wait's result. it returns GDD_ERR_BUSY while a request
 * runs, and where the drive refuses the seek. a null or a closed handle
 * returns GDD_ERR_ILLHNDL */
Sint32 gdFsMovePickup(GDFS gdfs);

/* the library's own calls, which no header of the SDK declares.
 * gdFsWaitCompleteReq waits for gdfs's request, and returns the wait's
 * result. it and gdFsStopRd time their waits by syTmrGetCount, so each
 * needs the timer that syTmrInit starts.
 *
 * gdFsReopenHn puts gdfs on fsize bytes from frame address fad, with fid its
 * entry number. it leaves the handle open and idle, with its next read at
 * its first sector. the handle keeps its callbacks and its transfer mode. it
 * returns gdfs. a null gdfs returns null, and so does a gdfs whose request
 * runs. each leaves gdfs as it was */
Sint32 gdFsWaitCompleteReq(GDFS gdfs);
GDFS gdFsReopenHn(GDFS gdfs, Sint32 fid, Sint32 fad, Sint32 fsize);

/* end a DMA from the drive, on its end interrupt. func is the transfer's end
 * callback, or null. it is passed obj first */
void gdGdcG1DmaEnd(GDFS_FUNC func, void *obj);

/* the boot ROM's drive calls, which the drive's functions make.
 *
 * gdGdcReqCmd issues the drive command cmd, which takes its arguments from
 * the words at param. it returns the request's number, or 0 where the drive
 * refuses the command. gdGdcReqDmaTrans moves param[1] bytes of a read's
 * data by DMA to param[0]. it returns 0 where the drive takes the transfer.
 * gdGdcCheckDmaTrans gives that transfer's state, and stores in *size the
 * bytes it has moved.
 *
 * gdGdcGetCmdStat gives gdchn's state, and stores four words at stat. the
 * first two are a failure's error and extended error code. the third is the
 * bytes the request has moved, and the fourth a busy request's wait value.
 * gdGdcGetDrvStat stores the drive's state and its disc's type at info. it
 * returns 1 where it can't */
Sint32 gdGdcReqCmd(Sint32 cmd, void *param);
Sint32 gdGdcGetCmdStat(Sint32 gdchn, void *stat);
void gdGdcExecServer(void);
void gdGdcInitSystem(void);
Sint32 gdGdcGetDrvStat(Sint32 *info);
Sint32 gdGdcReqDmaTrans(Sint32 gdchn, Sint32 *param);
Sint32 gdGdcCheckDmaTrans(Sint32 gdchn, Sint32 *size);
Sint32 gdGdcReadAbort(Sint32 gdchn);
void gdGdcReset(void);

/* the console's settings. each call returns SYD_CFG_OK where it succeeds, and
 * SYD_CFG_NG where it fails. the calls on the language or the sound mode go
 * through syBtCfgSet. they pass it the work that syCfgInit keeps.
 *
 * syCfgInit keeps work, which must stay valid until syCfgExit. a null work
 * fails. syCfgExit forgets the work */
#define SYD_CFG_OK 0
#define SYD_CFG_NG (-1)
Sint32 syCfgInit(void *work);
Sint32 syCfgExit(void);
/* the language syCfgGetLanguage stores */
#define SYD_CFG_JAPANESE 0
#define SYD_CFG_ENGLISH 1
#define SYD_CFG_GERMAN 2
#define SYD_CFG_FRENCH 3
#define SYD_CFG_SPANISH 4
#define SYD_CFG_ITALIAN 5
/* store the console's language in *language. where the console has none, it
 * stores the language of syCfgGetDefaultLanguage's digit. a default that reads
 * -1 gives SYD_CFG_ENGLISH. as shipped, the call also succeeds where
 * syCfgGetDefaultLanguage fails. it then subtracts SYD_CFG_D_JAPANESE from a
 * digit that call never read, and stores -48.
 *
 * the call fails for a null language, before syCfgInit and where syBtCfgSet
 * fails. each failure past the null test leaves SYD_CFG_JAPANESE */
Sint32 syCfgGetLanguage(Sint32 *language);
/* the sound mode syCfgGetSoundMode stores. it fails for a null mode, before
 * syCfgInit and where syBtCfgSet fails. it also fails for a setting that is
 * neither mode. each failure past the null test leaves SYD_CFG_STEREO.
 *
 * syCfgSetSoundMode sets the console's sound mode to mode, and has
 * syBtCfgSet write it to flash. it fails before syCfgInit and for another
 * mode, and each leaves the setting as it was. a write that fails still
 * leaves the new mode in the settings that syBtCfgSet keeps in memory */
#define SYD_CFG_STEREO 0
#define SYD_CFG_MONO 1
Sint32 syCfgGetSoundMode(Sint32 *mode);
Sint32 syCfgSetSoundMode(Sint32 mode);
/* syCfgGetCountryCode, syCfgGetDefaultLanguage and syCfgGetBroadcastType
 * each store a byte of kbCfgGetId's record. the bytes give the console's
 * country, default language and broadcast standard, each an ASCII digit.
 * none of the three needs syCfgInit. each fails for a null pointer and where
 * kbCfgIni fails. the failures past the null test leave SYD_CFG_JAPAN,
 * SYD_CFG_JAPANESE and SYD_CFG_NTSC.
 *
 * a default language's digit is SYD_CFG_D_JAPANESE plus its SYD_CFG_
 * language */
#define SYD_CFG_JAPAN 0x30
#define SYD_CFG_D_JAPANESE 0x30
#define SYD_CFG_NTSC 0x30
Sint32 syCfgGetCountryCode(Sint32 *country);
Sint32 syCfgGetDefaultLanguage(Sint32 *language);
Sint32 syCfgGetBroadcastType(Sint32 *broadcast);

#endif
