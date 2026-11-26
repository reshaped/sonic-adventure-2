/* concatenated play: files played one after another as one movie. where the
 * handle plays no audio, the video stage skips each file's sequence end code.
 * it adds the file's length to timer->supply_time */
#include <sfd.h>
#include <sg_xpt.h>
#include <sofdec_uty.h>

#define INVALID_CONCAT ((Sint32)0xff000161)
#define INVALID_FILE_TIME ((Sint32)0xff000162)

Sint32 SFD_SetConcatPlay(struct sfd_handle *handle)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_CONCAT);
    }
    SFSET_SetCond(handle, SFD_COND_CONCAT, TRUE);
    return 0;
}

Bool SFCON_IsEndcodeSkip(struct sfd_handle *handle)
{
    if (SFSET_GetCond(handle, SFD_COND_AUDIO) == TRUE) {
        return FALSE;
    }
    if (SFSET_GetCond(handle, SFD_COND_CONCAT) == 0) {
        return FALSE;
    }
    return TRUE;
}

void SFCON_UpdateConcatTime(struct sfd_handle *handle, Sint32 duration)
{
    struct sfd_timer *timer;
    Sint32 count;
    Sint32 mask;
    timer = &handle->timer;
    SFLIB_LockCs(&mask);
    timer->supply_time += duration;
    count = timer->concat_count + 1;
    timer->history[count % SFD_FILE_HISTORY] = timer->supply_time;
    timer->concat_count = count;
    SFLIB_UnlockCs(&mask);
}

/* set *time to SFTIM_GetTimeSub's time less the start of the file it falls
 * in, but not below 0. set *file to that file's number. timer->history holds
 * the starts of the last SFD_FILE_HISTORY files, in timer->start's unit. while
 * the first file plays, timer->supply_time is 0. then, and where the time is
 * of unit SFD_NO_UNIT, *file is 0 and *time SFTIM_GetTimeSub's own */
Sint32 SFD_GetTimePerFile(struct sfd_handle *handle, Sint32 *time, Sint32 *unit,
                          Sint32 *file)
{
    struct sfd_timer *timer;
    struct sfd_time *start;
    Sint32 start_unit;
    Sint32 count;
    Sint32 supplied;
    Sint32 file_start;
    Sint32 i;
    Sint32 mask;
    Sint32 result;

    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_FILE_TIME);
    }
    timer = &handle->timer;
    start = &timer->start;
    start_unit = start->unit;
    *file = 0;
    result = SFTIM_GetTimeSub(handle, time, unit);
    if (timer->supply_time == 0 || *unit == SFD_NO_UNIT) {
        return result;
    }
    SFLIB_LockCs(&mask);
    count = timer->concat_count;
    for (i = 0; i < SFD_FILE_HISTORY; i++, count--) {
        supplied = timer->history[count % SFD_FILE_HISTORY];
        file_start = UTY_MulAbDivC(supplied, *unit, start_unit);
        if (file_start <= *time) {
            break;
        }
    }
    *file = count;
    *time -= file_start;
    if (*time < 0) {
        *time = 0;
    }
    SFLIB_UnlockCs(&mask);
    return result;
}
