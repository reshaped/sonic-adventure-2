#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2AicaFifoIsEmpty(Bool *flg)
{
    /* @hack spelling: the test's result through ? FALSE : TRUE, where == 0
     * would do */
    *flg = (SY_G2_FIFO_STATUS & SY_G2_FIFO_AICA) ? FALSE : TRUE;
    return SYE_G2_ERR_NOTHING;
}
