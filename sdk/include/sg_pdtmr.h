/* Shinobi peripheral timer library, as far as the sources use it */
#ifndef SG_PDTMR_H
#define SG_PDTMR_H
#include <sg_xpt.h>

/* a timer call's results */
#define PDD_TMRERR_OK 0
/* Kaede, the SDK's peripheral driver, refuses the command */
#define PDD_TMRERR_BUSY (-1)
#define PDD_TMRERR_NO_TIMER (-2) /* the port has no timer that answers */
/* the timer answers a set with KDD_RES_TRANSMITAGAIN, which asks for the
 * command again */
#define PDD_TMRERR_INVALID (-3)

/* a date and time. dayofweek is 0 to 6 */
typedef struct {
    Uint16 year;
    Uint8 month;
    Uint8 day;
    Uint8 hour;
    Uint8 minute;
    Uint8 second;
    Uint8 dayofweek;
} PDS_TIME;

/* the function pdTmrSetTime and pdTmrGetTime report their result to, a
 * PDD_TMRERR_ code, with their param */
typedef void (*PD_TIMERCALLBACK)(Sint32 stat, Uint32 param);

/* 1 when the device at port has the timer function, and 0 otherwise. a port
 * is a Kaede device number */
Sint32 pdTmrIsReady(Uint32 port);
/* send data's four bytes to the timer at port as its alarm, in a
 * set-condition command. the call returns PDD_TMRERR_NO_TIMER where
 * pdTmrIsReady returns 0, PDD_TMRERR_BUSY where Kaede refuses the command,
 * and PDD_TMRERR_OK otherwise */
Sint32 pdTmrAlarm(Uint32 port, Uint8 *data);
/* pdTmrSetTime sets the timer at port to *time, and pdTmrGetTime reads it into
 * *time. each returns PDD_TMRERR_OK, and reports its result to func, with
 * param, unless func is null.
 *
 * func gets PDD_TMRERR_NO_TIMER or PDD_TMRERR_BUSY before the call returns,
 * where pdTmrAlarm would return them. otherwise func gets the timer's answer
 * once Kaede receives it. a time set or read gives PDD_TMRERR_OK. a set the
 * timer asks for again gives PDD_TMRERR_INVALID. any other answer gives
 * PDD_TMRERR_NO_TIMER. pdTmrGetTime stores a February day above 29 as 29. time
 * must stay valid until the timer answers.
 *
 * a timer holds one request. as shipped, each call replaces it, even where func
 * gets PDD_TMRERR_NO_TIMER or PDD_TMRERR_BUSY. an earlier call's command, still
 * queued, then reports to the later call's func. an earlier pdTmrGetTime stores
 * its time through the later call's time.
 *
 * port must be a controller's first or second socket, as PDD_PORT_A1 and
 * PDD_PORT_A2 are on connector A. any other port writes past the library's
 * requests. as shipped, the test of a timer's answer reads the device's LCD
 * function, not its timer function. so for a device without an LCD, func gets
 * PDD_TMRERR_NO_TIMER, whatever the timer answers */
Sint32 pdTmrSetTime(Uint32 port, const PDS_TIME *time, PD_TIMERCALLBACK func,
                    Uint32 param);
Sint32 pdTmrGetTime(Uint32 port, PDS_TIME *time, PD_TIMERCALLBACK func,
                    Uint32 param);

#endif
