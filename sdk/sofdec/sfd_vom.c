/* the manual video output stage. while the handle stands by or plays,
 * SFD_GetFrm hands the application the next decoded picture. it does so at
 * once under SFD_COND_DISPLAY_ALWAYS, and otherwise once the picture is due.
 * SFD_RelFrm takes it back */
#include <sfd.h>
#include <sg_xpt.h>
#include "../sofdec/sfd_stage.h"

#define NO_WRITE ((Sint32)0xff000701)
static void sfvom_ChkTermFlg(struct sfd_handle *handle);
static Bool sfvom_IsTerm(struct sfd_handle *handle);
static void sfvom_ChkPrepFlg(struct sfd_handle *handle);
static Bool sfvom_IsPrepEnd(struct sfd_handle *handle);
static Sint32 sfvom_OutputServer(struct sfd_handle *handle);
static Bool sfvom_IsDispTime(struct sfd_handle *handle,
                             struct sfd_frame_info *frame);

/* clang-format off */
static SFD_STAGE_NOOP(SFVOM_Init)
static SFD_STAGE_NOOP(SFVOM_Finish)

static Sint32 SFVOM_ExecServer(struct sfd_handle *handle, Sint32 first,
                               Sint32 second, Sint32 third)
/* clang-format on */
{
    Sint32 result;

    if (SFSET_GetCond(handle, SFD_COND_VIDEO) == 0) {
        return 0;
    }
    sfvom_ChkTermFlg(handle);
    result = sfvom_OutputServer(handle);
    sfvom_ChkPrepFlg(handle);
    return result;
}

static void sfvom_ChkTermFlg(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    Sint32 input;
    if (SFTRN_GetTermFlg(handle, SFD_VIDEO_OUT) == TRUE) {
        return;
    }
    stage = &handle->stages[SFD_VIDEO_OUT];
    input = stage->input;
    if (SFBUF_GetTermFlg(handle, input) != TRUE) {
        return;
    }
    if (!sfvom_IsTerm(handle)) {
        return;
    }
    SFTRN_SetTermFlg(handle, SFD_VIDEO_OUT, TRUE);
}

static Bool sfvom_IsTerm(struct sfd_handle *handle)
{
    /* @hack spelling: a test of SFTIM_IsVideoTerm that returns FALSE or TRUE,
     * where returning its result would do */
    if (!SFTIM_IsVideoTerm(handle)) {
        return FALSE;
    }
    return TRUE;
}

static void sfvom_ChkPrepFlg(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    Sint32 input;
    if (SFTRN_GetPrepFlg(handle, SFD_VIDEO_OUT) == TRUE) {
        return;
    }
    stage = &handle->stages[SFD_VIDEO_OUT];
    input = stage->input;
    if (SFBUF_GetPrepFlg(handle, input) != TRUE) {
        return;
    }
    if (!sfvom_IsPrepEnd(handle)) {
        return;
    }
    SFTRN_SetPrepFlg(handle, SFD_VIDEO_OUT, TRUE);
}

static Bool sfvom_IsPrepEnd(struct sfd_handle *handle)
{
    return TRUE;
}
static Sint32 sfvom_OutputServer(struct sfd_handle *handle)
{
    return 0;
}

/* clang-format off */
static SFD_STAGE_NOOP(SFVOM_Create)
static SFD_STAGE_NOOP(SFVOM_Destroy)
static SFD_STAGE_NOOP(SFVOM_Standby)
static SFD_STAGE_NOOP(SFVOM_Start)
static SFD_STAGE_NOOP(SFVOM_Stop)
static SFD_STAGE_NOOP(SFVOM_Pause)
SFD_STAGE_REJECT(SFVOM_GetWrite, NO_WRITE)
SFD_STAGE_REJECT(SFVOM_AddWrite, NO_WRITE)

/* @hack redundant: handle declared register, where a plain parameter would
 * do */
static Sint32 SFVOM_GetRead(register struct sfd_handle *handle, Sint32 out,
                            Sint32 second, Sint32 third)
/* clang-format on */
{
    struct sfd_frame_info **frame = (struct sfd_frame_info **)out;
    Sint32 result;
    struct sfd_transfer *stage;
    Sint32 input;

    if (handle->status == SFD_STANDBY || handle->status == SFD_PLAYING) {
        stage = &handle->stages[SFD_VIDEO_OUT];
        input = stage->input;
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((result = SFBUF_VfrmGetRead(handle, input, frame)) != 0) {
            return result;
        } else if (sfvom_IsDispTime(handle, *frame)) {
            return 0;
        }
    }
    *frame = NULL;
    return 0;
}

static Bool sfvom_IsDispTime(struct sfd_handle *handle,
                             struct sfd_frame_info *frame)
{
    const Sint32 *conditions;
    Bool due;
    Sint32 time;
    Sint32 unit;

    if (frame == NULL) {
        return FALSE;
    }
    conditions = handle->conditions;
    if (conditions[SFD_COND_DISPLAY_ALWAYS] != 0) {
        return TRUE;
    }
    time = frame->time;
    unit = frame->time_unit;
    SFTIM_IsExecTime(handle, time, unit, &due,
                     conditions[SFD_COND_DISPLAY_DELAY]);
    return due;
}

static Sint32 SFVOM_AddRead(struct sfd_handle *handle, Sint32 frame,
                            Sint32 second, Sint32 third)
{
    struct sfd_transfer *stage;
    Sint32 input;
    stage = &handle->stages[SFD_VIDEO_OUT];
    input = stage->input;
    return SFBUF_VfrmAddRead(handle, input, (struct sfd_frame_info *)frame);
}

/* clang-format off */
static SFD_STAGE_NOOP(SFVOM_Seek)

const struct sfd_stage_ops SFD_tr_vo_manu = {
    {SFVOM_Init, SFVOM_Finish, SFVOM_ExecServer, SFVOM_Create, SFVOM_Destroy,
     SFVOM_Standby, SFVOM_Start, SFVOM_Stop, SFVOM_Pause, SFVOM_GetWrite,
     SFVOM_AddWrite, SFVOM_GetRead, SFVOM_AddRead, SFVOM_Seek}};
/* clang-format on */
