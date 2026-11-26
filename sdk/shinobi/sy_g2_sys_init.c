#include <machine.h>
#include <sg_chain.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <string.h>
#include <sy_g2_work.h>

/* the bus's DMA registers, through P2 */
#define REGS ((volatile struct sy_g2_regs *)0xa05f7800)
#define MAIN_MEM_FIRST 0x0c000000
#define MAIN_MEM_LAST 0x0cffffff
/* the priority syG2DmaEndIntrruptHandler has on each channel's chain */
#define END_PRIORITY 16

SYE_G2_ERR syG2SysInit(void *prm_ptr)
{
    Sint32 mask;
    Sint32 i;

    mask = get_imask();
    set_imask(IMASK_ALL);
    /* @hack redundant: a call of strlen whose result nothing takes */
    strlen(sygG2Build);
    sygG2WrkPtr =
        (struct sy_g2_work *)(((Uint32)sygG2WrkImg + (SY_G2_WORK_ALIGN - 1)) &
                              ~(SY_G2_WORK_ALIGN - 1));
    memset(sygG2WrkPtr, 0xff, sizeof(struct sy_g2_work));
    sygG2WrkPtr->regs = REGS;
    sygG2WrkPtr->mask_interrupts = TRUE;
    sygG2WrkPtr->unk_d8 = 0;
    for (i = 0; i < SY_G2_DMA_NUM; i++) {
        sygG2WrkPtr->dma[i].type = i;
        /* @hack redundant: the eleven stores after the type's, which
         * syG2DmaReset makes again */
        sygG2WrkPtr->dma[i].open = FALSE;
        sygG2WrkPtr->dma[i].running = FALSE;
        sygG2WrkPtr->dma[i].transfer_ended = FALSE;
        sygG2WrkPtr->dma[i].g2_adr = 0;
        sygG2WrkPtr->dma[i].root_bus_adr = 0;
        sygG2WrkPtr->dma[i].sz = 0;
        sygG2WrkPtr->dma[i].dir = 0;
        sygG2WrkPtr->dma[i].mode = 0;
        sygG2WrkPtr->dma[i].unk_18 = 3;
        sygG2WrkPtr->dma[i].callback = NULL;
        sygG2WrkPtr->dma[i].callback_1st_arg = NULL;
        syG2DmaReset(&sygG2WrkPtr->dma[i]);
        sygG2WrkPtr->transfers_started[i] = 0;
        sygG2WrkPtr->unk_fc[i] = 0;
        sygG2WrkPtr->transfers_ended[i] = 0;
    }
    sygG2WrkPtr->fifo_waits = 0;
    sygG2WrkPtr->aica_fifo_waits = 0;
    syG2DmaSetProtect(MAIN_MEM_FIRST, MAIN_MEM_LAST);
    /* @hack redundant: a second store of TRUE to mask_interrupts, which
     * nothing has changed since the first */
    sygG2WrkPtr->mask_interrupts = TRUE;
    sygG2WrkPtr->dma[SYE_G2_DMA_TYPE_AICA].end_chain = syChainAddHandler(
        SYD_CHAIN_EVENT_IML4_AICADM, syG2DmaEndIntrruptHandler, END_PRIORITY,
        &sygG2WrkPtr->dma[SYE_G2_DMA_TYPE_AICA]);
    sygG2WrkPtr->dma[SYE_G2_DMA_TYPE_EXT0].end_chain = syChainAddHandler(
        SYD_CHAIN_EVENT_IML4_EX0DMA, syG2DmaEndIntrruptHandler, END_PRIORITY,
        &sygG2WrkPtr->dma[SYE_G2_DMA_TYPE_EXT0]);
    sygG2WrkPtr->dma[SYE_G2_DMA_TYPE_EXT1].end_chain = syChainAddHandler(
        SYD_CHAIN_EVENT_IML4_EX1DMA, syG2DmaEndIntrruptHandler, END_PRIORITY,
        &sygG2WrkPtr->dma[SYE_G2_DMA_TYPE_EXT1]);
    sygG2WrkPtr->dma[SYE_G2_DMA_TYPE_DEV].end_chain = syChainAddHandler(
        SYD_CHAIN_EVENT_IML4_DADMAE, syG2DmaEndIntrruptHandler, END_PRIORITY,
        &sygG2WrkPtr->dma[SYE_G2_DMA_TYPE_DEV]);
    sygG2DmaSuspendAll = FALSE;
    /* @hack redundant: a label nothing jumps to */
exit: /* NOLINT(clang-diagnostic-unused-label) */
    set_imask(mask);
    return SYE_G2_ERR_NOTHING;
}
