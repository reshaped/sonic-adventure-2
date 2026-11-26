/* the calls that change how a handle plays: pause, standby, speed and the
 * audio and video channels */
#include <sfd.h>
#include <sfd_player.h>
#include <sg_xpt.h>

#define INVALID_PAUSE_QUERY ((Sint32)0xff000141)
#define INVALID_PAUSE ((Sint32)0xff000142)
#define INVALID_STANDBY ((Sint32)0xff000143)
#define INVALID_SPEED ((Sint32)0xff000144)
#define INVALID_AUDIO_CHANNEL ((Sint32)0xff000145)
#define INVALID_VIDEO_CHANNEL ((Sint32)0xff000146)
#define AUDIO_IN_USE ((Sint32)0xff000211)
#define VIDEO_IN_USE ((Sint32)0xff000212)

static Sint32 sfpl2_Pause(struct sfd_handle *handle, Sint32 mode);
static Sint32 sfpl2_TrPause(struct sfd_handle *handle, Sint32 mode);

Sint32 SFD_GetPaStat(struct sfd_handle *handle)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_PAUSE_QUERY);
    }
    return handle->pause;
}

Sint32 SFD_Pause(struct sfd_handle *handle, Sint32 paused)
{
    Sint32 old;
    Sint32 mode;
    Sint32 result;

    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_PAUSE);
    }
    old = handle->pause;
    if (paused == 0) {
        mode = SFD_RESUME;
    } else if (old == 0) {
        mode = SFD_PAUSE;
    } else {
        mode = SFD_STEP;
    }
    handle->pause = paused;
    /* @hack spelling: the call's result held in result, where the return
     * could take it directly */
    result = sfpl2_Pause(handle, mode);
    return result;
}

static Sint32 sfpl2_Pause(struct sfd_handle *handle, Sint32 mode)
{
    Sint32 request;
    Sint32 result;
    request = handle->request;
    /* @hack spelling: an else after the return, where the code could follow
     * the if */
    if ((mode == SFD_STEP && handle->status != SFD_PLAYING) ||
        (request != SFD_STANDBY && request != SFD_PLAYING)) {
        return 0;
    } else {
        SFTIM_Pause(handle, mode);
        /* @hack spelling: the call's result held in result, where the return
         * could take it directly */
        result = sfpl2_TrPause(handle, mode);
        return result;
    }
}

static Sint32 sfpl2_TrPause(struct sfd_handle *handle, Sint32 mode)
{
    Sint32 error;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((error = SFTRN_CallTrtTrif(handle, SFD_AUDIO_OUT, SFD_OP_PAUSE, mode,
                                   0)) != 0) {
        return error;
    }
    return 0;
}

Sint32 SFD_Standby(struct sfd_handle *handle)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_STANDBY);
    }
    return SFPL2_Standby(handle);
}

Sint32 SFPL2_Standby(struct sfd_handle *handle)
{
    handle->request = SFD_STANDBY;
    return 0;
}

Sint32 SFD_SetSpeed(struct sfd_handle *handle, Sint32 speed)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_SPEED);
    }
    SFTIM_SetSpeed(handle, speed);
    SFRNA_SetSpeed(handle, speed);
    return 0;
}

Sint32 SFD_SetAudioCh(struct sfd_handle *handle, Sint32 channel)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_AUDIO_CHANNEL);
    }
    if (handle->status != SFD_STOPPED) {
        SFLIB_SetErr(NULL, AUDIO_IN_USE);
    }
    SFSET_SetCond(handle, SFD_COND_AUDIO_CHANNEL, channel);
    return 0;
}

Sint32 SFD_SetVideoCh(struct sfd_handle *handle, Sint32 channel)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_VIDEO_CHANNEL);
    }
    if (handle->status != SFD_STOPPED) {
        SFLIB_SetErr(NULL, VIDEO_IN_USE);
    }
    SFSET_SetCond(handle, SFD_COND_VIDEO_CHANNEL, channel);
    return 0;
}
