#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

SYE_G2_ERR syG2FifoWaitEmpty(void)
{
    Bool empty;

    /* @hack spelling: the count stepped inside the loop, under a test of
     * empty, where a step after the loop would do */
    do {
        syG2FifoIsEmpty(&empty);
        if (empty == TRUE) {
            sygG2WrkPtr->fifo_waits++;
        }
    } while (empty != TRUE);
    return SYE_G2_ERR_NOTHING;
}
