/* what a decoder read last: a picture's attributes, the sequence header's
 * bit rate and the group's link flags */
#include <sg_xpt.h>
#include <sofdec.h>

Sint32 MPV_GetPicAtr(struct mpv_obj *handle, struct mpv_picture *picture)
{
    /* @hack spelling: decoder, a copy of handle, where handle would do */
    struct mpv_obj *decoder = handle;

    if (MPVLIB_CheckHn(decoder) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_GET_PIC_ATR);
    }
    *picture = decoder->picture;
    return 0;
}

Sint32 MPV_GetBitRate(struct mpv_obj *handle, Sint32 *rate)
{
    /* @hack spelling: decoder, a copy of handle, where handle would do */
    struct mpv_obj *decoder = handle;

    if (MPVLIB_CheckHn(decoder) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_GET_BIT_RATE);
    }
    *rate = decoder->bit_rate;
    return 0;
}

Sint32 MPV_GetLinkFlg(struct mpv_obj *handle, Sint32 *closed_gop,
                      Sint32 *broken_link)
{
    /* @hack spelling: decoder, a copy of handle, where handle would do */
    struct mpv_obj *decoder = handle;

    if (MPVLIB_CheckHn(decoder) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_GET_LINK_FLG);
    }
    *closed_gop = decoder->closed_gop;
    *broken_link = decoder->broken_link;
    return 0;
}
