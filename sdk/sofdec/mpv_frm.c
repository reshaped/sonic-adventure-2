/* the decode of one picture, and the skip past one */
#include <sg_xpt.h>
#include <sofdec.h>

/* the start codes that can follow a picture: a picture's, a group's, a
 * sequence's or the sequence end */
#define FRAME_BOUNDARIES                                                       \
    (MPV_DELIM_PICTURE | MPV_DELIM_GROUP | MPV_DELIM_SEQUENCE | MPV_DELIM_END)

Sint32 MPV_DecodeFrm(struct mpv_obj *handle, struct mpv_input *input,
                     Sint32 *consumed, struct mpv_request *request)
{
    /* @hack spelling: decoder, a copy of handle, where handle would do */
    struct mpv_obj *decoder = handle;
    /* UTY_PushFr's save of FR12 to FR15 and XF0 to XF15, which the inverse
     * DCT changes */
    struct mpv_fpu_save saved;
    struct mpv_errinf *errors;
    Uint8 *data;
    Sint32 size;
    Sint32 misses;
    Sint32 others;
    Sint32 result;
    struct mpv_fpu_save *restore;

    if (MPVLIB_CheckHn(handle) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_DECODE_FRM);
    }
    UTY_PushFr(&saved);
    data = input->data;
    errors = &decoder->err;
    size = input->size;
    misses = errors->delimiter_misses;
    others = errors->other_errors;
    decoder->request = *request;
    MPVCMC_InitMcOiRt(decoder);
    MPVCMC_SetCcnt(decoder);
    result = MPVDEC_DecPicture(decoder, data, size, consumed);
    *request->picture = decoder->picture;
    request->delimiter_misses = errors->delimiter_misses - misses;
    request->other_errors = errors->other_errors - others;
    /* @hack spelling: restore, a pointer to saved, where UTY_PopFr(&saved)
     * would do */
    restore = &saved;
    UTY_PopFr(restore);
    return result;
}

Sint32 MPV_SkipFrm(struct mpv_obj *handle, struct mpv_input *input,
                   Sint32 *consumed)
{
    /* @hack spelling: decoder, a copy of handle, where handle would do */
    struct mpv_obj *decoder = handle;
    Uint8 *data;
    Sint32 size;
    Uint8 *delimiter;
    Sint32 error;

    if (MPVLIB_CheckHn(decoder) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_SKIP_FRM);
    }
    data = input->data;
    size = input->size;
    delimiter = MPV_SearchDelim(data, size, FRAME_BOUNDARIES);
    if (delimiter == NULL) {
        *consumed = size;
        error = MPV_ERR_NO_DELIMITER;
    } else {
        *consumed = delimiter - data;
        error = 0;
    }
    return MPVERR_SetCode(decoder, error);
}

void MPVFRM_Init(void)
{
}
