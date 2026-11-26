/* the audio output stage for audio that plays itself. the audio decoder
 * stage plays its own samples. this stage only follows its input's prepared
 * and ended flags. it passes standby, start, stop and pause on to the audio
 * decoder stage. where the handle has no audio, its server call and its
 * controls do nothing */
#include <sfd.h>
#include <sg_xpt.h>
#include "../sofdec/sfd_stage.h"

/* the error a ring call records: the stage has no ring */
#define NO_RING ((Sint32)0xff000a01)

static Sint32 sfaoap_OutputServer(struct sfd_handle *handle);
static void sfaoap_ChkPrepFlg(struct sfd_handle *handle);
static void sfaoap_ChkTermFlg(struct sfd_handle *handle);

/* clang-format off */
static SFD_STAGE_NOOP(SFAOAP_Init)
static SFD_STAGE_NOOP(SFAOAP_Finish)

static Sint32 SFAOAP_ExecServer(struct sfd_handle *handle, Sint32 first,
                                Sint32 second, Sint32 third)
/* clang-format on */
{
    Sint32 result;
    if (SFSET_GetCond(handle, SFD_COND_AUDIO) == 0) {
        return 0;
    }
    /* @hack spelling: sfaoap_OutputServer's result returned through result,
     * where returning the call would do */
    result = sfaoap_OutputServer(handle);
    return result;
}

static Sint32 sfaoap_OutputServer(struct sfd_handle *handle)
{
    sfaoap_ChkPrepFlg(handle);
    sfaoap_ChkTermFlg(handle);
    return 0;
}

static void sfaoap_ChkPrepFlg(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    Sint32 input;

    if (SFTRN_GetPrepFlg(handle, SFD_AUDIO_OUT) != TRUE) {
        stage = &handle->stages[SFD_AUDIO_OUT];
        input = stage->input;
        if (SFBUF_GetPrepFlg(handle, input) == TRUE) {
            SFTRN_SetPrepFlg(handle, SFD_AUDIO_OUT, TRUE);
        }
    }
}

static void sfaoap_ChkTermFlg(struct sfd_handle *handle)
{
    struct sfd_transfer *stage;
    Sint32 input;

    if (SFTRN_GetTermFlg(handle, SFD_AUDIO_OUT) != TRUE) {
        stage = &handle->stages[SFD_AUDIO_OUT];
        input = stage->input;
        if (SFBUF_GetTermFlg(handle, input) == TRUE) {
            SFTRN_SetTermFlg(handle, SFD_AUDIO_OUT, TRUE);
        }
    }
}

static Sint32 SFAOAP_Create(struct sfd_handle *handle, Sint32 first,
                            Sint32 second, Sint32 third)
{
    /* @hack redundant: a test of SFSET_GetCond's result, where both ways
     * return 0 */
    if (SFSET_GetCond(handle, SFD_COND_AUDIO) == 0) {
        return 0;
    }
    return 0;
}

/* clang-format off */
static SFD_STAGE_NOOP(SFAOAP_Destroy)

/* define name to pass op on to the audio decoder stage with argument. it
 * returns that stage's result, or 0 where the handle sets no such stage */
#define AUDIO_CONTROL(name, op, argument)                                      \
    static Sint32 name(struct sfd_handle *handle, Sint32 first, Sint32 second, \
                       Sint32 third)                                           \
    {                                                                          \
        Sint32 result;                                                         \
        if (SFSET_GetCond(handle, SFD_COND_AUDIO) == 0) {                      \
            return 0;                                                          \
        }                                                                      \
        /* @hack spelling: the assignment inside the test, where returning the \
         * call would do */                                                    \
        if ((result = SFTRN_CallTrtTrif(handle, SFD_AUDIO, op, argument,       \
                                        0)) != 0) {                            \
            return result;                                                     \
        }                                                                      \
        return 0;                                                              \
    }

AUDIO_CONTROL(SFAOAP_Standby, SFD_OP_STANDBY, 0)
AUDIO_CONTROL(SFAOAP_Start, SFD_OP_START, 0)
AUDIO_CONTROL(SFAOAP_Stop, SFD_OP_STOP, 0)
AUDIO_CONTROL(SFAOAP_Pause, SFD_OP_PAUSE, first)
SFD_STAGE_REJECT(SFAOAP_GetWrite, NO_RING)
SFD_STAGE_REJECT(SFAOAP_AddWrite, NO_RING)
SFD_STAGE_REJECT(SFAOAP_GetRead, NO_RING)
SFD_STAGE_REJECT(SFAOAP_AddRead, NO_RING)
static SFD_STAGE_NOOP(SFAOAP_Seek)

const struct sfd_stage_ops SFD_tr_ao_auto_p = {
    {SFAOAP_Init, SFAOAP_Finish, SFAOAP_ExecServer, SFAOAP_Create,
     SFAOAP_Destroy, SFAOAP_Standby, SFAOAP_Start, SFAOAP_Stop, SFAOAP_Pause,
     SFAOAP_GetWrite, SFAOAP_AddWrite, SFAOAP_GetRead, SFAOAP_AddRead,
     SFAOAP_Seek}};
/* clang-format on */
