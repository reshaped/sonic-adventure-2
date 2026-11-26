/* the sound library's own records and calls, which its members share */
#ifndef SD_WORK_H
#define SD_WORK_H
#include <sg_chain.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

/* the byte the library clears a record with: all ones, not 0. a field no
 * call has set since then reads as all ones */
#define SD_UNSET_FILL 0xff

/* a command for the sound driver: its number and its parameters */
struct sd_host_cmd {
    Uint32 cmd;
    Uint8 prm[16];
};
/* @hack spelling: the parameter through a byte view of the whole command,
 * where command.prm[n] would do */
#define SD_HOST_CMD_PRM(command, n)                                            \
    (((Uint8 *)&(command))[sizeof((command).cmd) + (n)])
/* @hack spelling: the number through a view of the whole command as a long,
 * where command.cmd would do */
#define SD_HOST_CMD_NO(command) (*(Uint32 *)&(command))

/* a command in the library's queue. valid is TRUE from the time
 * sdHostCmdSet fills the entry until sdHostCmdPrimBufSet moves its command
 * to the batch. time is syTmrGetCount's count when the entry was filled */
struct sd_host_cmd_entry {
    Bool valid;
    Uint32 time;
    struct sd_host_cmd cmd;
    Uint8 unk_1c[4];
};
/* how many entries the library's queue has */
#define SD_HOST_CMD_QUEUE_NUM 512
/* the most commands one sdHostCmd2ndBufSend moves to the batch */
#define SD_HOST_CMD_SEND_MAX 32

/* a command as the sound driver reads it: the low byte of its number, and
 * its first 14 parameter bytes */
struct sd_driver_cmd {
    Uint8 cmd;
    Uint8 unk_01;
    Uint8 prm[14];
};
/* the most commands the batch holds */
#define SD_BATCH_MAX 32

/* the commands on their way to the sound driver. the batch is the commands
 * that one transfer carries to sound memory. sdHostCmdSet queues a command.
 * sdHostCmd2ndBufSend moves the oldest queued commands to the batch.
 *
 * batch points to the batch's batch_num commands. the queue is a ring:
 * queue points to queue_max entries, and queue_num of them hold a command.
 * sdHostCmdSet fills entry queue_write, and sdHostCmd2ndBufSend takes
 * entry queue_read */
struct sd_host_cmd_buf {
    struct sd_driver_cmd *batch;
    Sint32 batch_num;
    struct sd_host_cmd_entry *queue;
    Sint32 queue_max;
    Sint32 queue_num;
    Sint32 queue_write;
    Sint32 queue_read;
    /* TRUE from the time sdHostCmd2ndBufSend starts to fill the batch until
     * sdHostCmdPrimBufFlushCallback runs */
    Bool sending;
};

/* the commands' numbers */
#define SD_HOST_CMD_MIDI_PLAY 0x01
#define SD_HOST_CMD_MIDI_STOP 0x02
#define SD_HOST_CMD_MIDI_PAUSE 0x03
#define SD_HOST_CMD_MIDI_CONTINUE 0x04
#define SD_HOST_CMD_MIDI_VOL 0x05
#define SD_HOST_CMD_MIDI_PAN 0x06
#define SD_HOST_CMD_MIDI_SPEED 0x07
#define SD_HOST_CMD_MIDI_PITCH 0x08
#define SD_HOST_CMD_MIDI_SET_FX_LEV 0x0a
#define SD_HOST_CMD_MIDI_RESET_PRM 0x0e
#define SD_HOST_CMD_SHOT_VOL 0x15
#define SD_HOST_CMD_SHOT_PAN 0x16
#define SD_HOST_CMD_SHOT_PITCH 0x18
#define SD_HOST_CMD_PSTM_PLAY 0x21
#define SD_HOST_CMD_PSTM_STOP 0x22
#define SD_HOST_CMD_PSTM_VOL 0x25
#define SD_HOST_CMD_PSTM_PAN 0x26
#define SD_HOST_CMD_PSTM_PITCH 0x28
#define SD_HOST_CMD_PSTM_FX_CH 0x29
#define SD_HOST_CMD_PSTM_DRCT_LEV 0x2b
#define SD_HOST_CMD_PSTM_RESET_PRM 0x2e
#define SD_HOST_CMD_SND_STOP_ALL 0x80
#define SD_HOST_CMD_SET_MASTER_VOL 0x81
#define SD_HOST_CMD_SET_FX_OUT 0x82
#define SD_HOST_CMD_SET_FX_PRG 0x84
#define SD_HOST_CMD_CLEAR_FX_PRG 0x85
#define SD_HOST_CMD_MIDI_SEND_MES 0x86
#define SD_HOST_CMD_SET_PAN_MODE 0x8a
#define SD_HOST_CMD_SET_SLOT_MAX 0x8d
#define SD_HOST_CMD_DRV_FINISH 0x8f

/* the most slots the MIDI, the one-shot and the stream ports may be given
 * together */
#define SD_SLOT_NUM 64
/* the highest priority sdMidiPlay and sdMidiSendMes take. a command holds
 * the priority in the top five bits of a byte, shifted up by
 * SD_MIDI_PRIORITY_SHIFT */
#define SD_MIDI_PRIORITY_MAX 31
#define SD_MIDI_PRIORITY_SHIFT 3
/* the port number that stands for every port in a command */
#define SD_PORT_ALL 255
/* the kinds of port, each named by its four characters */
#define SD_PORT_KIND_GLOBAL SDD_CHAR32('G', 'L', 'B', 'L')
#define SD_PORT_KIND_GDDA SDD_CHAR32('G', 'D', 'D', 'A')
#define SD_PORT_KIND_MIDI SDD_CHAR32('M', 'I', 'D', 'I')
#define SD_PORT_KIND_SHOT SDD_CHAR32('S', 'H', 'O', 'T')
#define SD_PORT_KIND_PSTM SDD_CHAR32('P', 'S', 'T', 'M')

/* the bias a command adds to each value it carries. a command carries a
 * level in one byte, and a pitch or a speed in two */
#define SD_LEVEL_BIAS 0x80
#define SD_PITCH_BIAS 0x8000
#define SD_SPEED_BIAS 0x8000

/* what every port's record opens with: the port's number within its kind.
 * a port's handle is the address of its record */
struct sd_port_head {
    Sint8 port;
    Uint8 unk_01;
};

/* a block of a port's levels. sd_port keeps three: target, cur and step.
 * in those, vol, pan, pitch and speed hold SD_LEVEL_UNIT times the value
 * their sdDrv calls take. fx_lev holds its level unscaled */
#define SD_LEVEL_UNIT 256
struct sd_port_levels {
    Sint32 vol;
    Sint32 pan;
    Sint32 fx_ch;
    Sint32 unk_0c;
    Sint32 fx_lev;
    Sint32 drct_lev;
    Sint32 pitch;
    Sint32 speed;
};

/* a port's record, the same for every kind. target holds the levels last
 * asked for and cur the levels the port has reached. step holds, for each
 * level, what a call that fades it adds to cur */
struct sd_port {
    struct sd_port_head head;
    struct sd_port_levels target;
    struct sd_port_levels cur;
    struct sd_port_levels step;
    Uint32 kind; /* one of the SD_PORT_KIND_ constants */
    /* sdModuleRefInit leaves it all ones. sdModuleOpenPort sets it to 0,
     * and no other call writes it. sdMidiPause and sdMidiStop send the
     * driver 0 for an unk_68 of 0, and 1 for any other. so a port that has
     * ever been opened sends 0, even once it is closed */
    Sint32 unk_68;
    /* 1 while the port is open, from sdModuleOpenPort until
     * sdModuleClosePort, which sets it to 0. every test of opened compares
     * it with 1, so any other value means closed. sdModuleRefInit sets it to
     * 0 in every record but the last stream port's. that record keeps all
     * ones until the port is first opened */
    Uint32 opened;
};

/* the most bytes of its ring buffer a stream uses, for each kind of
 * samples */
#define SD_PSTM_RING_MAX_8BIT 0xf000
#define SD_PSTM_RING_MAX_16BIT 0x1f000
#define SD_PSTM_RING_MAX_ADPCM 0x7000
/* the base pan of a channel in the middle, which sdPstmOpenPort gives every
 * channel */
#define SD_PSTM_BASE_PAN_CENTER 64

/* how many one-shot ports and how many stream ports the library has */
#define SD_SHOT_PORT_NUM 64
#define SD_PSTM_PORT_NUM 16
/* how many ports there are: the global port, the GDDA port, and the MIDI,
 * one-shot and stream ports */
#define SD_PORT_NUM                                                            \
    (2 + SDD_MIDI_PORT_NUM + SD_SHOT_PORT_NUM + SD_PSTM_PORT_NUM)
/* every port's record. all holds them in one array, and by_kind names each
 * kind's part of it */
union sd_module_refs {
    struct sd_port all[SD_PORT_NUM];
    struct {
        struct sd_port global;
        struct sd_port gdda;
        struct sd_port midi[SDD_MIDI_PORT_NUM];
        struct sd_port shot[SD_SHOT_PORT_NUM];
        struct sd_port pstm[SD_PSTM_PORT_NUM];
    } by_kind;
};

/* a stream port's own record, which the library keeps beside the port's
 * sd_port. the port plays up to SDD_PSTM_PORT_SLOT_NUM channels, each in a
 * slot of the sound driver's and from a ring buffer in sound memory.
 * target, cur and step hold a channel's levels each, as sd_port's hold the
 * port's */
struct sd_pstm_port_ext {
    Uint32 kind; /* SD_PORT_KIND_PSTM */
    Uint32 port; /* the port's number among the stream ports */
    /* how many channels the port was opened with, each in a stream slot of
     * its own */
    Sint8 slot_max;
    /* how many of those channels the port plays, from the first */
    Sint8 slot_num;
    Uint8 unk_00a[2];
    /* the levels each channel starts with, each as sdPstmSetBasePrm took
     * it */
    struct sd_port_levels base[SDD_PSTM_PORT_SLOT_NUM];
    struct sd_port_levels target[SDD_PSTM_PORT_SLOT_NUM];
    struct sd_port_levels cur[SDD_PSTM_PORT_SLOT_NUM];
    struct sd_port_levels step[SDD_PSTM_PORT_SLOT_NUM];
    /* the filter level each channel starts with: the level sdPstmSetBasePrm
     * took, shifted right by 2, or SDD_PSTM_PORT_SLOT_FILTER_THROUGH */
    Sint16 base_filter_lev[SDD_PSTM_PORT_SLOT_NUM];
    /* the filter Q each channel starts with */
    Sint8 base_filter_q[SDD_PSTM_PORT_SLOT_NUM];
    /* each channel's ring buffer: its bank number among those of
     * SDE_DATA_TYPE_PSTM_RING_BUF */
    Sint8 ring_bank[SDD_PSTM_PORT_SLOT_NUM];
    /* the number of the stream slot each channel plays in, or -1 for a
     * channel without one */
    Sint8 slot_no[SDD_PSTM_PORT_SLOT_NUM];
    SDE_PCM_TYPE pcm_type; /* the kind of samples sdPstmPlay last took */
    Sint32 freq; /* the sample rate sdPstmPlay last took */
    /* TRUE for a channel that sdPstmPlay hasn't started since the port was
     * opened or stopped */
    Bool first_transfer[SDD_PSTM_PORT_SLOT_NUM];
    /* how far into its ring buffer each channel's next transfer goes */
    Sint32 transfer_adr[SDD_PSTM_PORT_SLOT_NUM];
    /* how many times each channel's transfers have reached its ring
     * buffer's end */
    Sint32 transfer_wraps[SDD_PSTM_PORT_SLOT_NUM];
    /* the kind of samples of a stream that the port plays from main
     * memory */
    SDE_PCM_TYPE on_mem_pcm_type;
    Sint32 on_mem_pcm_freq; /* the stream's sample rate, in hertz */
    /* TRUE while sdPstmPlay is still to be called for the stream */
    Bool on_mem_play_req;
    /* TRUE while the port plays a stream from main memory */
    Bool on_mem;
    /* the bytes one transfer carries to each channel */
    Sint32 on_mem_transfer_size;
    /* TRUE for a stream that starts over at its data's end */
    Bool on_mem_loop;
    Sint32 unk_270[SDD_PSTM_PORT_SLOT_NUM];
    /* for each channel, the bytes of its data already carried */
    Sint32 on_mem_transfer_ofs[SDD_PSTM_PORT_SLOT_NUM];
    /* for each channel, the block that holds its data */
    SDMEMBLK on_mem_blk[SDD_PSTM_PORT_SLOT_NUM];
};

/* a block: bytes of main memory that the library carries to sound memory,
 * and the callback that follows. its handle is the address of its record */
struct sd_memblk {
    /* SD_MEMBLK_MADE from sdMemBlkCreate until sdMemBlkDestroy, and
     * SD_MEMBLK_FREE otherwise */
    Uint32 tag;
    SDE_MEMBLK_STAT stat;
    /* TRUE once the transfer of a block whose callback is
     * SDD_MEMBLK_SYNC_FUNC has ended. each transfer starts it FALSE */
    Bool ended;
    /* what sdMemBlkSetPrm was given: the bytes, their count, the callback
     * and its argument */
    const void *set_ptr;
    Sint32 set_size;
    SD_MEMBLK_CALLBACK_FUNC set_callback;
    const void *set_arg;
    /* the bytes, their count, the callback and its argument that the
     * block's transfer uses */
    void *ptr;
    Sint32 size;
    SD_MEMBLK_CALLBACK_FUNC callback;
    void *arg;
    /* when the block's last transfer by DMA started, in microseconds of
     * syTmrGetCount's count, or SD_MEMBLK_NO_START */
    Uint32 start_time;
    /* when the block's last transfer ended, in the same unit, or 0 from
     * sdMemBlkInit */
    Uint32 end_time;
    Uint32 unk_34[3];
};
/* the start_time of a block that no DMA has carried */
#define SD_MEMBLK_NO_START 0xffffffff
/* a record's tags, each named by its four characters */
#define SD_MEMBLK_MADE SDD_CHAR32('M', 'B', 'L', 'K')
#define SD_MEMBLK_FREE SDD_CHAR32('m', 'b', 'l', 'k')

/* a block that waits for its transfer, and where in sound memory it goes */
struct sd_memblk_queue_entry {
    SDMEMBLK blk;
    void *dst;
};
/* the blocks that wait for their transfer: a ring of max entries at
 * entries, num of them waiting. sdMemBlkQueuePush fills entry write, and
 * sdMemBlkQueuePop takes entry read */
struct sd_memblk_queue {
    struct sd_memblk_queue_entry *entries;
    Sint32 max;
    Sint32 write;
    Sint32 read;
    Sint32 num;
};

/* the sound driver's record of a MIDI port, in sound memory. vol, pan,
 * fx_lev and drct_lev each hold a level plus SD_LEVEL_BIAS. pitch and speed
 * each hold a value plus its bias in their low 16 bits */
struct sd_snd_midi_status {
    Uint8 flg; /* the port's SDD_PORT_FLG_ bits, as the driver sets them */
    Uint8 unk_01;
    Uint16 err; /* the port's SDD_PORT_ERR_ bits */
    Sint8 vol;
    Uint8 unk_05;
    Sint8 pan;
    Uint8 unk_07[2];
    Sint8 fx_lev;
    Uint8 unk_0a;
    Sint8 drct_lev;
    Uint8 unk_0c[4];
    Sint32 pitch;
    Sint32 speed;
    Uint8 unk_18[4];
    Sint32 cur_adr;
    Uint32 total_beat_time;
    Uint8 unk_24[0x1c];
};
/* the sound driver's record of a slot, in sound memory */
struct sd_snd_slot_status {
    Uint8 flg; /* the SDD_PORT_FLG_ bits of what plays in the slot */
    Uint8 unk_01[0x1b];
    /* how many samples into its ring buffer a stream's channel plays */
    Sint32 cur_adr;
    Uint8 unk_20[0x20];
};
/* the sound driver's status, in sound memory */
struct sd_snd_status {
    struct sd_snd_midi_status midi[SDD_MIDI_PORT_NUM];
    Uint8 unk_200[0x1000];
    struct sd_snd_slot_status slot[SD_SLOT_NUM];
};
/* the driver's record of handle's MIDI port. the pointer counts bytes from
 * sound memory's start, so the CPU can't read through it. a caller passes a
 * member's address to an sdSndMem call instead */
#define SD_SND_MIDI_STATUS(handle)                                             \
    (&((struct sd_snd_status *)sdgWrkPtr->snd_stat_adr)                        \
          ->midi[((struct sd_port_head *)(handle))->port])
/* the driver's record of slot number, a pointer of the same kind */
#define SD_SND_SLOT_STATUS(number)                                             \
    (&((struct sd_snd_status *)sdgWrkPtr->snd_stat_adr)->slot[number])

/* the sound driver's information, in sound memory SD_DRV_INFO_AT bytes past
 * where the driver lies: its version, the addresses in sound memory of its
 * parts, and stat */
struct sd_drv_info {
    Uint8 ver_major;
    Uint8 ver_minor;
    Uint8 ver_local;
    Uint8 unk_03[0x9d];
    Sint32 unk_a0;
    Sint32 snd_stat_adr;
    Sint32 stat_adr;
    Sint32 data_map_adr;
    Sint32 host_cmd_adr;
    Sint32 unk_b4;
    Uint8 unk_b8[0x20];
    /* SD_DRV_STAT_EXEC once the driver runs, and SD_DRV_STAT_FINISHED once
     * it has finished */
    Uint32 stat;
};
#define SD_DRV_INFO_AT 0x20
#define SD_DRV_STAT_EXEC SDD_CHAR32('E', 'X', 'E', 'C')
#define SD_DRV_STAT_FINISHED 0xffffffff
/* the address in sound memory of member, in the driver's information at
 * address */
#define SD_DRV_INFO_ADR(address, member)                                       \
    ((Sint32)(&((struct sd_drv_info *)(address))->member))
/* where the driver counts up while it runs, past the address the work's
 * stat_adr holds */
#define SD_DRV_COUNTER_AT 0x18
/* the long sdSndMemClear fills sound memory with */
#define SD_SND_MEM_FILL SDD_CHAR32('D', 'M', 'P', 'D')
/* the slots sdSysWrkInit and sdDrvGetInfo give each kind of port */
#define SD_MIDI_SLOT_DEFAULT 48
#define SD_SHOT_SLOT_DEFAULT 8
#define SD_PSTM_SLOT_DEFAULT 8

/* where a bank is mapped in sound memory, and its size */
struct sd_bank_stat {
    Sint32 adr;
    Sint32 size;
};
/* how many banks most kinds have, and how many drum banks
 * sdSndMemAllocateBank maps */
#define SD_BANK_MAX 16
#define SD_DRM_BANK_MAX 2
/* how many banks each of the three kinds of effect data has */
#define SD_FX_BANK_MAX 1
/* every bank's mapping, by kind. the driver keeps one in sound memory, and
 * the library a copy in its work */
struct sd_bank_map {
    struct sd_bank_stat midi_seq[SD_BANK_MAX];
    struct sd_bank_stat midi_prg[SD_BANK_MAX];
    struct sd_bank_stat shot[SD_BANK_MAX];
    struct sd_bank_stat pstm_ring[SD_BANK_MAX];
    struct sd_bank_stat fx_prg;
    Uint8 unk_208[0x78];
    struct sd_bank_stat fx_out;
    struct sd_bank_stat fx_prg_wrk;
    struct sd_bank_stat midi_drm[SD_BANK_MAX];
};

/* the library's blocks. table points to max records, and made counts those
 * made and not destroyed */
struct sd_memblk_table {
    struct sd_memblk *table;
    Sint32 max;
    Sint32 made;
    /* how many transfers have started, and how many have ended */
    Sint32 transfers_started;
    Sint32 transfers_ended;
    SYG2DMA dma; /* the DMA channel that carries blocks */
    /* the block whose transfer is under way, or null */
    SDMEMBLK in_transfer;
};

/* the library's work: what its members share. sdSysWrkInit makes it in
 * sdgGlobalWrk */
struct sd_work {
    union sd_module_refs *modules;
    struct sd_pstm_port_ext *pstm_ext;
    struct sd_host_cmd_buf host_cmd;
    /* the batches sdHostCmdPrimBufFlush has started toward sound memory */
    Sint32 batches_started;
    /* the batches sdHostCmdPrimBufFlushCallback has run for */
    Sint32 batches_ended;
    /* the block that carries a batch to sound memory */
    SDMEMBLK batch_blk;
    /* the slots each kind of port may use. sdSysSetSlotMax sets them, and
     * sdDrvGetInfo sets them to the defaults */
    Sint8 midi_slot_max;
    Sint8 shot_slot_max;
    Sint8 pstm_slot_max;
    Sint8 unk_037;
    Sint8 unk_038;
    /* how many stream slots the open stream ports have taken */
    Sint8 pstm_slot_used;
    /* for each stream slot, the number of the stream port that has taken
     * it, or -1 */
    Sint8 pstm_slot_port[SDD_PSTM_SLOT_NUM];
    Uint8 unk_04a[2];
    struct sd_memblk_table blocks;
    struct sd_memblk_queue blk_queue;
    SDE_MEMBLK_TRANSFER_MODE transfer_mode;
    /* the address in sound memory of the driver's information */
    Sint32 drv_info_adr;
    /* sdDrvGetInfo copies the six longs from unk_84 to unk_98 from struct
     * sd_drv_info's six from unk_a0 to unk_b4. snd_stat_adr, stat_adr,
     * data_map_adr and host_cmd_adr hold addresses in sound memory */
    Sint32 unk_84;
    Sint32 snd_stat_adr; /* the driver's status */
    /* the address of the byte that says a batch waits for the driver.
     * sdHostCmdPrimBufFlushCallback sets the byte to 1, and
     * sdHostCmd2ndBufSend sends nothing while it isn't 0 */
    Sint32 stat_adr;
    Sint32 data_map_adr; /* the driver's bank map */
    Sint32 host_cmd_adr; /* where the driver reads a batch */
    Sint32 unk_98;
    /* the address in sound memory where the driver lies, which sdSysWrkInit
     * sets to 0 */
    Sint32 drv_adr;
    /* the sound driver's version, as sdDrvGetInfo read it */
    Uint8 ver_major;
    Uint8 ver_minor;
    Uint8 ver_local;
    Uint8 ver_end; /* 0, which sdDrvGetInfo stores after the version */
    /* the library's copy of the driver's bank map */
    struct sd_bank_map bank_map;
    Uint8 unk_3b4[0x78];
    /* a block that sdSysWrkInit makes. sdMultiUnitDownload gives it each
     * bank's data in turn, with SDD_MEMBLK_SYNC_FUNC as its callback */
    SDMEMBLK unit_blk;
    /* a block that sdSysWrkInit makes. nothing reads it */
    SDMEMBLK unk_430;
    /* the handler syChainAddHandler made of sdSysVsyncServer for the
     * vertical blank */
    SYCHAIN vsync_chain;
    /* sdSysErrTrap calls it with each failure, unless it is null */
    SD_ERR_CALLBACK_FUNC err_callback;
};
/* the library's work while the library is started: from sdLibInit until
 * sdSysFinish. null otherwise */
extern struct sd_work *sdgWrkPtr;
/* TRUE while the sound driver runs */
extern Bool sdgExistSndDrv;
/* what the G2 library returned to sdMemBlkDmaTransfer, sdMoveWrkMemToSndMem
 * or sdMoveSndMemToWrkMem, whichever called it last. sdMemBlkTransfer sets
 * it to SYE_G2_ERR_NOTHING as each transfer begins */
extern SYE_G2_ERR sdgG2LibResult;
/* the library's name, version and build time */
extern const char *const sdgBuild;

/* the library's own memory. the work lies inside sdgGlobalWrk and the batch
 * inside sdgHostCmdPrimBuf, each at the first address in its array that is
 * a multiple of SD_BUFFER_ALIGN */
#define SD_BUFFER_ALIGN 32
extern union sd_module_refs sdgModuleRef;
extern struct sd_pstm_port_ext sdgPstmPortExtRef[SD_PSTM_PORT_NUM];
extern Uint8 sdgGlobalWrk[sizeof(struct sd_work) + SD_BUFFER_ALIGN];
extern struct sd_memblk sdgMemBlk[SDD_MEMBLK_HANDLE_NUM];
extern struct sd_memblk_queue_entry sdgMemBlkQueue[SDD_MEMBLK_QUEUE_NUM];
extern Uint8 sdgHostCmdPrimBuf[(SD_BATCH_MAX * sizeof(struct sd_driver_cmd)) +
                               SD_BUFFER_ALIGN];
extern struct sd_host_cmd_entry sdgHostCmd2ndBuf[SD_HOST_CMD_QUEUE_NUM];

/* sound memory, which the library reaches over the G2 bus: its size in
 * bytes, and the bus address of its first byte */
#define SD_SND_MEM_SIZE 0x200000
#define SD_SND_MEM_BASE 0xa0800000
/* the bits of a sound memory address that pick a byte within its long */
#define SD_SND_MEM_BYTE_BITS 3
/* a long of sound memory, and its parts. the SH-4 and sound memory are both
 * little-endian, so part n holds the nth byte or half from the long's low
 * end */
union sd_snd_mem_long {
    Uint32 whole;
    Uint16 halves[2];
    Uint8 bytes[4];
};

/* store api in sdgErrApi and err in sdgErrResult: the address of the call
 * that failed, and its failure. then pass both to the library's error
 * callback, unless it is null. the call reads the callback through
 * sdgWrkPtr untested. a report made while sdgWrkPtr is null reads through
 * null */
void sdSysErrTrap(void *api, SDE_ERR err);

/* each call declared below passes a failure to sdSysErrTrap under its own
 * address, unless its comment names another. the sdSndMemRead and
 * sdSndMemWrite calls and sdSndMemAllocateBank pass none. sdMemBlkSetStat
 * and the six setters after it pass none of their own: sdMemBlkCheck passes
 * theirs */

/* make the library's work in sdgGlobalWrk, every byte all ones. then start
 * its parts: the blocks, the queue of blocks, the batch, the queue of
 * commands and the ports. the transfer mode becomes
 * SDE_MEMBLK_TRANSFER_MODE_DMA, and the error callback null. last, the call
 * queues the default slot counts with sdSysSetSlotMax. its sizes are the
 * library's own constants: it reads none of its parameters. it returns
 * SDE_ERR_NOTHING */
SDE_ERR sdSysWrkInit(Sint32 mem_blk_handle_max, Sint32 mem_blk_queue_max,
                     Sint32 second_host_cmd_max);
/* fill sdgModuleRef and sdgPstmPortExtRef with all-ones bytes, and set up
 * the ports' records in them. the work's modules and pstm_ext then point at
 * them. the library's work must be set: the call writes through sdgWrkPtr
 * untested. it returns SDE_ERR_NOTHING.
 *
 * each sd_port takes its kind and number, and 0 in head.unk_01 and opened.
 * each sd_pstm_port_ext takes its kind and number, FALSE in on_mem,
 * on_mem_play_req and on_mem_loop, and 0 in on_mem_transfer_size. every
 * other field keeps all ones. in the work, every stream slot becomes free,
 * and unk_037 and unk_038 become 0.
 *
 * as shipped, the last stream port's sd_port keeps all ones in head.unk_01
 * and opened. opened is compared only with 1, so that port still reads as
 * closed */
SDE_ERR sdModuleRefInit(void);
/* make the num entries at queue the library's queue of blocks, and empty
 * each of them. every interrupt is masked while it runs. it returns
 * SDE_ERR_NO_INIT before the library's work is set */
SDE_ERR sdMemBlkQueueInit(struct sd_memblk_queue_entry *queue, Sint32 num);
/* the sound chip's register that holds the sound processor in reset while
 * its SD_ARM_RESET bit is set */
#define SD_ARM_RESET_REG ((void *)0xa0702c00)
#define SD_ARM_RESET 1
/* hold the sound processor in reset, or let it run. sdArm7ResetKeep sets
 * the SD_ARM_RESET bit, and sdArm7ResetRel clears it. every interrupt is
 * masked while each runs. each returns 0 */
int sdArm7ResetKeep(void);
int sdArm7ResetRel(void);
/* hold the sound processor in reset and fill sound memory with
 * SD_SND_MEM_FILL. it returns 0 */
int sdSndMemClear(void);
/* read the driver's version and the addresses of its parts into the
 * library's work, from the driver's information in sound memory, and set
 * the work's slot counts to the defaults. it returns SDE_ERR_NO_INIT before
 * the library is started */
SDE_ERR sdDrvGetInfo(void);
/* stop the sound driver once no block waits for its transfer. the call
 * stops every sound and clears the effects program. it sends the queued
 * commands, then the command to finish. it waits until the driver's stat is
 * SD_DRV_STAT_FINISHED, and sets sdgExistSndDrv to FALSE. every interrupt
 * is masked while it runs.
 *
 * it returns SDE_ERR_NO_INIT before the library is started, and otherwise
 * sdDrvCheckExecute's or sdHostCmdSet's failure */
SDE_ERR sdDrvFinish(void);
/* set the library's record of the stream port's transfers as a stop leaves
 * it. a port that plays from main memory goes through sdPstmOnMemStop. each
 * of the port's slot_max channels gets a first_transfer of TRUE, and a
 * transfer_adr and transfer_wraps of 0.
 *
 * it returns SDE_ERR_NO_INIT before the library is started and
 * SDE_ERR_HANDLE_NULL for a null handle. as shipped, it passes either
 * failure to sdSysErrTrap under sdPstmStop's address, not its own */
SDE_ERR sdPstmStopWrk(SDPSTM handle);
/* set the target, cur and step levels of channel target_slot of the stream
 * port to 0. a target_slot of SDD_PSTM_PORT_SLOT_ALL sets those of every
 * channel. it returns SDE_ERR_NO_INIT before the library is started and
 * SDE_ERR_HANDLE_NULL for a null handle */
SDE_ERR sdPstmResetPrmWrk(SDPSTM handle, Sint32 target_slot);
/* set the stream port's on_mem to FALSE, and the on_mem_transfer_ofs of
 * each channel it plays to 0. it sends the driver nothing. it returns
 * SDE_ERR_NO_INIT before the library is started and SDE_ERR_HANDLE_NULL
 * for a null handle */
SDE_ERR sdPstmOnMemStop(SDPSTM handle);

/* queue a copy of cmd for the sound driver. every interrupt is masked while
 * it runs. it returns SDE_ERR_NOTHING, or SDE_ERR_NO_INIT before the
 * library is started. it returns SDE_ERR_HOST_CMD_BUF_NO_ENOUGH once the
 * queue holds SD_HOST_CMD_QUEUE_NUM commands, whatever its queue_max */
SDE_ERR sdHostCmdSet(struct sd_host_cmd *cmd);
/* call sdHostCmd2ndBufSend if the library is started, the sound driver
 * runs and the queue holds a command. every interrupt is masked while it
 * runs. it returns SDE_ERR_NOTHING */
SDE_ERR sdHostCmdServer(void);
/* give the queue the num entries at buf, all empty. it returns
 * SDE_ERR_NOTHING, or SDE_ERR_NO_INIT before the library is started */
SDE_ERR sdHostCmd2ndBufInit(struct sd_host_cmd_entry *buf, const Sint32 num);
/* move up to SD_HOST_CMD_SEND_MAX of the queue's commands to the batch,
 * oldest first. then start the batch toward sound memory with
 * sdHostCmdPrimBufFlush, and return its result. every interrupt is masked
 * while it runs.
 *
 * it returns SDE_ERR_NO_INIT before the library is started. where queue_max
 * is 0 it moves nothing and returns SDE_ERR_NOTHING. the call finds the
 * driver busy while a batch waits for it or an earlier send hasn't ended.
 * it then moves nothing and returns SDE_ERR_DRV_BUSY. it returns a failure
 * of sdHostCmdPrimBufSet at once, and starts no batch.
 *
 * as shipped, a failure of sdHostCmdPrimBufSet or sdHostCmdPrimBufFlush
 * leaves sending TRUE. each later send then returns SDE_ERR_DRV_BUSY */
SDE_ERR sdHostCmd2ndBufSend(void);

/* give the batch the SD_BATCH_MAX commands at buf, all empty, and make the
 * block that carries it. every interrupt is masked while it runs. it
 * returns SDE_ERR_NO_INIT before the library is started, and otherwise
 * sdMemBlkCreate's result */
SDE_ERR sdHostCmdPrimBufInit(void *buf);
/* move entry's command to the batch, mark entry empty, and count one
 * command fewer in the queue's queue_num. it returns
 * SDE_ERR_HOST_CMD_BUF_NO_ENOUGH, and moves nothing, for a batch that holds
 * SD_BATCH_MAX commands. the library must be started: the call reads
 * sdgWrkPtr untested */
SDE_ERR sdHostCmdPrimBufSet(struct sd_host_cmd_entry *entry);
/* start the batch toward sound memory, with sdHostCmdPrimBufFlushCallback
 * to follow it. an odd count of commands is raised by one first, which adds
 * a command whose number is 0. an empty batch starts nothing and returns
 * SDE_ERR_NOTHING. otherwise it returns the result of setting the block up
 * or of starting its transfer. the library must be started: the call reads
 * sdgWrkPtr untested */
SDE_ERR sdHostCmdPrimBufFlush(void);

/* the sdSndMemRead and sdSndMemWrite calls each read or write one value of
 * sound memory. address counts bytes from sound memory's start. the bus
 * address is SD_SND_MEM_BASE plus address.
 *
 * an address of SD_SND_MEM_SIZE or more reads or writes nothing, and the
 * call still returns SDE_ERR_NOTHING. a negative address isn't refused: its
 * access reaches the bus below SD_SND_MEM_BASE.
 *
 * a call on a Uint32 or a Sint32 needs an address that is a multiple of 4.
 * for an address that isn't, it does nothing and returns
 * SDE_ERR_SND_MEM_ADR_ERR.
 *
 * the calls on a Uint16, a Uint8 or a Sint8 read the long that holds the
 * value. sdSndMemWriteUint8 then changes its byte in that long and writes
 * the long back */
SDE_ERR sdSndMemReadUint32(Sint32 address, Uint32 *value);
SDE_ERR sdSndMemReadSint32(Sint32 address, Sint32 *value);
SDE_ERR sdSndMemReadUint16(Sint32 address, Uint16 *value);
SDE_ERR sdSndMemReadUint8(Sint32 address, Uint8 *value);
SDE_ERR sdSndMemReadSint8(Sint32 address, Sint8 *value);
SDE_ERR sdSndMemWriteUint32(Sint32 address, Uint32 value);
SDE_ERR sdSndMemWriteSint32(Sint32 address, Sint32 value);
SDE_ERR sdSndMemWriteUint8(Sint32 address, Uint8 value);

/* give the library the num records at blk for its blocks, all free, and
 * open the DMA channel that carries them. every interrupt is masked while
 * it runs. it returns SDE_ERR_NO_INIT before the library is started */
SDE_ERR sdMemBlkInit(struct sd_memblk *blk, Sint32 num);
/* copy the block's bytes to dst of sound memory, and mark it the block in
 * transfer. dst is an address of sound memory held as a pointer.
 * sdMemBlkEndTransferHandller follows the copy. every interrupt is masked
 * while it runs. it returns sdMemBlkCheck's result for handle, and
 * otherwise sdMoveWrkMemToSndMem's. as shipped, a copy that
 * sdMoveWrkMemToSndMem refuses still leaves the block the one in transfer.
 * no end handler follows that copy */
SDE_ERR sdMemBlkCpuTransfer(SDMEMBLK handle, void *dst);
/* start the DMA that carries the block's bytes to dst_adr of sound memory.
 * the block becomes the one in transfer, and sdMemBlkEndTransferHandller
 * follows the DMA's end. every interrupt is masked while it runs.
 *
 * it returns sdMemBlkCheck's result for handle. for a block whose size
 * isn't a multiple of 32 it starts nothing and returns
 * SDE_ERR_PRM_ILLEGAL_VALUE. sdgG2LibResult holds the start's result.
 *
 * syG2DmaTrigger refuses the start while the channel's last transfer is
 * under way. as shipped, the call then still makes the block the one in
 * transfer, and returns SDE_ERR_NOTHING. sdMemBlkEndTransferHandller then
 * runs for this block when the earlier transfer ends, though this block's
 * bytes never moved. it never runs for the earlier block */
SDE_ERR sdMemBlkDmaTransfer(SDMEMBLK handle, Sint32 dst_adr);
/* mark the transfer of the block whose handle is first_arg ended, and call
 * sdMemBlkExecuteCallback for the block. then start the transfer of the
 * block that has waited longest, if one waits. for a block whose transfer
 * has ended already it does nothing. it then neither clears in_transfer nor
 * starts a waiting block. every interrupt is masked while it runs.
 * Handller is the library's spelling */
void sdMemBlkEndTransferHandller(void *first_arg);
/* run the G2 library's handler for a DMA transfer's end by hand. it serves a
 * caller whose interrupt mask holds back the end's own interrupt. the call
 * acts only where it is entered with the mask at IMASK_ALL. it then runs the
 * handler only while the channel is idle and a block is in transfer. where
 * it acts, it reads sdgWrkPtr untested, so the library must be started. it
 * returns SDE_ERR_NOTHING */
SDE_ERR sdMemBlkServer(void);
/* store how blocks travel in *transfer_mode. before the library is started
 * it stores nothing. it returns SDE_ERR_NOTHING either way */
SDE_ERR sdMemBlkGetTransferMode(SDE_MEMBLK_TRANSFER_MODE *transfer_mode);

/* the two copies between main and sound memory go over the G2 bus, a long
 * at a time. snd_mem_adr counts bytes from sound memory's start.
 * each copy that is made stores the G2 library's result in sdgG2LibResult */

/* copy size bytes from wrk_mem_ptr to snd_mem_adr of sound memory. then
 * call func with arg, unless func is null or SDD_MEMBLK_SYNC_FUNC.
 *
 * it returns SDE_ERR_PRM_OVER_RANGE, and copies nothing, for a snd_mem_adr
 * or a size that is negative or above SD_SND_MEM_SIZE. it does the same for
 * a wrk_mem_ptr or a size that isn't a multiple of 4. as shipped, the call
 * never tests the address and the size together. so a copy may run past
 * sound memory's end */
SDE_ERR sdMoveWrkMemToSndMem(void *wrk_mem_ptr, Sint32 snd_mem_adr, Sint32 size,
                             SD_MEMBLK_CALLBACK_FUNC func, void *arg);
/* copy size bytes from snd_mem_adr of sound memory to wrk_mem_ptr. it
 * returns SDE_ERR_UNKNOW_NUM, and copies nothing, for a wrk_mem_ptr or size
 * that isn't a multiple of 4. it tests nothing else */
SDE_ERR sdMoveSndMemToWrkMem(Sint32 snd_mem_adr, void *wrk_mem_ptr,
                             Sint32 size);

/* sdMemBlkSetStat, the three sdMemBlkSetRealUse setters and the three
 * sdMemBlkSetUserSet setters each return sdMemBlkCheck's result for
 * handle, and store nothing unless it is SDE_ERR_NOTHING */

/* set where the block's transfer stands */
SDE_ERR sdMemBlkSetStat(SDMEMBLK handle, SDE_MEMBLK_STAT stat);
/* set the bytes the block's transfer uses, their count, or its callback and
 * the callback's argument */
SDE_ERR sdMemBlkSetRealUseAdr(SDMEMBLK handle, void *ptr);
SDE_ERR sdMemBlkSetRealUseSz(SDMEMBLK handle, Sint32 sz);
SDE_ERR sdMemBlkSetRealUseCallback(SDMEMBLK handle,
                                   SD_MEMBLK_CALLBACK_FUNC callback_func,
                                   void *callback_first_arg);
/* set the bytes, their count, or the callback and its argument in the
 * block's record of what sdMemBlkSetPrm was given */
SDE_ERR sdMemBlkSetUserSetAdr(SDMEMBLK handle, const void *ptr);
SDE_ERR sdMemBlkSetUserSetSz(SDMEMBLK handle, Sint32 sz);
SDE_ERR sdMemBlkSetUserSetCallback(SDMEMBLK handle,
                                   SD_MEMBLK_CALLBACK_FUNC callback_func,
                                   const void *callback_first_arg);

/* sdMemBlkSetRealUsePrm sets all four of what the block's transfer uses.
 * sdMemBlkSetUserSetPrm sets all four of the record of what was given. each
 * then marks the transfer not started. each returns sdMemBlkCheck's result
 * for handle, and SDE_ERR_MEMBLK_ALREADY_TRANSFER for a block whose
 * transfer is under way. a call that fails sets nothing */
SDE_ERR sdMemBlkSetRealUsePrm(SDMEMBLK handle, void *ptr, Sint32 sz,
                              SD_MEMBLK_CALLBACK_FUNC callback_func,
                              void *callback_first_arg);
SDE_ERR sdMemBlkSetUserSetPrm(SDMEMBLK handle, const void *ptr, Sint32 sz,
                              SD_MEMBLK_CALLBACK_FUNC callback_func,
                              const void *callback_first_arg);
/* call the block's callback with its argument, unless the callback is
 * null. where the callback is SDD_MEMBLK_SYNC_FUNC it calls nothing and
 * sets the record's ended to TRUE. it returns sdMemBlkCheck's result */
SDE_ERR sdMemBlkExecuteCallback(SDMEMBLK handle);

/* store the count of blocks that wait for their transfer in *num. it
 * returns SDE_ERR_NO_INIT before the library is started */
SDE_ERR sdMemBlkQueueGetPushNum(Sint32 *num);
/* add the block to those that wait, bound for dst. every interrupt is
 * masked while it runs. it returns sdMemBlkCheck's result, or
 * SDE_ERR_MEMBLK_QUEUE_NO_ENOUGH for a full queue */
SDE_ERR sdMemBlkQueuePush(SDMEMBLK handle, void *dst);
/* take the block that has waited longest: store its handle in *handle and
 * where it is bound in *dst. every interrupt is masked while it runs. it
 * doesn't test for an empty queue: there it takes whatever the next entry
 * holds, and counts below 0. it returns SDE_ERR_NO_INIT before the library
 * is started */
SDE_ERR sdMemBlkQueuePop(SDMEMBLK *handle, void **dst);

/* what the library runs at each vertical blank, if it is started and the
 * sound driver runs: it passes queued commands on, then calls the MIDI, the
 * one-shot, the stream and the block servers. every interrupt is masked
 * while it runs. it doesn't read arg */
void sdSysVsyncServer(void *arg);
/* sdMidiServer, sdShotServer and sdPstmServer each step every open port of
 * their kind. a step adds each level's nonzero step to its cur, and stops
 * cur at the level's target. a step queues a command where it changes cur
 * rounded down to a multiple of SD_LEVEL_UNIT. the command carries
 * cur / SD_LEVEL_UNIT. each server returns SDE_ERR_NO_INIT before the
 * library is started.
 *
 * sdPstmServer calls sdPstmOnMemServer for each port before it steps it.
 * the first of those calls to fail ends the walk, and sdPstmServer returns
 * its failure */
SDE_ERR sdMidiServer(void);
SDE_ERR sdShotServer(void);
SDE_ERR sdPstmServer(void);
/* sdMidiServer's and sdShotServer's step of one level of port, as the
 * paragraph above says. level names the field of cur, step and target to
 * step. set takes port as a handle_type. prev is the caller's scratch, which
 * takes cur.level before the step */
#define SD_STEP_LEVEL(port, prev, level, set, handle_type)                     \
    do {                                                                       \
        if ((port)->step.level != 0) {                                         \
            (prev) = (port)->cur.level;                                        \
            (port)->cur.level += (port)->step.level;                           \
            /* @hack spelling: nested tests of the step's sign and the target, \
             * here and below, where && would do */                            \
            if ((port)->step.level < 0) {                                      \
                if ((port)->cur.level <= (port)->target.level) {               \
                    (port)->step.level = 0;                                    \
                    (port)->cur.level = (port)->target.level;                  \
                }                                                              \
            }                                                                  \
            /* @hack spelling: target <= cur for a step above 0, where cur >=  \
             * target would do */                                              \
            if ((port)->step.level > 0) {                                      \
                if ((port)->target.level <= (port)->cur.level) {               \
                    (port)->step.level = 0;                                    \
                    (port)->cur.level = (port)->target.level;                  \
                }                                                              \
            }                                                                  \
            if (((prev) & ~(SD_LEVEL_UNIT - 1)) !=                             \
                ((port)->cur.level & ~(SD_LEVEL_UNIT - 1))) {                  \
                (set)((handle_type)(port), (port)->cur.level / SD_LEVEL_UNIT); \
            }                                                                  \
        }                                                                      \
    } while (0)
/* feed the stream that the port plays from main memory, if it plays one.
 * every interrupt is masked while it runs.
 *
 * each channel takes the next on_mem_transfer_size bytes of its block,
 * where its ring buffer has room for them. a channel whose block is still
 * in transfer takes nothing this time. sdPstmPlay then starts the stream,
 * unless it has started already.
 *
 * channel 0's block marks the data's end. once its on_mem_transfer_ofs
 * reaches the block's size, the call feeds nothing. where on_mem_loop is
 * TRUE, the call then sets each channel's on_mem_transfer_ofs back to 0.
 * otherwise it stops the stream with sdPstmStop. the call also feeds
 * nothing where sdMemBlkCheck refuses channel 0's block.
 *
 * it returns SDE_ERR_NO_INIT before the library is started, and
 * SDE_ERR_HANDLE_NULL for a null handle. it otherwise returns sdPstmPlay's
 * result where it calls that */
SDE_ERR sdPstmOnMemServer(SDPSTM handle);

/* record that bank bank_num of data_type is mapped at adr of sound memory
 * and has size bytes. the call records it in the driver's bank map and in the
 * library's copy. it returns SDE_ERR_NO_INIT before the library is started,
 * and SDE_ERR_BANK_ILLEGAL_TYPE for a data_type without banks.
 *
 * it returns SDE_ERR_BANK_ILLEGAL_NUM for a negative bank_num, and for one of
 * SD_BANK_MAX or more. it refuses as well a drum bank of SD_DRM_BANK_MAX or
 * more, and an effect bank other than 0 */
SDE_ERR sdSndMemAllocateBank(const SDE_DATA_TYPE data_type,
                             const Sint8 bank_num, Sint32 adr, Sint32 size);

/* open the first port of kind that isn't open, and store its handle in
 * *handle. kind is SD_PORT_KIND_MIDI, SD_PORT_KIND_SHOT or
 * SD_PORT_KIND_PSTM. every field of the port's target, cur and step becomes
 * 0, but unk_0c. the port's unk_68 becomes 0 too.
 *
 * where every port of the kind is open, the call stores null and returns
 * SDE_ERR_HANDLE_NO_ENOUGH. for another kind it stores nothing and returns
 * SDE_ERR_UNKNOW_NUM. the library must be started: the call reads its work
 * untested */
SDE_ERR sdModuleOpenPort(SDHANDLE **handle, Sint32 kind);
/* check port: it returns SDE_ERR_HANDLE_NULL for null, and
 * SDE_ERR_HANDLE_ILLEGAL_VALUE for a pointer of all ones or a port that
 * isn't open */
SDE_ERR sdModuleCheckPort(struct sd_port *port);
/* close port, and return sdModuleCheckPort's result for it */
SDE_ERR sdModuleClosePort(struct sd_port *port);
/* each of the six getters below reads one value of handle's MIDI port from
 * the driver's status. it stores the value less its bias in its second
 * parameter. the bias is SD_PITCH_BIAS for pitch, SD_SPEED_BIAS for speed,
 * and SD_LEVEL_BIAS for the other four. sdMidiGetSpeed doesn't divide by 4,
 * though sdDrvMidiSetSpeed sends the driver 4 times the speed it is given.
 *
 * each getter returns SDE_ERR_NO_INIT before the library is started, and
 * SDE_ERR_HANDLE_NULL for a null handle. each returns SDE_ERR_PTR_NULL for
 * a null second parameter. as shipped, sdMidiGetPitch returns SDE_ERR_NOTHING
 * in each of these cases. only sdSysErrTrap learns of its failure. as shipped
 * too, sdMidiGetDrctLev passes its failure to sdSysErrTrap under
 * sdMidiGetStat's address */
SDE_ERR sdMidiGetVol(SDMIDI handle, Sint8 *vol);
SDE_ERR sdMidiGetPan(SDMIDI handle, Sint8 *pan);
SDE_ERR sdMidiGetFxLev(SDMIDI handle, Sint8 *fx_lev);
SDE_ERR sdMidiGetSpeed(SDMIDI handle, Sint16 *speed);
SDE_ERR sdMidiGetDrctLev(SDMIDI handle, Sint8 *drct_lev);
SDE_ERR sdMidiGetPitch(SDMIDI handle, Sint16 *pitch);
/* store the port's SDD_PORT_ERR_ bits in *err. it returns SDE_ERR_NO_INIT,
 * SDE_ERR_HANDLE_NULL and SDE_ERR_PTR_NULL as the getters above do. it also
 * returns SDE_ERR_HANDLE_ILLEGAL_VALUE for a handle of all ones, and
 * SDE_ERR_PTR_ILLEGAL_VALUE for an err of all ones */
SDE_ERR sdMidiGetErr(SDMIDI handle, Uint32 *err);
/* store the port's SDD_PORT_FLG_ bits in *flg. it reads them from the
 * driver's status. it then sets each of the four SDD_PORT_FLG_CHG_ bits
 * from the library's own record. a bit is set while the port's step for
 * that level isn't 0, and clear otherwise. it returns SDE_ERR_NO_INIT,
 * SDE_ERR_HANDLE_NULL and SDE_ERR_PTR_NULL as the getters above do */
SDE_ERR sdMidiGetFlg(SDMIDI handle, Uint32 *flg);

/* set port's target, cur and step levels all to 0. it returns
 * SDE_ERR_NO_INIT before the library is started, and SDE_ERR_HANDLE_NULL
 * for a null port */
SDE_ERR sdMidiResetPrmWrk(struct sd_port *port);
/* the span of fade_time that one step of a fade covers. sdMidiServer,
 * sdShotServer and sdPstmServer take one step each vertical blank. at 60 Hz
 * a unit of fade_time is then about a millisecond */
#define SD_FADE_STEP_TIME 16
/* set *step to the step that takes cur to target in
 * fade_time / SD_FADE_STEP_TIME steps. fade_time must be SD_FADE_STEP_TIME or
 * more, unchecked: a smaller one divides by 0. it returns SDE_ERR_NOTHING, or
 * SDE_ERR_NO_INIT before the library is started */
SDE_ERR sdUtilCalculationFade(Sint32 cur, Sint32 target, Sint32 fade_time,
                              Sint32 *step);
/* queue the command that gives the sound driver the MIDI port's volume, pan,
 * pitch or speed. the command carries the low byte of vol or pan. it carries
 * the low two bytes of pitch, or of 4 times speed. each returns SDE_ERR_NO_INIT
 * before the library is started, SDE_ERR_HANDLE_NULL for a null handle, and
 * otherwise sdHostCmdSet's result */
SDE_ERR sdDrvMidiSetVol(SDMIDI handle, Sint32 vol);
SDE_ERR sdDrvMidiSetPan(SDMIDI handle, Sint32 pan);
SDE_ERR sdDrvMidiSetPitch(SDMIDI handle, Sint32 pitch);
SDE_ERR sdDrvMidiSetSpeed(SDMIDI handle, Sint16 speed);
/* queue the command that gives the sound driver the one-shot port's volume,
 * its pan or its pitch. each returns SDE_ERR_NO_INIT before the library is
 * started, SDE_ERR_HANDLE_NULL for a null handle, and otherwise
 * sdHostCmdSet's result */
SDE_ERR sdDrvShotSetVol(SDSHOT handle, Sint8 vol);
SDE_ERR sdDrvShotSetPan(SDSHOT handle, Sint8 pan);
SDE_ERR sdDrvShotSetPitch(SDSHOT handle, Sint16 pitch);
/* queue the command that sets the volume, pan or pitch of channel ch of a
 * stream port. a ch of SDD_PSTM_PORT_SLOT_ALL names every channel the port
 * plays.
 *
 * each returns sdHostCmdSet's result, SDE_ERR_NO_INIT before the library is
 * started, and SDE_ERR_HANDLE_NULL for a null handle. a ch that names no
 * channel the port plays queues nothing and returns SDE_ERR_NOTHING. with
 * SDD_PSTM_PORT_SLOT_ALL, sdDrvPstmSetVol and sdDrvPstmSetPan return the
 * last command's result, and sdDrvPstmSetPitch stops at the first that
 * fails.
 *
 * as shipped, each passes a failure to sdSysErrTrap under another call's
 * address. the address is sdPstmSetVol's for the volume, sdPstmSetPan's for
 * the pan and sdPstmSetPitch's for the pitch */
SDE_ERR sdDrvPstmSetVol(SDPSTM handle, Sint8 ch, Sint8 vol);
SDE_ERR sdDrvPstmSetPan(SDPSTM handle, Sint8 ch, Sint8 pan);
SDE_ERR sdDrvPstmSetPitch(SDPSTM handle, Sint8 ch, Sint16 pitch);
/* store in *flg the SDD_PORT_FLG_ bits of channel ch of the stream port.
 * they come from the driver's status of the channel's slot. the four
 * SDD_PORT_FLG_CHG_ bits come from the channel's steps instead, as
 * sdMidiGetFlg's come from the port's.
 *
 * it returns SDE_ERR_NO_INIT before the library is started, and
 * SDE_ERR_HANDLE_NULL for a null handle. it returns SDE_ERR_PTR_NULL for a
 * null flg. it returns SDE_ERR_PRM_OVER_RANGE for a ch at or above the port's
 * slot_num. it does the same for a negative ch, such as
 * SDD_PSTM_PORT_SLOT_ALL */
SDE_ERR sdPstmGetFlg(SDPSTM handle, Sint8 ch, Uint32 *flg);

#endif
