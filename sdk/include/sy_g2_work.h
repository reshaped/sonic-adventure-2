/* the G2 bus library's own records, data and calls, which its members share */
#ifndef SY_G2_WORK_H
#define SY_G2_WORK_H
#include <sg_chain.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

/* every channel's registers, by type */
/* @hack type: the channels' registers in a struct of their own, where a
 * pointer to the first channel's would do for the work's regs */
struct sy_g2_regs {
    struct sy_g2_dma_regs dma[SY_G2_DMA_NUM];
};

/* the bits a record keeps of a transfer. g2_adr and root_bus_adr keep their
 * SY_G2_DMA_ADR_BITS, a physical address that is a multiple of 32. sz keeps
 * its SY_G2_DMA_SZ_BITS, a multiple of 32, and has SY_G2_DMA_SZ_END set.
 * sz & SY_G2_DMA_SZ_BYTES is sz without that bit: the bytes syG2DmaTrigger
 * writes back from the cache. dir keeps its SY_G2_DMA_DIR_BITS. mode keeps its
 * SY_G2_DMA_MODE_BITS, which sg_syg2.h defines beside struct sy_g2_dma_regs */
#define SY_G2_DMA_ADR_BITS 0x1fffffe0
#define SY_G2_DMA_SZ_BITS 0x01ffffe0
#define SY_G2_DMA_SZ_END 0x80000000
#define SY_G2_DMA_SZ_BYTES 0x7fffffe0
#define SY_G2_DMA_DIR_BITS 0x1

/* a DMA channel's record, which its handle points to. g2_adr to mode hold
 * the transfer syG2DmaSetPrm set, as syG2DmaTrigger loads it into the
 * channel's registers. nothing reads unk_18, end_chain, running or
 * transfer_ended */
struct sy_g2_dma {
    SYE_G2_DMA_TYPE type; /* the record's place in the work's dma */
    Uint32 g2_adr;
    Uint32 root_bus_adr;
    Uint32 sz;
    Uint32 dir;
    Uint32 mode;
    Sint32 unk_18; /* 3, the only value the library stores here */
    /* what syG2DmaEndIntrruptHandler calls, with its argument, or null */
    SYE_G2_CALLBACK_FUNC callback;
    void *callback_1st_arg;
    /* the chain number syChainAddHandler gave the channel's end handler */
    SYCHAIN end_chain;
    Bool open; /* TRUE from syG2DmaOpen until syG2DmaClose */
    /* running is TRUE while a transfer the channel started runs.
     * transfer_ended is TRUE once it has ended, until syG2DmaSetPrm sets up
     * another. a reset or an open clears both */
    Bool running;
    Bool transfer_ended;
};

struct sy_g2_work {
    volatile struct sy_g2_regs *regs;
    struct sy_g2_dma dma[SY_G2_DMA_NUM]; /* each channel's record, by type */
    /* where TRUE, syG2Read and syG2Write mask every interrupt while they
     * run. syG2SysInit sets it TRUE */
    Bool mask_interrupts;
    Uint32 unk_d8;
    /* how many transfers syG2DmaTrigger has started on each channel, and
     * how many times syG2DmaEndIntrruptHandler has run for each */
    Uint32 transfers_started[SY_G2_DMA_NUM];
    Uint32 transfers_ended[SY_G2_DMA_NUM];
    Uint32 unk_fc[SY_G2_DMA_NUM];
    /* how many waits syG2FifoWaitEmpty has ended, and how many
     * syG2AicaFifoWaitEmpty has */
    Uint32 fifo_waits;
    Uint32 aica_fifo_waits;
};
/* the work starts at the first multiple of SY_G2_WORK_ALIGN at or after
 * sygG2WrkImg's start */
#define SY_G2_WORK_ALIGN 32

/* the bits of an address that are its physical address */
#define SY_G2_PHYSICAL_BITS 0x1fffffff
/* syG2Read and syG2Write also wait for the sound chip's FIFO at these
 * physical addresses, the first to the last */
#define SY_G2_AICA_FIRST 0x00700000
#define SY_G2_AICA_LAST 0x01000000

/* the system bus's FIFO status register, through P2. a FIFO's bit reads 0
 * while the FIFO is empty. SY_G2_FIFO_AICA is the sound chip's bit, and
 * SY_G2_FIFO_G2 the G2 bus's */
#define SY_G2_FIFO_STATUS (*(volatile Uint32 *)0xa05f688c)
#define SY_G2_FIFO_AICA 0x01
#define SY_G2_FIFO_G2 0x10

/* the work, or null before syG2SysInit */
extern struct sy_g2_work *sygG2WrkPtr;
/* a string that names the library's version and build time */
extern const char *sygG2Build;
/* the bytes syG2SysInit lays the work out in */
extern Uint8 sygG2WrkImg[sizeof(struct sy_g2_work) + SY_G2_WORK_ALIGN];
/* TRUE from syG2DmaSuspendAll until syG2DmaResumeAll */
extern Bool sygG2DmaSuspendAll;

/* reset the work's record of dma's type, and clear that channel's enable
 * register. the record ends as syG2SysInit leaves it: closed, with no
 * transfer set and no callback. every interrupt is masked while it runs. it
 * returns SYE_G2_ERR_NOTHING */
SYE_G2_ERR syG2DmaReset(struct sy_g2_dma *dma);
/* give the bus's DMA protection register the megabytes that hold first and
 * last. the call keeps the register's word in syG2DmaProtect. every interrupt
 * is masked while it runs. it returns SYE_G2_ERR_NOTHING */
SYE_G2_ERR syG2DmaSetProtect(Uint32 first, Uint32 last);

#endif
