/* the motion compensation calls a decoder takes for skipped macroblocks
 * while its MPV_COND_DECODE condition is 0. the member holds four more. each
 * does nothing */
#include <sg_xpt.h>
#include <sofdec.h>

/* the member exports these functions, though no header of the SDK declares
 * them. mpvadec.c's skip_macroblock holds MPVMMC_PpicSkipped and
 * MPVMMC_BpicSkipped. nothing in the image refers to the other four */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void MPVMMC_Intra(struct mpv_obj *mpv)
{
}

void MPVMMC_Forward(struct mpv_obj *mpv)
{
}

void MPVMMC_Backward(struct mpv_obj *mpv)
{
}

void MPVMMC_BiDirect(struct mpv_obj *mpv)
{
}

void MPVMMC_PpicSkipped(struct mpv_obj *mpv, Sint32 nmb)
{
}

void MPVMMC_BpicSkipped(struct mpv_obj *mpv, Sint32 nmb)
{
}
/* NOLINTEND(misc-use-internal-linkage) */
