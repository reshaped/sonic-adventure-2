/* the peripheral timer: the clock of a device with Kaede's timer function,
 * set and read by block commands */
#include <kaede.h>
#include <sg_pdtmr.h>
#include <sg_xpt.h>

/* the timers pdgTmrNo numbers */
#define TIMERS 8
/* the bytes of a time as the timer takes and gives it. the time holds
 * PDS_TIME's fields in order, a byte each but the year's two, low byte first.
 * a block command counts the time in 4-byte words */
#define TIME_SIZE 8
#define TIME_WORDS (TIME_SIZE / 4)
/* the block's offset in a block read's response, in bytes from the header */
#define BLOCK_AT 8
/* the bits of a weekday that index a weekday table */
#define WEEKDAY_MASK 7
/* February's month number, and the most days February has */
#define FEBRUARY 2
#define FEBRUARY_DAYS 29
/* pdgTmrNo's entry for a device with no timer */
#define NO_TIMER 255

/* a timer's request: where a read stores the time, and the caller's function
 * for the result, with its argument */
struct timer_request {
    PDS_TIME *time;
    Uint32 param;
    PD_TIMERCALLBACK func;
};

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the peripheral timer library's version and build time */
const char pdgTmrBuild[] = "\npdTmr Ver 1.06 Build:Apr 26 1999 16:55:29\n";
/* each device's timer, or NO_TIMER. only the second and third of each
 * controller port's six devices have one */
const Uint8 pdgTmrNo[KDD_DEV_MAX] = {
    NO_TIMER, 0, 1, NO_TIMER, NO_TIMER, NO_TIMER,
    NO_TIMER, 2, 3, NO_TIMER, NO_TIMER, NO_TIMER,
    NO_TIMER, 4, 5, NO_TIMER, NO_TIMER, NO_TIMER,
    NO_TIMER, 6, 7, NO_TIMER, NO_TIMER, NO_TIMER,
};
/* pdgWeekTblSet takes PDS_TIME's dayofweek to the timer's weekday. it
 * takes dayofweek n to n - 1, and 0 to 6. pdgWeekTblGet takes the timer's
 * weekday back. each table takes 7, which no weekday is, to 0 */
const Uint8 pdgWeekTblSet[WEEKDAY_MASK + 1] = {
    6, 0, 1, 2, 3, 4, 5, 0,
};
const Uint8 pdgWeekTblGet[WEEKDAY_MASK + 1] = {
    1, 2, 3, 4, 5, 6, 0, 0,
};
struct timer_request pdgTmr[TIMERS];
void pdTmrSetTimeCallback(Sint32 device, Sint32 seqno, KDS_DEVICE *dev,
                          KDS_DEVICEINFO *info);
void pdTmrGetTimeCallback(Sint32 device, Sint32 seqno, KDS_DEVICE *dev,
                          KDS_DEVICEINFO *info);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 pdTmrIsReady(Uint32 port)
{
    KDS_TIMERINFO timer;
    Sint32 err;

    /* @hack spelling: the result through err, not tested in place */
    err = kdDevGetTimerInfo(&timer, kdGetDeviceInfo(port));
    if (err < 0) {
        return 0;
    }
    return 1;
}

/* pdTmrSetTime's command callback. it passes the result to the request's
 * func */
void pdTmrSetTimeCallback(Sint32 device, Sint32 seqno, KDS_DEVICE *dev,
                          KDS_DEVICEINFO *info)
{
    struct timer_request *request;
    Sint32 err;

    /* @bug logic: passes PDD_TMRERR_NO_TIMER for a device with the timer
     * function and no LCD function, whatever it answers. the test reads
     * KDD_FUNC_LCD (TST #4,R0 at 0x8c111324), though the command goes by
     * KDD_FUNC_TIMER (MOV #8,R5 at 0x8c11141a) */
    if (!(info->function & KDD_FUNC_LCD)) {
        err = PDD_TMRERR_NO_TIMER;
    } else {
        /* @hack redundant: the cases for 0 and KDD_RES_NORESPONSE, which do
         * what the default does */
        switch (dev->response) {
        case KDD_RES_DEVICEREPLY:
            err = PDD_TMRERR_OK;
            break;
        case KDD_RES_TRANSMITAGAIN:
            err = PDD_TMRERR_INVALID;
            break;
        case 0:
        case KDD_RES_NORESPONSE:
        default:
            err = PDD_TMRERR_NO_TIMER;
            break;
        }
    }
    request = &pdgTmr[pdgTmrNo[device]];
    if (request->func != NULL) {
        request->func(err, request->param);
    }
}

Sint32 pdTmrSetTime(Uint32 port, const PDS_TIME *time, PD_TIMERCALLBACK func,
                    Uint32 param)
{
    struct timer_request *request = &pdgTmr[pdgTmrNo[port]];
    Uint8 data[TIME_SIZE];
    Uint8 *to;
    Sint32 err;

    /* @bug memory: writes the request past pdgTmr, at index NO_TIMER, for a
     * port that pdgTmrNo gives no timer. the stores (0x8c1113a2 to
     * 0x8c1113a8) come before pdTmrIsReady's call at 0x8c1113aa */
    /* @bug logic: replaces the timer's request even when Kaede refuses the
     * command, as the stores come before kdSetBlockWriteEx's call (JSR @R3 at
     * 0x8c111424). an earlier command still queued then reports to this
     * call's func, which has had PDD_TMRERR_BUSY already. the earlier call's
     * func gets no result */
    request->time = (PDS_TIME *)time;
    request->param = param;
    request->func = func;
    if (!pdTmrIsReady(port)) {
        err = PDD_TMRERR_NO_TIMER;
    } else {
        /* @hack spelling: the bytes stored through a cursor, not as data[0]
         * to data[7] */
        to = data;
        *to++ = time->year;
        *to++ = time->year >> 8;
        *to++ = time->month;
        *to++ = time->day;
        *to++ = time->hour;
        *to++ = time->minute;
        *to++ = time->second;
        *to = pdgWeekTblSet[time->dayofweek & WEEKDAY_MASK];
        /* @hack spelling: kdSetBlockWriteEx's result held in err for its
         * test, where the call in the test would do */
        err = kdSetBlockWriteEx(port, KDD_FUNC_TIMER, 0, 0, 0, data, TIME_WORDS,
                                pdTmrSetTimeCallback);
        if (err < 0) {
            err = PDD_TMRERR_BUSY;
        } else {
            err = PDD_TMRERR_OK;
        }
    }
    if (err < 0 && func != NULL) {
        func(err, param);
    }
    return PDD_TMRERR_OK;
}

/* pdTmrGetTime's command callback. it stores the time read through the
 * request's time, and passes the result to the request's func */
void pdTmrGetTimeCallback(Sint32 device, Sint32 seqno, KDS_DEVICE *dev,
                          KDS_DEVICEINFO *info)
{
    struct timer_request *request = &pdgTmr[pdgTmrNo[device]];
    Uint8 *data;
    Sint32 err;

    /* @bug logic: stores no time, and passes PDD_TMRERR_NO_TIMER, for a device
     * with the timer function and no LCD function. the test reads
     * KDD_FUNC_LCD (TST #4,R0 at 0x8c111462), though the command goes by
     * KDD_FUNC_TIMER (MOV #8,R5 at 0x8c111520) */
    /* @hack spelling: each refusal leaves by goto, where one test with ||
     * and the rest in its else would do */
    if (!(info->function & KDD_FUNC_LCD)) {
        err = PDD_TMRERR_NO_TIMER;
        goto done;
    }
    if (dev->response != KDD_RES_DATATRANSFER) {
        err = PDD_TMRERR_NO_TIMER;
        goto done;
    }
    data = (Uint8 *)dev->recvdata + BLOCK_AT;
    request->time->year = (data[1] * 256) + data[0];
    request->time->month = data[2];
    request->time->day = data[3];
    request->time->hour = data[4];
    request->time->minute = data[5];
    request->time->second = data[6];
    /* @hack spelling: the two tests nested, not joined by && */
    if (request->time->month == FEBRUARY) {
        if (request->time->day > FEBRUARY_DAYS) {
            request->time->day = FEBRUARY_DAYS;
        }
    }
    request->time->dayofweek = pdgWeekTblGet[data[7] & WEEKDAY_MASK];
    err = PDD_TMRERR_OK;
done:
    if (request->func != NULL) {
        request->func(err, request->param);
    }
}

Sint32 pdTmrGetTime(Uint32 port, PDS_TIME *time, PD_TIMERCALLBACK func,
                    Uint32 param)
{
    struct timer_request *request = &pdgTmr[pdgTmrNo[port]];
    Sint32 err;

    /* @bug memory: writes the request past pdgTmr, as pdTmrSetTime does, for a
     * port that pdgTmrNo gives no timer. the stores (0x8c111506 to
     * 0x8c11150c) come before pdTmrIsReady's call at 0x8c11150e */
    /* @bug logic: replaces the timer's request even when Kaede refuses the
     * command, as pdTmrSetTime does. an earlier read still queued then stores
     * its time through this call's time, and reports to this call's func. the
     * stores come before kdSetBlockReadEx's call (JSR @R1 at 0x8c11152a) */
    request->time = time;
    request->param = param;
    request->func = func;
    if (!pdTmrIsReady(port)) {
        err = PDD_TMRERR_NO_TIMER;
    } else {
        /* @hack spelling: kdSetBlockReadEx's result held in err for its
         * test, where the call in the test would do */
        err = kdSetBlockReadEx(port, KDD_FUNC_TIMER, 0, 0, 0,
                               pdTmrGetTimeCallback);
        if (err < 0) {
            err = PDD_TMRERR_BUSY;
        } else {
            err = PDD_TMRERR_OK;
        }
    }
    if (err < 0 && func != NULL) {
        func(err, param);
    }
    return PDD_TMRERR_OK;
}
