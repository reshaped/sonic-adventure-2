/* the motion compensation's setup */
#include <sofdec.h>

/* the copy for each value of a source address's two low bits */
static const MPVMC_COPY mpvmc_copy_tbl[MPVMC_COPIES] = {
    MPVAMC_OneRef1Long,
    MPVAMC_OneRef1Byte,
    MPVAMC_OneRef1Word,
    MPVAMC_OneRef1Byte,
};

void MPVMC_Init(struct mpv_mc *mc)
{
    /* @hack spelling: the four copies written out, where a loop over copy
     * would do */
    mc->copy[0] = mpvmc_copy_tbl[0];
    mc->copy[1] = mpvmc_copy_tbl[1];
    mc->copy[2] = mpvmc_copy_tbl[2];
    mc->copy[3] = mpvmc_copy_tbl[3];
}
