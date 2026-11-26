/* the block decoders a decoder takes while its MPV_COND_DECODE condition is
 * 0. each does nothing, and returns 0 */
#include <sg_xpt.h>
#include <sofdec.h>

/* the member exports these functions, though no header of the SDK declares
 * them. mpvadec.c's decode_intra and decode_nonintra hold MPVMDEC_IntraBlocks
 * and MPVMDEC_NintraBlocks. nothing in the image refers to the other three */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 MPVMDEC_IntraBlocks(struct mpv_obj *mpv)
{
    return 0;
}

Sint32 MPVMDEC_NintraBlocks(struct mpv_obj *mpv)
{
    return 0;
}

Sint32 mpvmdec_AwayIntraYblock(void)
{
    return 0;
}

Sint32 mpvmdec_AwayIntraCblock(void)
{
    return 0;
}

Sint32 mpvmdec_AwayNintraBlock(void)
{
    return 0;
}
/* NOLINTEND(misc-use-internal-linkage) */
