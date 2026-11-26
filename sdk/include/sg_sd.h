/* the Dreamcast sound library, version 2.02.12, as far as the sources use
 * it */
#ifndef SG_SD_H
#define SG_SD_H
#include <sg_xpt.h>

typedef enum SDE_ERR {
    SDE_ERR_NOTHING = 0x00000000,
    SDE_ERR_PRM_OVER_RANGE = 0x03000001, /* a parameter out of its range */
    /* a DMA transfer's size that isn't a multiple of 32 bytes, the one case
     * that gives it */
    SDE_ERR_PRM_ILLEGAL_VALUE = 0x03000002,
    /* fewer free slots than a stream port asks for */
    SDE_ERR_MODULE_SLOT_NO_ENOUGH = 0x05000001,
    /* a block whose transfer is under way */
    SDE_ERR_MEMBLK_ALREADY_TRANSFER = 0x06000001,
    SDE_ERR_MEMBLK_QUEUE_NO_ENOUGH = 0x06000101, /* a full queue of blocks */
    SDE_ERR_HANDLE_NO_ENOUGH = 0x07000101, /* no handle left to give */
    /* a handle that addresses no block, or no open port */
    SDE_ERR_HANDLE_ILLEGAL_VALUE = 0x07000102,
    SDE_ERR_HANDLE_NULL = 0x07000103,
    SDE_ERR_PTR_ILLEGAL_VALUE = 0x07000202, /* a pointer of all ones */
    SDE_ERR_PTR_NULL = 0x07000203,
    SDE_ERR_BANK_ILLEGAL_TYPE = 0x09000001, /* a kind that has no banks */
    SDE_ERR_BANK_ILLEGAL_NUM = 0x09000004, /* a bank number out of range */
    /* data that isn't of the kind the call takes */
    SDE_ERR_DATA_ILLEGAL_TYPE = 0x09000101,
    /* data of a version the library doesn't take */
    SDE_ERR_DATA_ILLEGAL_VER = 0x09000102,
    SDE_ERR_BANK_NO_MAPPING = 0x09000110, /* a bank that isn't mapped */
    SDE_ERR_HOST_CMD_BUF_NO_ENOUGH = 0x0a000001, /* a full command queue */
    SDE_ERR_DRV_PROBLEM = 0x0b000001, /* the sound driver doesn't run */
    /* a sound driver of a version the library doesn't take */
    SDE_ERR_DRV_ILLEGAL_VER = 0x0b000002,
    SDE_ERR_DRV_BUSY = 0x0b000003,
    SDE_ERR_NO_INIT = 0x0d000001,
    SDE_ERR_ALREADY_INIT = 0x0d000002,
    /* a sound memory address that isn't a multiple of 4 */
    SDE_ERR_SND_MEM_ADR_ERR = 0x0f010101,
    SDE_ERR_UNKNOW_NUM = 0x7fffffff
} SDE_ERR;

/* the kinds of sound data, each named by its four characters */
#define SDD_CHAR32(d1, d2, d3, d4)                                             \
    ((d1) | ((d2) << 8) | ((d3) << 16) | ((d4) << 24))
typedef enum SDE_DATA_TYPE {
    SDE_DATA_TYPE_DRV = SDD_CHAR32('S', 'D', 'R', 'V'),
    SDE_DATA_TYPE_MULTI_UNIT = SDD_CHAR32('S', 'M', 'L', 'T'),
    SDE_DATA_TYPE_MIDI_DRM_BANK = SDD_CHAR32('S', 'M', 'D', 'B'),
    SDE_DATA_TYPE_MIDI_SEQ_BANK = SDD_CHAR32('S', 'M', 'S', 'B'),
    SDE_DATA_TYPE_MIDI_PRG_BANK = SDD_CHAR32('S', 'M', 'P', 'B'),
    SDE_DATA_TYPE_SHOT_BANK = SDD_CHAR32('S', 'O', 'S', 'B'),
    SDE_DATA_TYPE_PSTM_RING_BUF = SDD_CHAR32('S', 'P', 'S', 'R'),
    SDE_DATA_TYPE_FX_OUT_BANK = SDD_CHAR32('S', 'F', 'O', 'B'),
    SDE_DATA_TYPE_FX_PRG_BANK = SDD_CHAR32('S', 'F', 'P', 'B'),
    SDE_DATA_TYPE_FX_PRG_WRK = SDD_CHAR32('S', 'F', 'P', 'W')
} SDE_DATA_TYPE;

/* what a port is doing: playing, paused, or fading its volume, speed, pan
 * or pitch. SDD_PORT_FLG_TROUBLE marks a port with an error */
#define SDD_BIT(x) (1 << (x))
#define SDD_PORT_FLG_PLAY ((Uint32)SDD_BIT(0))
#define SDD_PORT_FLG_PAUSE ((Uint32)SDD_BIT(1))
#define SDD_PORT_FLG_CHG_VOL ((Uint32)SDD_BIT(2))
#define SDD_PORT_FLG_CHG_SPEED ((Uint32)SDD_BIT(3))
#define SDD_PORT_FLG_CHG_PAN ((Uint32)SDD_BIT(4))
#define SDD_PORT_FLG_CHG_PITCH ((Uint32)SDD_BIT(5))
#define SDD_PORT_FLG_TROUBLE ((Uint32)SDD_BIT(7))
/* a port's error: a request below the priority of what the port plays */
#define SDD_PORT_ERR_PRIORITY_LOW ((Uint32)SDD_BIT(9))

/* a MIDI port's status, as sdMidiGetStat fills it */
typedef struct SDS_MIDI_STAT {
    Sint16 rsv;
    Sint8 m_Vol;
    Sint8 m_Pan;
    Sint8 m_FxLev;
    Sint8 m_DrctLev;
    Sint16 m_Pitch;
    Sint16 m_Speed;
    Uint32 m_TotalBeatTime;
    Sint32 m_CurAdr;
    Uint32 m_Err; /* SDD_PORT_ERR_ bits */
    Uint32 m_Flg; /* SDD_PORT_FLG_ bits */
} SDS_MIDI_STAT;

/* a MIDI message as sdMidiSetMes packs it for sdMidiSendMes. m_Member[0]
 * is the second data byte, and m_Member[1] the first. m_Member[2] holds the
 * port's number in its high four bits and the channel in its low four.
 * m_Member[3] is the kind's number: bits 4 to 6 of the status byte */
typedef struct SDS_MIDI_MES {
    Uint8 m_Member[4];
} SDS_MIDI_MES;
/* the kinds of MIDI message sdMidiSetMes takes: a status byte's high four
 * bits. SDD_MIDI_MES_MONO_PRES and SDD_MIDI_MES_POLY_PRES swap standard
 * MIDI's names, where 0xa0 is polyphonic pressure and 0xd0 channel
 * pressure */
#define SDD_MIDI_MES_NOTE_OFF 0x80
#define SDD_MIDI_MES_NOTE_ON 0x90
#define SDD_MIDI_MES_MONO_PRES 0xa0
#define SDD_MIDI_MES_CTL_CHG 0xb0
#define SDD_MIDI_MES_PRG_CHG 0xc0
#define SDD_MIDI_MES_POLY_PRES 0xd0
#define SDD_MIDI_MES_PITCH_BEND 0xe0

/* a MIDI port's channels, from SDD_MIDI_CH_MIN to SDD_MIDI_CH_MAX */
#define SDD_MIDI_CH_NUM 16
#define SDD_MIDI_CH_MIN 0
#define SDD_MIDI_CH_MAX 15
/* the controllers of a SDD_MIDI_MES_CTL_CHG message. a controller plus
 * SDD_MIDI_CTL_CHG_MSB is the controller of its value's other byte, the low
 * byte in standard MIDI */
#define SDD_MIDI_CTL_CHG_MSB 0x20
#define SDD_MIDI_CTL_CHG_BANK 0x00 /* the bank of the next program change */
#define SDD_MIDI_CTL_CHG_VOL 0x07
#define SDD_MIDI_CTL_CHG_PAN 0x0a

/* the library's error callback. func is the call that failed, by its
 * address, and result its failure. msg is null. every call declared below
 * passes each failure it returns to the callback, where one is set.
 *
 * as shipped, a failure before the library is started reads the callback
 * through a null pointer. the library calls the long it finds there, unless
 * that long is 0 */
typedef void (*SD_ERR_CALLBACK_FUNC)(void *func, SDE_ERR result, char *msg);

/* how many blocks the library has, and how many can wait for a transfer */
#define SDD_MEMBLK_HANDLE_NUM 64
#define SDD_MEMBLK_QUEUE_NUM 64
/* a slot is one of the sound chip's voices. these are the most slots the
 * MIDI ports, the one-shot ports and the stream ports can each be given */
#define SDD_MIDI_SLOT_NUM 64
#define SDD_SHOT_SLOT_NUM 64
#define SDD_PSTM_SLOT_NUM 16
/* the most channels a stream port has. each channel plays in a slot of its
 * own. the stream calls' names say slot for channel, as target_slot does */
#define SDD_PSTM_PORT_SLOT_NUM 4
/* the channel number that stands for every channel of a stream port */
#define SDD_PSTM_PORT_SLOT_ALL ((Sint8)0xff)
/* the filter level that leaves a stream port's channel unfiltered */
#define SDD_PSTM_PORT_SLOT_FILTER_THROUGH ((Sint16) - 1)
/* a stream's samples: 8-bit or 16-bit linear, or 4-bit ADPCM */
typedef enum SDE_PCM_TYPE {
    SDE_PCM_TYPE_8BIT_LINEAR,
    SDE_PCM_TYPE_16BIT_LINEAR,
    SDE_PCM_TYPE_4BIT_ADPCM,
    SDE_PCM_TYPE_8BIT_PCM = SDE_PCM_TYPE_8BIT_LINEAR,
    SDE_PCM_TYPE_16BIT_PCM = SDE_PCM_TYPE_16BIT_LINEAR
} SDE_PCM_TYPE;
/* how many MIDI ports the library has */
#define SDD_MIDI_PORT_NUM 8

/* a port's handle */
typedef struct SDS_PORT_REF {
    void *m_Ref;
} SDS_PORT_REF;
typedef SDS_PORT_REF *SDHANDLE;
typedef SDHANDLE *SDMIDI;
typedef SDHANDLE *SDSHOT; /* a one-shot port's handle */
typedef SDHANDLE *SDPSTM; /* a stream port's handle */

/* the handle of a block: a run of main memory to carry to sound memory */
typedef struct SDS_MEMBLK *SDMEMBLK;
/* what a block calls after its transfer, with the argument sdMemBlkSetPrm
 * took for it */
typedef void (*SD_MEMBLK_CALLBACK_FUNC)(void *first_arg);
/* a callback that calls nothing. a block given it only records its
 * transfer's end, and sdMemBlkTransfer waits for that end */
#define SDD_MEMBLK_SYNC_FUNC ((SD_MEMBLK_CALLBACK_FUNC)0xffffffff)
/* where a block's transfer stands: not started, ended, or under way. a
 * block that waits for its transfer to start is under way already */
typedef enum SDE_MEMBLK_STAT {
    SDE_MEMBLK_STAT_TRANSFER_REMAIN,
    SDE_MEMBLK_STAT_TRANSFER_FINISHED,
    SDE_MEMBLK_STAT_TRANSFER_PROGRESS
} SDE_MEMBLK_STAT;

/* how blocks travel: copied by the CPU, or by the G2 bus's DMA */
typedef enum SDE_MEMBLK_TRANSFER_MODE {
    SDE_MEMBLK_TRANSFER_MODE_CPU,
    SDE_MEMBLK_TRANSFER_MODE_DMA
} SDE_MEMBLK_TRANSFER_MODE;

/* return SDE_ERR_NOTHING for a handle that addresses a block's record. it
 * returns SDE_ERR_NO_INIT before the library is started. a handle that is
 * null or all ones returns SDE_ERR_HANDLE_NULL. any other handle that
 * addresses no block's record returns SDE_ERR_HANDLE_ILLEGAL_VALUE. a free
 * record passes as a made one does, so a destroyed block's handle passes */
SDE_ERR sdMemBlkCheck(SDMEMBLK handle);
/* make a block in the first free one of the library's SDD_MEMBLK_HANDLE_NUM
 * records. store its handle in *handle. the block has no bytes and no callback,
 * and its transfer is not started. every interrupt is masked while it runs.
 * where no record is free, the call stores null and returns
 * SDE_ERR_HANDLE_NO_ENOUGH. as shipped, before the library is started it reads
 * through a null pointer, then returns SDE_ERR_NO_INIT */
SDE_ERR sdMemBlkCreate(SDMEMBLK *handle);
/* free the block's record. it returns sdMemBlkCheck's result for handle, and
 * SDE_ERR_HANDLE_ILLEGAL_VALUE for a block that isn't made.
 *
 * as shipped, the call frees a block whose transfer is under way, and the
 * transfer still runs. it carries the bytes the record holds when it starts.
 * its end calls the callback the record holds then. a block made in the record
 * meanwhile takes the freed block's place in that transfer */
SDE_ERR sdMemBlkDestroy(SDMEMBLK handle);
/* give the block the src_blk_sz bytes at src_blk_ptr to carry. the end of its
 * transfer calls callback_func with callback_first_arg. the call marks the
 * block's transfer not started. every interrupt is masked while it runs. it
 * returns sdMemBlkCheck's result for handle, and
 * SDE_ERR_MEMBLK_ALREADY_TRANSFER for a block whose transfer is under way */
SDE_ERR sdMemBlkSetPrm(SDMEMBLK handle, const void *src_blk_ptr,
                       const Sint32 src_blk_sz,
                       const SD_MEMBLK_CALLBACK_FUNC callback_func,
                       const void *callback_first_arg);
/* store where the block's transfer stands in *stat. every interrupt is
 * masked while it runs. it returns sdMemBlkCheck's result for handle, or
 * SDE_ERR_PTR_NULL for a null stat */
SDE_ERR sdMemBlkGetStat(SDMEMBLK handle, SDE_MEMBLK_STAT *stat);
/* start the block's bytes toward dst_adr of sound memory, the way the
 * transfer mode says. the call marks the block's transfer under way first.
 * every interrupt is masked while it runs, its waits included. it returns
 * sdMemBlkCheck's result for handle, and SDE_ERR_MEMBLK_ALREADY_TRANSFER
 * for a block whose transfer is under way.
 *
 * by CPU, the whole transfer happens before the call returns. the call copies
 * the bytes, marks the transfer ended and calls the block's callback. a copy
 * that sdMoveWrkMemToSndMem refuses returns its result.
 *
 * by DMA, the block joins the queue of blocks that wait. where no block is
 * in transfer, the one that has waited longest starts. a full queue returns
 * SDE_ERR_MEMBLK_QUEUE_NO_ENOUGH, and a start that fails returns
 * sdMemBlkDmaTransfer's result. for a block whose callback is
 * SDD_MEMBLK_SYNC_FUNC, the call waits for its transfer's end, with no time
 * limit.
 *
 * as shipped, a block whose copy, queue or start fails stays marked under
 * way. sdMemBlkSetPrm and this call refuse it with
 * SDE_ERR_MEMBLK_ALREADY_TRANSFER until it is destroyed. a transfer mode
 * that is neither CPU nor DMA starts nothing and leaves the block marked.
 * the call then returns SDE_ERR_NOTHING */
SDE_ERR sdMemBlkTransfer(SDMEMBLK handle, Sint32 dst_adr);
/* set how blocks travel from now on. it first runs sdSysServer until no
 * block is in transfer. every interrupt is masked while it runs. before the
 * library is started it sets nothing. it returns SDE_ERR_NOTHING */
SDE_ERR sdMemBlkSetTransferMode(SDE_MEMBLK_TRANSFER_MODE transfer_mode);

/* carry the block's bytes to where bank bank_num of bank_type is mapped. it
 * returns sdSndMemGetBankStat's result for the bank,
 * SDE_ERR_BANK_NO_MAPPING for a bank that isn't mapped, or
 * sdMemBlkTransfer's result */
SDE_ERR sdBankDownload(SDMEMBLK handle, const SDE_DATA_TYPE bank_type,
                       const Sint8 bank_num);
/* map and carry each bank of the multi unit the block holds. a multi unit
 * is data of SDE_DATA_TYPE_MULTI_UNIT. for each bank, it gives where the
 * bank is mapped in sound memory, and the bank's data. a bank given a
 * negative size keeps the mapping it had. the call carries each bank's
 * data, where there is any, in a block of the library's own. it waits for
 * each transfer to end.
 *
 * it returns SDE_ERR_NO_INIT before the library is started, and
 * SDE_ERR_HANDLE_NULL for a null handle. a block whose bytes are null or all
 * ones returns SDE_ERR_PTR_NULL. bytes that aren't a multi unit return
 * SDE_ERR_DATA_ILLEGAL_TYPE, and a multi unit of another version
 * SDE_ERR_DATA_ILLEGAL_VER. a bank whose mapping or transfer fails ends the
 * call with that failure. the banks before it stay mapped and carried.
 *
 * as shipped, every return reads through handle, even a null one. a failed
 * transfer leaves the library's block marked under way, as sdMemBlkTransfer
 * says. each later call then fails at its first bank that has data, with
 * SDE_ERR_MEMBLK_ALREADY_TRANSFER */
SDE_ERR sdMultiUnitDownload(SDMEMBLK handle);
/* start the sound driver the block holds. the call holds the sound
 * processor in reset and clears sound memory. it carries the driver, less
 * its header, to address 0 of sound memory. it waits until the driver runs,
 * with no time limit, then calls the block's callback. the library must be
 * started and the block given bytes, which the call reads untested.
 *
 * it returns SDE_ERR_HANDLE_NULL for a null handle, and
 * SDE_ERR_DATA_ILLEGAL_TYPE for a block that holds no driver. a driver of
 * another version returns SDE_ERR_DRV_ILLEGAL_VER. a failure changes
 * nothing */
SDE_ERR sdDrvInit(SDMEMBLK handle);
/* test whether the sound driver still runs. the call returns
 * SDE_ERR_DRV_PROBLEM unless each of the sound processor's eight vectors
 * holds a branch. it returns SDE_ERR_NO_INIT before the library is started
 * or while no driver runs */
SDE_ERR sdDrvCheckExecute(void);
/* store in *count_num the count the driver raises while it runs. it stores
 * all ones and returns SDE_ERR_NO_INIT before the library is started or
 * while no driver runs. it returns SDE_ERR_PTR_NULL for a null count_num */
SDE_ERR sdDrvGetExecuteCounter(Uint32 *count_num);
/* start the library. from then on, while the sound driver runs, the library
 * passes queued commands on at each vertical blank. it steps its ports' fades
 * then too. the call queues sdSysSetSlotMax's command for the default counts.
 * every interrupt is masked while it runs. it ignores its parameters: the
 * library uses its own memory and sizes. it returns SDE_ERR_ALREADY_INIT for a
 * library that is started */
SDE_ERR sdLibInit(void *wrk_ptr, Sint32 mem_blk_handle_max,
                  Sint32 second_host_cmd_max);
/* stop the library. the call stops the sound driver, clears sound memory
 * and ends the library's work at the vertical blank. it leaves the sound
 * processor held in reset. every interrupt is masked while it runs.
 * it returns SDE_ERR_NO_INIT before the library is started */
SDE_ERR sdSysFinish(void);
/* set the library's error callback, or none with null. it returns
 * SDE_ERR_NO_INIT before the library is started */
SDE_ERR sdSysSetErrCallback(const SD_ERR_CALLBACK_FUNC callback);
/* pass queued commands on to the sound driver. once the DMA is idle, the call
 * also runs the end of the block that was in transfer. the call does nothing
 * unless the library is started and the sound driver runs. every interrupt is
 * masked while it runs. it returns SDE_ERR_NOTHING */
SDE_ERR sdSysServer(void);
/* pass queued commands on to the sound driver, without the rest of
 * sdSysServer's work. it returns SDE_ERR_NOTHING */
SDE_ERR sdSysFlushHostCmd(void);
/* how hard sdSysHostCmdBufFlush tries */
typedef enum SDE_HOST_CMD_FLUSH_MODE {
    SDE_HOST_CMD_FLUSH_MODE_NORMAL,
    SDE_HOST_CMD_FLUSH_MODE_FORCE,
    SDE_HOST_CMD_FLUSH_MODE_FORCE_ALL
} SDE_HOST_CMD_FLUSH_MODE;
/* pass queued commands on to the sound driver, as sdSysFlushHostCmd does, as
 * many times as mode says. the call returns SDE_ERR_NOTHING. the library must
 * be started: the call reads the library's work untested.
 *
 * SDE_HOST_CMD_FLUSH_MODE_NORMAL passes them once.
 * SDE_HOST_CMD_FLUSH_MODE_FORCE passes them again while the driver is busy.
 * as shipped, the call never learns that the driver is busy, so FORCE too
 * passes them once. SDE_HOST_CMD_FLUSH_MODE_FORCE_ALL passes them until
 * none is queued, with no time limit. it never returns while a command is
 * queued and no driver runs */
SDE_ERR sdSysHostCmdBufFlush(const SDE_HOST_CMD_FLUSH_MODE mode);
/* queue the command that tells the sound driver how many slots each kind of
 * port may use. then record the three counts. it returns SDE_ERR_NO_INIT before
 * the library is started. a negative count, or one above its kind's most,
 * returns SDE_ERR_PRM_OVER_RANGE. so do counts whose sum is above 64, the
 * chip's slots in all. where the command can't be queued, the call returns the
 * queue's failure and records nothing */
SDE_ERR sdSysSetSlotMax(Sint32 midi_slot_max, Sint32 shot_slot_max,
                        Sint32 pstm_slot_max);

/* whether sounds are placed left and right, or all play in the middle */
typedef enum SDE_PAN_MODE {
    SDE_PAN_MODE_MONO,
    SDE_PAN_MODE_STEREO,
    SDE_PAN_MODE_DISABLE = SDE_PAN_MODE_MONO,
    SDE_PAN_MODE_ENABLE = SDE_PAN_MODE_STEREO
} SDE_PAN_MODE;

/* each sdSnd call below queues a command for the sound driver about every
 * sound. it returns SDE_ERR_NO_INIT before the library is started. otherwise
 * it returns the result of queuing its command, unless its comment says
 * otherwise */

/* set the volume of all sound to vol, 0 to 15. the command carries only
 * vol's low four bits */
SDE_ERR sdSndSetMasterVol(const Sint8 vol);
/* set whether sounds are placed left and right. it returns
 * SDE_ERR_PRM_OVER_RANGE for a pan_mode that is neither SDE_PAN_MODE_MONO
 * nor SDE_PAN_MODE_STEREO */
SDE_ERR sdSndSetPanMode(const SDE_PAN_MODE pan_mode);
/* choose the effect program and the effect output, each by its number, with
 * a command for each. it returns SDE_ERR_PRM_OVER_RANGE for a negative
 * number. where the program's command can't be queued, the call returns the
 * queue's failure. it then queues no command for the output */
SDE_ERR sdSndSetFxPrg(const Sint8 new_fx_prg_num, const Sint8 new_fx_out_num);
/* choose no effect program */
SDE_ERR sdSndClearFxPrg(void);
/* queue the command that stops every sound. where it is queued, the call
 * also sets every stream port's transfers back, as a stop does */
SDE_ERR sdSndStopAll(void);

/* a stream port plays up to SDD_PSTM_PORT_SLOT_NUM channels of samples that
 * its caller keeps feeding it. each channel has a slot, and a ring buffer in
 * sound memory. in the calls below, target_slot is a channel's number, from
 * 0.
 *
 * a channel's ring buffer is the start of a bank of
 * SDE_DATA_TYPE_PSTM_RING_BUF. the buffer's size is the bank's, cut to a
 * limit for the port's kind of samples. the limit is 0xf000 bytes for 8-bit
 * samples, 0x1f000 for 16-bit and 0x7000 for ADPCM. as shipped, sdPstmPlay
 * keeps a pcm_type it refuses, and the limit is then an uninitialized value.
 *
 * a stop sets a channel's transfers back as they were when the port opened.
 * the next transfer starts at the ring buffer's first byte. until sdPstmPlay,
 * the whole buffer is room for transfers.
 *
 * each call returns SDE_ERR_NO_INIT before the library is started. each but
 * sdPstmOpenPort returns SDE_ERR_HANDLE_NULL for a null handle. a call's own
 * comment gives any other failure */

/* open a stream port of use_slot_num channels, 1 to SDD_PSTM_PORT_SLOT_NUM,
 * and store its handle in *handle. an int follows use_slot_num for each
 * channel: the number of its ring buffer's bank, 0 to 15. the port takes
 * the free stream slots with the lowest numbers. every interrupt is masked
 * while it runs.
 *
 * it returns SDE_ERR_PRM_OVER_RANGE for a use_slot_num or a bank outside its
 * range. it
 * returns SDE_ERR_MODULE_SLOT_NO_ENOUGH where fewer than use_slot_num
 * stream slots are free. where every stream port is open, it stores null and
 * returns SDE_ERR_HANDLE_NO_ENOUGH. the error callback gets that failure twice.
 * the call doesn't test handle for null */
SDE_ERR sdPstmOpenPort(SDPSTM *handle, const Sint8 use_slot_num, ...);
/* stop the port as sdPstmStop does, reset every channel's levels, give the
 * port's slots back and close it. where the stop fails, the call returns
 * the stop's failure and closes nothing. a reset that fails doesn't stop
 * the close.
 *
 * the port must be open. the call refuses a closed port with
 * SDE_ERR_HANDLE_ILLEGAL_VALUE. it does so only after the stop, the reset and
 * the return of the slots. as shipped, a second close corrupts the library's
 * count of used stream slots. the count becomes -1 minus the port's slots,
 * and sdPstmOpenPort then counts more slots free than there are */
SDE_ERR sdPstmClosePort(SDPSTM handle);
/* make the port play only the first use_slot_num of the channels it was opened
 * with. then reset every channel's levels as sdPstmResetPrm does. it returns
 * SDE_ERR_PRM_OVER_RANGE for a use_slot_num below 1 or above the count the
 * port was opened with */
SDE_ERR sdPstmSetUseSlotNum(SDPSTM handle, const Sint8 use_slot_num);
/* set what channel target_slot starts with at sdPstmPlay, or every channel
 * with SDD_PSTM_PORT_SLOT_ALL. base_vol and base_pan are its volume and
 * pan, and base_drct_lev its direct level. the channel feeds effect channel
 * fx_in_ch at base_fx_lev. base_filter_lev and base_filter_q set its filter,
 * and a base_filter_lev of SDD_PSTM_PORT_SLOT_FILTER_THROUGH leaves it
 * unfiltered. the call queues no command. it returns SDE_ERR_PRM_OVER_RANGE
 * for a target_slot that is neither SDD_PSTM_PORT_SLOT_ALL nor one of the
 * channels the port plays */
SDE_ERR sdPstmSetBasePrm(SDPSTM handle, const Sint8 target_slot,
                         const Sint8 base_vol, const Sint8 base_pan,
                         const Sint8 fx_in_ch, const Sint8 base_fx_lev,
                         const Sint8 base_drct_lev,
                         const Sint16 base_filter_lev,
                         const Sint8 base_filter_q);
/* queue, for each channel the port plays, the command that starts it
 * playing its ring buffer. the channel plays samples of pcm_type at freq
 * hertz, with the levels and filter sdPstmSetBasePrm set. it returns
 * SDE_ERR_PRM_OVER_RANGE for a pcm_type that isn't one of the three. a command
 * that can't be queued ends the call with the queue's failure. the channels
 * before it play. as shipped, the call keeps pcm_type and freq even where it
 * refuses pcm_type */
SDE_ERR sdPstmPlay(SDPSTM handle, const SDE_PCM_TYPE pcm_type,
                   const Uint16 freq);
/* queue, for each channel the port plays, the command that stops it. then set
 * the port's transfers back, as a stop does. a command that can't be queued
 * ends the call with the queue's failure. the transfers then stay as they
 * were */
SDE_ERR sdPstmStop(SDPSTM handle);
/* queue the command that resets the levels of channel target_slot in the
 * driver. then set the library's record of them to 0. with
 * SDD_PSTM_PORT_SLOT_ALL it does so for each channel the port was opened with,
 * in turn.
 *
 * a target_slot that is neither SDD_PSTM_PORT_SLOT_ALL nor one of those
 * channels resets nothing, and the call returns SDE_ERR_NOTHING. a command that
 * can't be queued ends the call with the queue's failure. the channels before
 * it stay reset, and the rest keep their levels */
SDE_ERR sdPstmResetPrm(SDPSTM handle, const Sint8 target_slot);
/* store in *cur_smp_adr how far into its ring buffer channel target_slot
 * plays, in bytes. the call converts the sound driver's count of samples by
 * the kind sdPstmPlay last took. a sample takes 2 bytes at 16 bits, 1 at 8
 * bits and half a byte in ADPCM. it returns SDE_ERR_PTR_NULL for a null
 * cur_smp_adr. it returns SDE_ERR_PRM_OVER_RANGE for a target_slot that isn't
 * one of the channels the port plays */
SDE_ERR sdPstmGetCurAdr(SDPSTM handle, const Sint8 target_slot,
                        Sint32 *cur_smp_adr);
/* store in *flg whether data_sz bytes fit in the room of channel target_slot's
 * ring buffer. the room is the bytes the next transfer may fill. once the
 * channel plays, the room runs from the last transfer's end up to where the
 * channel plays. where the channel plays below that end, the room runs to the
 * buffer's end instead. it never wraps to the buffer's start. before
 * sdPstmPlay, a data_sz above the whole buffer returns
 * SDE_ERR_PRM_OVER_RANGE, with *flg FALSE.
 *
 * it returns SDE_ERR_PTR_NULL for a null flg, and doesn't test
 * target_slot */
SDE_ERR sdPstmIsTransferWaveData(SDPSTM handle, const Sint8 target_slot,
                                 const Sint32 data_sz, Bool *flg);
/* start the transfer of memblk's bytes into channel target_slot's ring buffer,
 * at the last transfer's end. a transfer that reaches the buffer's end makes
 * the next start at the buffer's first byte. a transfer that starts at the
 * first byte leaves a copy of the buffer's first long due. the next call that
 * passes the size test writes it just past the buffer's end. that long must be
 * free for it.
 *
 * the call swaps memblk's callback for one of the library's. it does so even
 * where it then refuses target_slot or the size. at the transfer's end, that
 * callback puts back and calls the one sdMemBlkSetPrm gave. a channel holds one
 * block. a block given before the last one's transfer ends takes its place. the
 * older transfer's end then calls the newer block's callback. the older block's
 * callback is never called.
 *
 * a null memblk returns SDE_ERR_HANDLE_NULL. a target_slot at or above the
 * count of channels the port plays returns SDE_ERR_PRM_OVER_RANGE. so do bytes
 * that would pass the buffer's end. the call doesn't test target_slot for a
 * negative.
 *
 * as shipped, the call returns SDE_ERR_NOTHING whatever sdMemBlkTransfer
 * returns. a memblk that sdMemBlkTransfer refuses still moves the next
 * transfer's start on by its size.
 *
 * as shipped, a target_slot of SDD_PSTM_PORT_SLOT_NUM or more makes the call
 * write past a table of its own. it refuses the slot only after that write */
SDE_ERR sdPstmTransferWaveData(SDPSTM handle, const Sint8 target_slot,
                               SDMEMBLK memblk);
/* move the volume, the pan or the pitch of channel target_slot toward a new
 * value, the target. SDD_PSTM_PORT_SLOT_ALL moves every channel's. each
 * channel's new level then goes to the driver in a command.
 *
 * a fade_time of 0 sets a level to its target at once. a fade_time above 0
 * moves the level one step toward the target. the library then moves it a
 * step more at each vertical blank, until it reaches the target. a step is
 * (target - level) / (fade_time / 16), in 256ths of a level, each quotient
 * truncated. where the step is 0, the call sets the level to the target at
 * once. as shipped, a fade_time of 1 to 15 divides by 0, whose result C
 * leaves undefined.
 *
 * each returns SDE_ERR_PRM_OVER_RANGE for a negative fade_time. it does the
 * same for a target_slot that is neither SDD_PSTM_PORT_SLOT_ALL nor a channel
 * the port plays. sdPstmSetVol and sdPstmSetPan test fade_time only after they
 * record the target of the first channel they set. a command that can't be
 * queued ends the call with the queue's failure. its channel's levels are set
 * by then. the later channels keep theirs */
SDE_ERR sdPstmSetVol(SDPSTM handle, const Sint8 target_slot, const Sint8 vol,
                     const Sint32 fade_time);
SDE_ERR sdPstmSetPan(SDPSTM handle, const Sint8 target_slot, const Sint8 pan,
                     const Sint32 fade_time);
SDE_ERR sdPstmSetPitch(SDPSTM handle, const Sint8 target_slot,
                       const Sint16 pitch, const Sint32 fade_time);
/* sdPstmSetDrctLev queues the command that gives the driver the direct level
 * of channel target_slot. sdPstmSetFxCh queues the one that makes the channel
 * feed effect channel fx_in_ch at level fx_base_lev. a fx_in_ch of -1 makes
 * it feed none. a fx_base_lev of 127 goes as 126: the command's byte for 127
 * means none.
 *
 * with SDD_PSTM_PORT_SLOT_ALL, each queues a command for every channel the
 * port plays. a command that can't be queued doesn't stop the others. the
 * call returns the result of queuing the last. each returns
 * SDE_ERR_PRM_OVER_RANGE for a target_slot that is neither
 * SDD_PSTM_PORT_SLOT_ALL nor one of the channels the port plays */
SDE_ERR sdPstmSetDrctLev(SDPSTM handle, const Sint8 target_slot,
                         const Sint8 drct_lev);
SDE_ERR sdPstmSetFxCh(SDPSTM handle, const Sint8 target_slot,
                      const Sint8 fx_in_ch, const Sint8 fx_base_lev);

/* store in *allocate_ptr where bank bank_num of data_type is mapped in
 * sound memory, and its size in *allocate_size. each is as the library
 * recorded it. it returns SDE_ERR_NO_INIT before the library is started. it
 * returns SDE_ERR_PTR_NULL for a null allocate_ptr, and SDE_ERR_PRM_OVER_RANGE
 * for a null allocate_size. it returns SDE_ERR_BANK_ILLEGAL_NUM for a bank_num
 * outside 0 to 15. the three kinds of effect data take only bank 0. for a
 * data_type that has no banks it stores 0 and -1 and returns
 * SDE_ERR_BANK_ILLEGAL_TYPE */
SDE_ERR sdSndMemGetBankStat(const SDE_DATA_TYPE data_type, const Sint8 bank_num,
                            Sint32 *allocate_ptr, Sint32 *allocate_size);

/* each sdMidi call returns SDE_ERR_NO_INIT before the library is started. each
 * that takes a handle returns SDE_ERR_HANDLE_NULL for a null one, unless its
 * comment says otherwise */

/* open the first MIDI port that isn't open, and store its handle in
 * *handle. where all SDD_MIDI_PORT_NUM ports are open, it stores null and
 * returns SDE_ERR_HANDLE_NO_ENOUGH. the error callback gets that failure twice.
 * where handle is null, the call opens nothing and returns SDE_ERR_NOTHING */
SDE_ERR sdMidiOpenPort(SDMIDI *handle);
/* read the port's whole status into *midi_stat, with midi_stat->rsv 0. it
 * returns SDE_ERR_PTR_NULL for a null midi_stat */
SDE_ERR sdMidiGetStat(SDMIDI handle, SDS_MIDI_STAT *midi_stat);
/* store in *cur_adr the port's place in its sequence, as the sound driver
 * reports it. it returns SDE_ERR_PTR_NULL for a null cur_adr */
SDE_ERR sdMidiGetCurAdr(SDMIDI handle, Sint32 *cur_adr);
/* store in *cur_beat_time the beats the port's sequence has played, as the
 * sound driver reports them. it doesn't test cur_beat_time for null */
SDE_ERR sdMidiGetTotalBeatTime(SDMIDI handle, Uint32 *cur_beat_time);
/* queue the command that resets every MIDI port's levels in the sound
 * driver. where it is queued, the call resets the library's record of the
 * levels too. as shipped, it resets only those of the open port with the
 * lowest number. it returns the result of queuing the command */
SDE_ERR sdMidiResetAllPrm(void);

/* fill *midi_mes_ptr with a MIDI message for handle's port. midi_mes is the
 * message's status byte. an int follows for each data byte: one for
 * SDD_MIDI_MES_PRG_CHG and SDD_MIDI_MES_MONO_PRES, and two for any other. the
 * call sends nothing: sdMidiSendMes does.
 *
 * a system message, whose status byte is 0xf0 or more, has no int after it. the
 * call fills *midi_mes_ptr with 0 for one, and tests nothing more. for any
 * other message it returns SDE_ERR_PRM_OVER_RANGE for a midi_mes below 0x80 or
 * a data byte above 0x7f. it doesn't test midi_mes_ptr for null */
SDE_ERR sdMidiSetMes(SDMIDI handle, SDS_MIDI_MES *midi_mes_ptr,
                     const Uint8 midi_mes, ...);

/* every call from here on queues a command for the sound driver about the
 * MIDI port of handle. a call's own comment gives any failure past the two
 * every sdMidi call gives. otherwise it returns the result of queuing the
 * command */

/* play sequence data_num of sequence bank bank_num on the port, at priority
 * 0 to 31. it returns SDE_ERR_PRM_OVER_RANGE for a negative bank_num or
 * data_num, or a priority outside that range. it returns
 * SDE_ERR_BANK_NO_MAPPING for a bank that isn't mapped. as shipped, a bank_num
 * of 16 or more isn't refused. sdSndMemGetBankStat passes
 * SDE_ERR_BANK_ILLEGAL_NUM to the error callback for it and stores no mapping.
 * the call then tests a mapping it never read, and may queue the play */
SDE_ERR sdMidiPlay(SDMIDI handle, const Sint8 bank_num, const Sint8 data_num,
                   const Sint8 priority);
SDE_ERR sdMidiStop(SDMIDI handle);
SDE_ERR sdMidiPause(SDMIDI handle);
SDE_ERR sdMidiContinue(SDMIDI handle);
/* queue the MIDI message at midi_mes_ptr for the sound driver, at priority
 * 0 to 31. it returns SDE_ERR_PRM_OVER_RANGE for another priority. the
 * message goes to the port that sdMidiSetMes packed into it: handle is only
 * tested for null. the call doesn't test midi_mes_ptr for null */
SDE_ERR sdMidiSendMes(SDMIDI handle, const SDS_MIDI_MES *midi_mes_ptr,
                      const Sint8 priority);
/* move the port's volume, pan or pitch toward a new value, the target.
 * fade_time moves the level as sdPstmSetVol's moves a channel's. the call
 * records the target first, even where it then fails. a negative fade_time
 * returns SDE_ERR_PRM_OVER_RANGE. a call that doesn't fail queues a command
 * with the level the port has reached */
SDE_ERR sdMidiSetVol(SDMIDI handle, const Sint8 vol, const Sint32 fade_time);
SDE_ERR sdMidiSetPan(SDMIDI handle, const Sint8 pan, const Sint32 fade_time);
SDE_ERR sdMidiSetPitch(SDMIDI handle, const Sint16 pitch,
                       const Sint32 fade_time);
/* set the port's effect input level, at once. the library's record of the
 * port takes the level only if the command is queued */
SDE_ERR sdMidiSetFxLev(SDMIDI handle, const Sint8 fx_lev);

#endif
