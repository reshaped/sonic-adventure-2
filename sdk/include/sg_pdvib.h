/* Shinobi vibration library, as far as the sources use it */
#ifndef SG_PDVIB_H
#define SG_PDVIB_H
#include <sg_xpt.h>

/* a vibration call's results */
#define PDD_VIBERR_OK 0
/* Kaede, the SDK's peripheral driver, refuses the command. pdVibEnumerateUnit
 * gives other cases */
#define PDD_VIBERR_BUSY (-1)
#define PDD_VIBERR_NO_VIBRATOR (-2) /* the port has no vibration pack */

/* a pack's orientations, as pdVibGetDirection returns them */
#define PDD_VIB_DIRECTION_NORMAL 0
#define PDD_VIB_DIRECTION_FLIP 1
#define PDD_VIB_DIRECTION_LEFT 2
#define PDD_VIB_DIRECTION_RIGHT 3

/* a vibration's flags, which pdVibStart sends the pack as they are */
#define PDD_VIB_FLAG_CONTINUOUS (1 << 0)
#define PDD_VIB_FLAG_EXHALATION (1 << 3)
#define PDD_VIB_FLAG_CONVERGENCE (1 << 7)

/* a vibration for one unit of a pack */
typedef struct {
    Uint8 unit;
    Uint8 flag;
    Sint8 power;
    Uint8 freq;
    Uint8 inc;
    Uint8 reserved[3];
} PDS_VIBPARAM;

/* a pack's unit counts, which pdVibGetInfo copies from the first two bytes
 * of its vibration function's fd word. units counts the pack's units, and
 * se_units, by the SDK's account, those that can vibrate at once */
typedef struct {
    Uint8 units;
    Uint8 se_units;
} PDS_VIBINFO;

/* a unit as the pack describes it. position and axis are each 0 to 3, and each
 * enable field is 1 or 0. min_freq and max_freq are the unit's lowest and
 * highest frequencies. a description that gives one frequency gives it as both.
 * the library raises a min_freq below 15 to 15, even past max_freq. for a
 * description of another form, the library gives both as 0. for a form it
 * doesn't know, it gives both as 255 */
typedef struct {
    Uint8 position;
    Uint8 axis;
    Uint8 pow_enable;
    Uint8 cont_enable;
    Uint8 dir_enable;
    Uint8 wave_enable;
    Uint8 min_freq;
    Uint8 max_freq;
} PDS_VIBUNITINFO;

/* a function of a result, a PDD_VIBERR_ code, and a param. the library
 * drops what it returns. no call declared here takes one */
typedef Sint32 (*PD_VIBCALLBACK)(Sint32 stat, Uint32 param);
/* what pdVibEnumerateUnit's walk calls for each unit, with the walk's
 * param. where stat is PDD_VIBERR_OK, unit is the unit's number and *info its
 * description. the description lasts only until the callback returns. where
 * stat is PDD_VIBERR_NO_VIBRATOR, no description arrived, and unit and *info
 * hold nothing. the library drops what the callback returns */
typedef Sint32 (*PD_VIBENUMUNITCALLBACK)(Uint32 unit, Sint32 stat,
                                         PDS_VIBUNITINFO *info, Uint32 param);

/* a port in the calls below is a Kaede device number, which must be below
 * KDD_DEV_MAX. the calls don't check it. pdVibSetStopTime, pdVibStart and
 * pdVibGetInfo return PDD_VIBERR_NO_VIBRATOR where pdVibIsReady returns 0. the
 * first two return PDD_VIBERR_BUSY where Kaede refuses their command. each
 * otherwise returns PDD_VIBERR_OK.
 *
 * pdVibIsReady returns 1 when the device at port has the vibration
 * function, and 0 otherwise */
Sint32 pdVibIsReady(Uint32 port);
/* send the stop times of num units in one block write. unit[i] is a unit's
 * number, below 16, and time[i] its stop time. the block is a 2-byte mask of
 * the units, high byte first, then each time's byte. num must be 18 or
 * fewer, for the block's 20 bytes */
Sint32 pdVibSetStopTime(Uint32 port, Uint8 *unit, Uint8 *time, Uint32 num);
/* send the numunits vibrations at param, 16 or fewer, in one set-condition
 * command. a vibration's unit is below 16, and its power is -7 to 7. the
 * call sends a freq below 15 as 15. without PDD_VIB_FLAG_EXHALATION or
 * PDD_VIB_FLAG_CONVERGENCE, it sends inc as 0. such a vibration's power of 1 or
 * -1 goes as 2 or -2 */
Sint32 pdVibStart(Uint32 port, const PDS_VIBPARAM *param, Uint32 numunits);
/* store the pack's counts in *info */
Sint32 pdVibGetInfo(Uint32 port, PDS_VIBINFO *info);

/* the same as pdVibIsReady */
Sint32 pdVibMxIsReady(Uint32 port);
/* pdVibSetStopTime for unit 1 alone, with time's low byte as its stop time */
Sint32 pdVibMxSetStopTime(Uint32 port, Uint32 time);
/* pdVibStart for the one vibration at param */
Sint32 pdVibMxStart(Uint32 port, const PDS_VIBPARAM *param);
/* stop unit 1: pdVibStart for a vibration of unit 1 at power 0 */
Sint32 pdVibMxStop(Uint32 port);
/* walk the pack's units, from unit 1 to the pack's unit count. func gets
 * each unit's description, with param. a description that doesn't arrive
 * goes to func as a failure, and ends the walk. a request for a later unit
 * that Kaede refuses ends the walk with no call to func. func must not be
 * null.
 *
 * the call returns PDD_VIBERR_OK once it has asked for unit 1, and
 * PDD_VIBERR_BUSY where Kaede refuses that request. it also returns
 * PDD_VIBERR_BUSY while an earlier call's first request is unanswered. for a
 * port without a pack it returns PDD_VIBERR_NO_VIBRATOR, and PDD_VIBERR_BUSY
 * from then on.
 *
 * as shipped, a call made after a walk's first answer and before its last takes
 * the walk over. it does so whatever it returns. the walk's later answers go to
 * the new func and param. it counts its units again from 1 */
Sint32 pdVibEnumerateUnit(Uint32 port, PD_VIBENUMUNITCALLBACK func,
                          Uint32 param);
/* which way up the pack at port faces, a PDD_VIB_DIRECTION_ value. it
 * follows from the connector directions of the controller's socket and of
 * the pack */
Sint32 pdVibGetDirection(Uint32 port);

#endif
