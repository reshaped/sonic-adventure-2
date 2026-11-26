#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

/* the system bus's normal interrupt status register, through P2. writing 1
 * to a bit clears it. a DMA channel's end is bit INT_DMA_END plus the
 * channel's type */
#define INT_STATUS_NORMAL (*(volatile Uint32 *)0xa05f6900)
#define INT_DMA_END 15

void syG2DmaEndIntrruptHandler(void *arg)
{
    Sint32 mask;
    /* @hack spelling: dma set after the interrupts are masked, where an
     * initializer would do */
    struct sy_g2_dma *dma;
    /* @hack spelling: regs, a local for the channel's registers, where
     * sygG2WrkPtr->regs->dma[dma->type].enable stored in place would do */
    volatile struct sy_g2_dma_regs *regs;
    /* @hack redundant: status, a volatile local that takes each read of
     * INT_STATUS_NORMAL, where the reads alone would do */
    volatile Uint32
        status; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    mask = get_imask();
    set_imask(IMASK_ALL);
    dma = arg;
    regs = &sygG2WrkPtr->regs->dma[dma->type];
    regs->enable = 0;
    dma->running = FALSE;
    dma->transfer_ended = TRUE;
    sygG2WrkPtr->transfers_ended[dma->type]++;
    INT_STATUS_NORMAL = 1 << (dma->type + INT_DMA_END);
    status = INT_STATUS_NORMAL;
    status = INT_STATUS_NORMAL;
    if (dma->callback != NULL) {
        dma->callback(dma->callback_1st_arg);
    }
    set_imask(mask);
}
