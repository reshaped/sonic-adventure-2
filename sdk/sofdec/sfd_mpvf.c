/* the video stage's frames. the decoder predicts from two references, and
 * decodes pictures into a pool of frames. a pool frame is free, taken by the
 * decoder, or ready for the video output */
#include <sfd.h>
#include <sg_xpt.h>
#include <sofdec.h>

enum frame_state { FRAME_FREE, FRAME_DECODING, FRAME_READY };
static Bool sfmpvf_IsChkFirst(const struct sfd_output_frame *first,
                              const struct sfd_output_frame *frame);

/* lay the two references out in params->references. the first's planes are
 * Cb, Cr and Y. the second's follow as Y, Cb and Cr. cut params->output into
 * params->frames free frames, each a 4:2:0 picture of output_width by
 * output_height */
void SFMPVF_InitRfb(struct sfd_handle *handle)
{
    Sint32 mask;
    struct sfd_transfer *stage;
    struct sfd_video_work *video;
    struct sfd_video_params *params;
    struct mpv_frame *references;
    struct sfd_output_frame *frame;
    Uint8 *base;
    Uint8 *output;
    Sint32 count;
    Sint32 i;
    Sint32 chroma_width;
    Sint32 chroma_height;
    Sint32 luma_width;
    Sint32 luma_height;
    Sint32 output_width;
    Sint32 output_height;
    Sint32 chroma_size;
    Sint32 luma_size;
    Sint32 frame_size;
    Sint32 first_luma;
    Sint32 second_luma;
    Sint32 second_chroma;

    SFLIB_LockCs(&mask);
    stage = &handle->stages[SFD_VIDEO];
    video = &stage->work.video;
    params = &video->params;
    count = params->frames;
    chroma_width = params->chroma_width;
    chroma_height = params->chroma_height;
    luma_width = params->luma_width;
    luma_height = params->luma_height;
    output_width = params->output_width;
    output_height = params->output_height;
    chroma_size = chroma_width * chroma_height;
    luma_size = luma_width * luma_height;
    frame_size = SFD_PICTURE_BYTES(output_width, output_height);
    base = params->references;
    references = &video->references[0];
    references[0].chroma_pitch = chroma_width;
    references[1].chroma_pitch = chroma_width;
    references[0].luma_pitch = luma_width;
    references[1].luma_pitch = luma_width;
    references[0].chroma_b = base;
    references[0].chroma_r = base + chroma_size;
    /* @hack spelling: first_luma, second_luma and second_chroma each assigned
     * inside the store that first uses it, where statements of their own
     * would do */
    references[0].luma = (first_luma = chroma_size + chroma_size) + base;
    references[1].luma = (second_luma = first_luma + luma_size) + base;
    references[1].chroma_b = (second_chroma = second_luma + luma_size) + base;
    references[1].chroma_r = second_chroma + chroma_size + base;
    output = params->output;
    frame = video->frames;
    for (i = 0; i < count; i++, frame++) {
        frame->state = FRAME_FREE;
        frame->pixels = output + (i * frame_size);
        frame->time = 0;
        frame->unit = SFD_NO_UNIT;
        frame->sequence = 0;
    }
    video->first_reference = 0;
    video->second_reference = 1;
    video->ended = FALSE;
    video->at_boundary = FALSE;
    SFLIB_UnlockCs(&mask);
}

void SFMPVF_TermDec(struct sfd_handle *handle)
{
    struct sfd_transfer *stage = &handle->stages[SFD_VIDEO];
    struct sfd_video_work *video = &stage->work.video;
    video->ended = TRUE;
}

Bool SFMPVF_IsTermDec(struct sfd_handle *handle)
{
    struct sfd_transfer *stage = &handle->stages[SFD_VIDEO];
    struct sfd_video_work *video = &stage->work.video;
    return video->ended;
}

void SFMPVF_SetGopStat(struct sfd_handle *handle, Bool at_boundary)
{
    struct sfd_transfer *stage = &handle->stages[SFD_VIDEO];
    struct sfd_video_work *video = &stage->work.video;
    video->at_boundary = at_boundary;
}

Sint32 SFMPVF_GetNumFrm(struct sfd_handle *handle)
{
    Sint32 mask;
    struct sfd_transfer *stage;
    struct sfd_video_work *video;
    struct sfd_video_params *params;
    struct sfd_output_frame *frame;
    Sint32 count;
    Sint32 ready;
    Sint32 unused;
    Sint32 i;

    SFLIB_LockCs(&mask);
    stage = &handle->stages[SFD_VIDEO];
    video = &stage->work.video;
    params = &video->params;
    count = params->frames;
    /* @hack spelling: ready and unused cleared by one chained assignment,
     * where a statement each would do */
    ready = unused = 0;
    frame = video->frames;
    for (i = 0; i < count; i++, frame++) {
        if (frame->state == FRAME_READY) {
            ready++;
        }
        if (frame->state == FRAME_FREE) {
            unused++;
        }
    }
    if (video->ended == TRUE && unused == count) {
        ready = SFMPVF_ENDED;
    }
    SFLIB_UnlockCs(&mask);
    return ready;
}

struct sfd_output_frame *SFMPVF_AllocFrm(struct sfd_handle *handle)
{
    Sint32 mask;
    struct sfd_transfer *stage;
    struct sfd_video_work *video;
    struct sfd_video_params *params;
    struct sfd_output_frame *frame;
    Sint32 count;
    Sint32 i;

    SFLIB_LockCs(&mask);
    stage = &handle->stages[SFD_VIDEO];
    video = &stage->work.video;
    params = &video->params;
    count = params->frames;
    frame = video->frames;
    for (i = 0; i < count; i++, frame++) {
        if (frame->state == FRAME_FREE) {
            frame->state = FRAME_DECODING;
            break;
        }
    }
    if (i == count) {
        frame = NULL;
    }
    SFLIB_UnlockCs(&mask);
    return frame;
}

void SFMPVF_FreeFrm(struct sfd_output_frame *frame)
{
    frame->state = FRAME_FREE;
}

void SFMPVF_StbyFrm(struct sfd_output_frame *frame)
{
    frame->state = FRAME_READY;
}

/* the ready frame with the earliest time, or NULL. while just one is ready,
 * it waits for a second. it doesn't wait once decoding has ended. nor does it
 * wait once a sequence header, group start or end code follows the last
 * picture decoded. *last is TRUE where it hands out the one frame left after
 * decoding has ended */
struct sfd_output_frame *SFMPVF_HoldFrm(struct sfd_handle *handle, Bool *last)
{
    Sint32 mask;
    struct sfd_transfer *stage;
    struct sfd_video_work *video;
    struct sfd_video_params *params;
    struct sfd_output_frame *frame;
    struct sfd_output_frame *held;
    Sint32 count;
    Sint32 ready;
    Sint32 i;

    SFLIB_LockCs(&mask);
    stage = &handle->stages[SFD_VIDEO];
    video = &stage->work.video;
    params = &video->params;
    count = params->frames;
    *last = FALSE;
    frame = video->frames;
    ready = 0;
    held = NULL;
    for (i = 0; i < count; i++, frame++) {
        if (frame->state != FRAME_READY) {
            continue;
        }
        ready++;
        if (sfmpvf_IsChkFirst(held, frame)) {
            held = frame;
        }
    }
    if (ready == 1) {
        if (video->ended != 0) {
            *last = TRUE;
        } else if (!video->at_boundary) {
            held = NULL;
        }
    }
    SFLIB_UnlockCs(&mask);
    return held;
}

static Bool sfmpvf_IsChkFirst(const struct sfd_output_frame *first,
                              const struct sfd_output_frame *frame)
{
    if (first == NULL) {
        return TRUE;
    }
    if (first->time > frame->time) {
        return TRUE;
    }
    return FALSE;
}
