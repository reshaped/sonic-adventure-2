#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2Read(void *g2_adr, void *buf_ptr, SYE_G2_DATA_WIDTH data_width,
                    Sint32 data_num, SYE_G2_ADR_MODE src_mode,
                    SYE_G2_ADR_MODE dst_mode)
{
    Sint32 mask;
    Sint32 src_step;
    Sint32 dst_step;
    Uint32 adr;
    Sint32 i;
    Uint8 *dst8;
    Uint8 *src8;
    Uint16 *dst16;
    Uint16 *src16;
    Uint32 *dst32;
    Uint32 *src32;

#ifdef NONMATCHING
    /* GCC can't see that every path sets mask, src_step and dst_step. the two
     * tests of mask_interrupts agree, so mask is set where it is read.
     * sg_syg2.h requires src_mode and dst_mode to be SYE_G2_ADR_MODE values,
     * so each switch sets its step */
    mask = 0;
    src_step = 0;
    dst_step = 0;
#endif
    if (sygG2WrkPtr->mask_interrupts == TRUE) {
        mask = get_imask();
        set_imask(IMASK_ALL);
    }
    switch (src_mode) {
    case SYE_G2_ADR_MODE_FIX:
        src_step = 0;
        break;
    case SYE_G2_ADR_MODE_ADD:
        src_step = 1;
        break;
    case SYE_G2_ADR_MODE_SUB:
        src_step = -1;
        break;
    }
    switch (dst_mode) {
    case SYE_G2_ADR_MODE_FIX:
        dst_step = 0;
        break;
    case SYE_G2_ADR_MODE_ADD:
        dst_step = 1;
        break;
    case SYE_G2_ADR_MODE_SUB:
        dst_step = -1;
        break;
    }
    syG2DmaSuspendAll();
    syG2FifoWaitEmpty();
    adr = (Uint32)g2_adr & SY_G2_PHYSICAL_BITS;
    if (adr >= SY_G2_AICA_FIRST && adr <= SY_G2_AICA_LAST) {
        syG2AicaFifoWaitEmpty();
    }
    switch (data_width) {
    case SYE_G2_DATA_WIDTH_8BIT:
        dst8 = buf_ptr;
        src8 = g2_adr;
        for (i = 0; i < data_num; i++) {
            *dst8 = *src8;
            src8 += src_step;
            dst8 += dst_step;
        }
        break;
    case SYE_G2_DATA_WIDTH_16BIT:
        dst16 = buf_ptr;
        src16 = g2_adr;
        for (i = 0; i < data_num; i++) {
            *dst16 = *src16;
            src16 += src_step;
            dst16 += dst_step;
        }
        break;
    case SYE_G2_DATA_WIDTH_32BIT:
        dst32 = buf_ptr;
        src32 = g2_adr;
        for (i = 0; i < data_num; i++) {
            *dst32 = *src32;
            src32 += src_step;
            dst32 += dst_step;
        }
        break;
    }
    syG2DmaResumeAll();
    if (sygG2WrkPtr->mask_interrupts == TRUE) {
        set_imask(mask);
    }
    return SYE_G2_ERR_NOTHING;
}
