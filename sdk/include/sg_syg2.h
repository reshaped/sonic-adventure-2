/* the Dreamcast G2 bus library, as far as the sources use it */
#ifndef SG_SYG2_H
#define SG_SYG2_H
#include <sg_xpt.h>

/* a call's result. a call returns SYE_G2_ERR_NOTHING unless its comment
 * names another */
typedef enum SYE_G2_ERR {
    SYE_G2_ERR_NOTHING = 0x00000000,
    SYE_G2_ERR_DMA_ALREADY_OPEN = 0x00090101,
    SYE_G2_ERR_DMA_NOT_OPEN = 0x00090102,
    SYE_G2_ERR_DMA_BUSY = 0x00090180,
    SYE_G2_ERR_UNKNOW = 0x7fffffff
} SYE_G2_ERR;

/* the size in bytes of each item a call moves */
typedef enum SYE_G2_DATA_WIDTH {
    SYE_G2_DATA_WIDTH_8BIT = 1,
    SYE_G2_DATA_WIDTH_16BIT = 2,
    SYE_G2_DATA_WIDTH_32BIT = 4
} SYE_G2_DATA_WIDTH;

/* how a call moves an address from one item to the next: not at all, up or
 * down */
typedef enum SYE_G2_ADR_MODE {
    SYE_G2_ADR_MODE_FIX,
    SYE_G2_ADR_MODE_ADD,
    SYE_G2_ADR_MODE_SUB
} SYE_G2_ADR_MODE;

/* the handle of a DMA channel on the bus */
typedef struct SYS_G2_DMA_HANDLE *SYG2DMA;
/* the bus's DMA channels, by what each serves */
typedef enum SYE_G2_DMA_TYPE {
    SYE_G2_DMA_TYPE_AICA,
    SYE_G2_DMA_TYPE_EXT0,
    SYE_G2_DMA_TYPE_EXT1,
    SYE_G2_DMA_TYPE_DEV
} SYE_G2_DMA_TYPE;
/* how many DMA channels the bus has: one for each SYE_G2_DMA_TYPE */
#define SY_G2_DMA_NUM 4
/* a DMA channel's registers. syG2DmaTrigger loads the first five from the
 * channel's record */
struct sy_g2_dma_regs {
    Uint32 g2_adr;
    Uint32 root_bus_adr;
    Uint32 sz;
    Uint32 dir;
    Uint32 mode;
    Uint32 enable;
    Uint32 start;
    Uint32 suspend;
};
/* the bits of a channel's mode that syG2DmaSetPrm's transfer_ctl and
 * suspend_mode turn on and off. syG2DmaTrigger adds its SYE_G2_DMA_TRIGGER
 * to them. SY_G2_DMA_MODE_BITS are all three */
#define SY_G2_DMA_MODE_TRANSFER_CTL 0x1
#define SY_G2_DMA_MODE_SUSPEND 0x4
#define SY_G2_DMA_MODE_BITS 0x7
/* which way a DMA transfer goes: from main memory to the bus, or back */
typedef enum SYE_G2_DMA_DIR {
    SYE_G2_DMA_DIR_ROOT_TO_G2,
    SYE_G2_DMA_DIR_G2_TO_ROOT
} SYE_G2_DMA_DIR;
/* what starts a DMA transfer: the call that triggers it, or an interrupt */
typedef enum SYE_G2_DMA_TRIGGER {
    SYE_G2_DMA_TRIGGER_CPU = 0,
    SYE_G2_DMA_TRIGGER_INT = 2
} SYE_G2_DMA_TRIGGER;
/* what a DMA transfer calls when it ends, with the argument syG2DmaSetPrm
 * took for it */
typedef void (*SYE_G2_CALLBACK_FUNC)(void *arg);

/* start the library. the call closes and disables every DMA channel. it lets
 * DMA reach main memory's first megabyte through its last. it adds
 * syG2DmaEndIntrruptHandler to each channel's end interrupt chain, again at
 * each call. it ignores prm_ptr. every interrupt is masked while it runs.
 *
 * every call below but syG2AicaFifoIsEmpty and syG2FifoIsEmpty reads the
 * library's work. syG2SysInit lays the work out, so it must come first */
SYE_G2_ERR syG2SysInit(void *prm_ptr);

/* syG2Read copies data_num items of data_width from the bus at g2_adr to
 * buf_ptr. syG2Write copies them from buf_ptr to the bus at g2_adr. after each
 * item, src_mode moves the address read and dst_mode the address written. both
 * modes must be SYE_G2_ADR_MODE values, which neither call checks. a data_width
 * that isn't a SYE_G2_DATA_WIDTH value copies nothing.
 *
 * each call suspends every DMA channel, then waits until the bus's FIFO is
 * empty. syG2Write waits again before its first item and every eighth after it.
 * a wait before an item at a sound chip address waits for the chip's FIFO too.
 * no wait has a time limit. after the copy, each call resumes every channel,
 * even one that was suspended before the call. each masks every interrupt while
 * it runs */
SYE_G2_ERR syG2Read(void *g2_adr, void *buf_ptr, SYE_G2_DATA_WIDTH data_width,
                    Sint32 data_num, SYE_G2_ADR_MODE src_mode,
                    SYE_G2_ADR_MODE dst_mode);
SYE_G2_ERR syG2Write(void *buf_ptr, void *g2_adr, SYE_G2_DATA_WIDTH data_width,
                     Sint32 data_num, SYE_G2_ADR_MODE src_mode,
                     SYE_G2_ADR_MODE dst_mode);

/* the Aica calls test the sound chip's FIFO, and the others the bus's. each
 * IsEmpty call stores in *flg whether its FIFO is empty. each WaitEmpty
 * call waits until it is, with no time limit */
SYE_G2_ERR syG2AicaFifoIsEmpty(Bool *flg);
SYE_G2_ERR syG2FifoIsEmpty(Bool *flg);
SYE_G2_ERR syG2AicaFifoWaitEmpty(void);
SYE_G2_ERR syG2FifoWaitEmpty(void);

/* open the DMA channel of type, and store its handle in *handle. the channel
 * opens with no transfer set and no callback. for a channel that is open
 * already, it stores null and returns SYE_G2_ERR_DMA_ALREADY_OPEN. type must be
 * a SYE_G2_DMA_TYPE value, which the call doesn't check. every interrupt is
 * masked while it runs */
SYE_G2_ERR syG2DmaOpen(SYG2DMA *handle, SYE_G2_DMA_TYPE type);
/* set the channel's next transfer, which syG2DmaTrigger starts. the transfer
 * moves sz bytes between g2_adr on the bus and root_bus_adr in main memory.
 * dir says which way. the call keeps each address as a physical one, cut down
 * to a multiple of 32. it cuts sz down to a multiple of 32 too, modulo
 * 0x2000000. the transfer's end calls callback with callback_1st_arg, unless
 * callback is null.
 *
 * transfer_ctl sets the transfer control bit of the channel's mode, and
 * suspend_mode its suspend bit. TRUE sets a bit, and any other value clears
 * it.
 *
 * the call writes no register: syG2DmaTrigger loads the channel's registers.
 * it doesn't test whether the channel is open. every interrupt is masked while
 * it runs */
SYE_G2_ERR syG2DmaSetPrm(SYG2DMA handle, void *g2_adr, void *root_bus_adr,
                         Sint32 sz, SYE_G2_DMA_DIR dir, Bool transfer_ctl,
                         Bool suspend_mode, SYE_G2_CALLBACK_FUNC callback,
                         void *callback_1st_arg);
/* close the channel. it returns SYE_G2_ERR_DMA_NOT_OPEN for a channel that
 * isn't open. it leaves the channel's registers, and a transfer under way,
 * as they are. every interrupt is masked while it runs */
SYE_G2_ERR syG2DmaClose(SYG2DMA handle);
/* start the transfer syG2DmaSetPrm set. the call first writes the operand
 * cache back over the transfer's main memory. any trigger but
 * SYE_G2_DMA_TRIGGER_CPU counts as SYE_G2_DMA_TRIGGER_INT.
 * while the channel's last transfer is under way, it returns
 * SYE_G2_ERR_DMA_BUSY and changes nothing. every interrupt is masked while
 * it runs */
SYE_G2_ERR syG2DmaTrigger(SYG2DMA handle, SYE_G2_DMA_TRIGGER trigger);
/* syG2DmaSuspend sets the channel's suspend register, and syG2DmaResume
 * clears it. syG2DmaSuspendAll and syG2DmaResumeAll do so for every
 * channel, open or not. of the four, only syG2DmaSuspend masks every
 * interrupt while it runs */
SYE_G2_ERR syG2DmaSuspend(SYG2DMA handle);
SYE_G2_ERR syG2DmaSuspendAll(void);
SYE_G2_ERR syG2DmaResume(SYG2DMA handle);
SYE_G2_ERR syG2DmaResumeAll(void);
/* store in *flg whether the channel's transfer is under way, as its start
 * register says. every interrupt is masked while it runs */
SYE_G2_ERR syG2DmaIsBusy(SYG2DMA handle, Bool *flg);
/* the handler syG2SysInit adds to each channel's end interrupt, with the
 * channel's handle as arg. the sound library calls it too, and Intrrupt is
 * the library's own spelling. it disables the channel, counts the
 * transfer's end and clears the interrupt. it then calls the channel's
 * callback with its argument, unless the callback is null. every interrupt
 * is masked while it runs */
void syG2DmaEndIntrruptHandler(void *arg);

#endif
