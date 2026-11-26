#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdUtilCalculationFade(Sint32 cur, Sint32 target, Sint32 fade_time,
                              Sint32 *step)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 diff;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    /* @hack spelling: diff, a local for the difference, where the
     * difference in the division would do */
    diff = target - cur;
    /* @bug arithmetic: divides by 0 for a fade_time from 1 to
     * SD_FADE_STEP_TIME - 1, whose count of steps is 0 (JSR to the division at
     * 0x8c1551f8). its callers are sdMidiSetVol, sdMidiSetPan, sdMidiSetPitch,
     * sdPstmSetVol, sdPstmSetPan and sdPstmSetPitch. each handles a fade_time
     * of 0 itself and refuses a negative one, but passes on any other */
    *step = diff / (fade_time / SD_FADE_STEP_TIME);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdUtilCalculationFade, result);
    }
    return result;
}
