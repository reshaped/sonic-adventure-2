/* the peripheral timer's alarm */
#include <kaede.h>
#include <sg_pdtmr.h>
#include <sg_xpt.h>

/* an alarm is four bytes. a set-condition command counts its data in
 * 4-byte words */
#define ALARM_WORDS 1

/* data is the SDK's Uint8 *, without const, though the call only reads it */
/* NOLINTNEXTLINE(readability-non-const-parameter) */
Sint32 pdTmrAlarm(Uint32 port, Uint8 *data)
{
    /* @hack type: alarm as a Uint32, filled through a pointer to its bytes,
     * where a Uint8 array of four would do */
    Uint32 alarm;
    /* @hack spelling: the bytes stored through a cursor, not as to[0] to
     * to[3] */
    Uint8 *to;
    /* @hack spelling: the result through err, not tested in place */
    Sint32 err;

    if (!pdTmrIsReady(port)) {
        return PDD_TMRERR_NO_TIMER;
    }
    to = (Uint8 *)&alarm;
    *to++ = data[0];
    *to++ = data[1];
    *to++ = data[2];
    *to = data[3];
    err = kdSetSetCondition(port, KDD_FUNC_TIMER, &alarm, ALARM_WORDS);
    if (err < 0) {
        return PDD_TMRERR_BUSY;
    }
    return PDD_TMRERR_OK;
}
