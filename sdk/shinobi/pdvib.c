/* the vibration pack, through Kaede's vibration function: its units'
 * vibrations, stop times and descriptions */
#include <kaede.h>
#include <sg_pdvib.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* a vibration's first byte, and a description's, hold the unit's number in
 * the four bits from UNIT_SHIFT up */
#define UNIT_SHIFT 4
#define UNIT_MASK 15

/* a block of stop times opens with a mask of STOP_MASK_SIZE bytes, high
 * byte first. the mask sets bit n for unit n. a byte of time for each unit
 * follows. a block write counts the block in words of WORD_SIZE bytes, and
 * pdVibSetStopTime's block holds STOP_BLOCK_WORDS of them */
#define STOP_MASK_SIZE 2
#define WORD_SIZE 4
#define STOP_BLOCK_WORDS 5

/* the flags under which pdVibStart sends PDS_VIBPARAM's inc */
#define FLAG_INC (PDD_VIB_FLAG_EXHALATION | PDD_VIB_FLAG_CONVERGENCE)
/* a vibration takes VIBRATION_SIZE bytes of the set-condition command. they
 * hold the unit, the power, the frequency and inc, in that order. the unit
 * shares its byte with PDD_VIB_FLAG_CONTINUOUS, and the power with
 * FLAG_INC's flags. pdVibStart's command holds VIBRATIONS_MAX vibrations */
#define VIBRATION_SIZE 4
#define VIBRATIONS_MAX 16
/* a positive power goes in the bits of POWER_POSITIVE, and a negative
 * power's magnitude in those of POWER_NEGATIVE */
#define POWER_POSITIVE 0x70
#define POWER_POSITIVE_SHIFT 4
#define POWER_NEGATIVE 0x07
/* the lowest frequency pdVibStart sends, and the lowest min_freq
 * pdVibSetUnitInfo gives where it takes min_freq from a description */
#define FREQ_MIN 15

/* a pack's response to a media information command. the description of the
 * unit the command named follows the header and the function word */
struct vib_media_info {
    Uint32 header;
    Uint32 function;
    Uint8 description[4];
};
/* a description's first byte holds the unit's number, its position and its
 * axis */
#define POSITION_SHIFT 2
#define POSITION_MASK 3
#define AXIS_MASK 3
/* a description's second byte holds the four _ENABLE bits. each sets the
 * PDS_VIBUNITINFO field of its name in lowercase, as POW_ENABLE sets
 * pow_enable. the byte's FREQ_FORM bits give the form of the last two
 * bytes */
#define POW_ENABLE 0x80
#define CONT_ENABLE 0x40
#define DIR_ENABLE 0x20
#define WAVE_ENABLE 0x10
#define FREQ_FORM 0x0f
/* the last two bytes are the lowest and highest frequencies under
 * FREQ_FORM_RANGE. under FREQ_FORM_ONE the first is both. pdVibSetUnitInfo
 * gives both as 0 under FREQ_FORM_ZERO, and as FREQ_UNKNOWN under any other
 * form */
#define FREQ_FORM_RANGE 0
#define FREQ_FORM_ONE 1
#define FREQ_FORM_ZERO 15
#define FREQ_UNKNOWN 0xff

/* a port's state: the walk pdVibEnumerateUnit starts over its pack's units,
 * and where pdVibGetUnitInfoCallback sends a description */
struct vib_port {
    /* TRUE from pdVibEnumerateUnit until its first request is answered or
     * refused. a call that finds no pack leaves it TRUE */
    Bool busy;
    /* where pdVibGetUnitInfoCallback stores a description */
    PDS_VIBUNITINFO *unit_info;
    Uint32 param; /* the caller's argument for info_func and unit_func */
    /* what pdVibGetUnitInfoCallback calls. pdVibEnumerateUnit sets it to
     * null, and nothing else in this source sets it */
    PD_VIBCALLBACK info_func;
    PD_VIBENUMUNITCALLBACK unit_func; /* the func pdVibEnumerateUnit took */
    Uint32 unit; /* the unit last asked for, from 1 */
    /* the pack's unit counts, as pdVibEnumerateUnit read them */
    PDS_VIBINFO info;
};

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 pdVibSetUnitInfo(Sint32 device, Sint32 seqno, const KDS_DEVICE *dev,
                        const KDS_DEVICEINFO *info, PDS_VIBUNITINFO *unit_info,
                        Uint32 *unit);
void pdVibGetUnitInfoCallback(Sint32 device, Sint32 seqno, KDS_DEVICE *dev,
                              KDS_DEVICEINFO *info);
void pdVibEnumUnitCallback(Sint32 device, Sint32 seqno, KDS_DEVICE *dev,
                           KDS_DEVICEINFO *info);

/* the library's version and build time */
const char pdgVibBuild[] = "\npdVib Ver 2.01 Build:Feb 15 2000 12:20:25\n";
struct vib_port pdgVib[KDD_DEV_MAX];
/* NOLINTEND(misc-use-internal-linkage) */

/* each port's controller port: the first of its connector's six ports */
static const Uint8 vib_root_ports[KDD_DEV_MAX] = {
    PDD_PORT_A0, PDD_PORT_A0, PDD_PORT_A0, PDD_PORT_A0, PDD_PORT_A0,
    PDD_PORT_A0, PDD_PORT_B0, PDD_PORT_B0, PDD_PORT_B0, PDD_PORT_B0,
    PDD_PORT_B0, PDD_PORT_B0, PDD_PORT_C0, PDD_PORT_C0, PDD_PORT_C0,
    PDD_PORT_C0, PDD_PORT_C0, PDD_PORT_C0, PDD_PORT_D0, PDD_PORT_D0,
    PDD_PORT_D0, PDD_PORT_D0, PDD_PORT_D0, PDD_PORT_D0,
};
/* pdVibGetDirection's results, by the controller's connector_dir for the
 * socket, then by the pack's own connector_dir[0] */
/* @hack spelling: the table flat, indexed by KD_CONNECTOR_DIRS times the
 * socket's direction plus the pack's, where a table of KD_CONNECTOR_DIRS rows
 * would do */
static const Uint8 vib_directions[KD_CONNECTOR_DIRS * KD_CONNECTOR_DIRS] = {
    PDD_VIB_DIRECTION_FLIP,  PDD_VIB_DIRECTION_NORMAL, PDD_VIB_DIRECTION_LEFT,
    PDD_VIB_DIRECTION_RIGHT, PDD_VIB_DIRECTION_NORMAL, PDD_VIB_DIRECTION_FLIP,
    PDD_VIB_DIRECTION_RIGHT, PDD_VIB_DIRECTION_LEFT,   PDD_VIB_DIRECTION_RIGHT,
    PDD_VIB_DIRECTION_LEFT,  PDD_VIB_DIRECTION_FLIP,   PDD_VIB_DIRECTION_NORMAL,
    PDD_VIB_DIRECTION_LEFT,  PDD_VIB_DIRECTION_RIGHT,  PDD_VIB_DIRECTION_NORMAL,
    PDD_VIB_DIRECTION_FLIP,
};

Sint32 pdVibIsReady(Uint32 port)
{
    /* @hack spelling: the device's description through info, not read from
     * kdGetDeviceInfo's result in place */
    const KDS_DEVICEINFO *info;

    info = kdGetDeviceInfo(port);
    return (info->function & KDD_FUNC_VIBRATION) ? 1 : 0;
}

Sint32 pdVibSetStopTime(Uint32 port, Uint8 *unit, Uint8 *time, Uint32 num)
{
    Uint32 block[STOP_BLOCK_WORDS];
    Uint8 *to;
    Uint16 units;
    Uint32 i;
    /* @hack spelling: words, a local set in two statements, where
     * (num + STOP_MASK_SIZE + WORD_SIZE - 1) / WORD_SIZE as kdSetBlockWrite's
     * argument would do */
    Uint32 words;
    /* @hack spelling: the result through err, not tested in place */
    Sint32 err;

    if (!pdVibIsReady(port)) {
        return PDD_VIBERR_NO_VIBRATOR;
    }
    /* @hack spelling: block's words cleared in one chained assignment, where
     * a statement for each would do */
    block[0] = block[1] = block[2] = block[3] = block[4] = 0;
    to = (Uint8 *)block;
    units = 0;
    /* @hack spelling: unit stepped as a cursor, where unit[i] would do */
    for (i = 0; i < num; i++) {
        units |= 1 << *unit++;
    }
    *to++ = units >> 8;
    *to++ = units;
    /* @hack spelling: time stepped as a cursor, where time[i] would do */
    for (i = 0; i < num; i++) {
        *to++ = *time++;
    }
    words = num + STOP_MASK_SIZE;
    words = (words + WORD_SIZE - 1) / WORD_SIZE;
    err = kdSetBlockWrite(port, KDD_FUNC_VIBRATION, 0, 0, 0, block, words);
    if (err < 0) {
        return PDD_VIBERR_BUSY;
    }
    return PDD_VIBERR_OK;
}

Sint32 pdVibStart(Uint32 port, const PDS_VIBPARAM *param, Uint32 numunits)
{
    Uint8 condition[VIBRATIONS_MAX * VIBRATION_SIZE];
    Uint8 *to;
    Uint32 i;
    Sint8 power;
    Uint8 level;
    /* @hack spelling: flag, a local for param->flag, where param->flag at
     * each use would do */
    Uint8 flag;
    /* @hack spelling: the result through err, not tested in place */
    Sint32 err;

    if (!pdVibIsReady(port)) {
        return PDD_VIBERR_NO_VIBRATOR;
    }
    to = condition;
    /* @hack spelling: param stepped as a cursor, where param[i] would do */
    for (i = 0; i < numunits; i++, param++) {
        flag = param->flag;
        *to++ = (param->unit << UNIT_SHIFT) | (flag & PDD_VIB_FLAG_CONTINUOUS);
        power = param->power;
        if (!(flag & FLAG_INC)) {
            if (power == 1) {
                power = 2;
            }
            if (power == -1) {
                power = -2;
            }
        }
        if (power > 0) {
            level = (power << POWER_POSITIVE_SHIFT) & POWER_POSITIVE;
        } else if (power < 0) {
            level = -power & POWER_NEGATIVE;
        } else {
            level = 0;
        }
        level |= flag & FLAG_INC;
        *to++ = level;
        /* @hack spelling: the frequency stored in each arm of an if, where
         * one store of a conditional expression would do */
        if (param->freq < FREQ_MIN) {
            *to++ = FREQ_MIN;
        } else {
            *to++ = param->freq;
        }
        *to++ = (flag & FLAG_INC) ? param->inc : 0;
    }
    err = kdSetSetCondition(port, KDD_FUNC_VIBRATION, condition, numunits);
    if (err < 0) {
        return PDD_VIBERR_BUSY;
    }
    return PDD_VIBERR_OK;
}

Sint32 pdVibGetInfo(Uint32 port, PDS_VIBINFO *info)
{
    KDS_VIBRATIONINFO vib;
    /* @hack spelling: the result through err, not tested in place */
    Sint32 err;

    err = kdDevGetVibrationInfo(&vib, kdGetDeviceInfo(port));
    if (err < 0) {
        return PDD_VIBERR_NO_VIBRATOR;
    }
    info->units = vib.vn;
    info->se_units = vib.se;
    return PDD_VIBERR_OK;
}

/* store in *unit_info the description in dev's response to a media
 * information command. it stores the unit's number in *unit, unless unit is
 * null, and returns PDD_VIBERR_OK. where info lacks the vibration function, or
 * the response isn't KDD_RES_DATATRANSFER, it stores nothing and returns
 * PDD_VIBERR_NO_VIBRATOR */
Sint32 pdVibSetUnitInfo(Sint32 device, Sint32 seqno, const KDS_DEVICE *dev,
                        const KDS_DEVICEINFO *info, PDS_VIBUNITINFO *unit_info,
                        Uint32 *unit)
{
    const Uint8 *data;
    /* @hack spelling: bits, a local for data[1], where data[1] at each use
     * would do */
    Uint8 bits;
    /* @hack spelling: both results through err to one return, where a
     * return on each path would do */
    Sint32 err;

    if (!(info->function & KDD_FUNC_VIBRATION) ||
        dev->response != KDD_RES_DATATRANSFER) {
        err = PDD_VIBERR_NO_VIBRATOR;
    } else {
        data = ((const struct vib_media_info *)dev->recvdata)->description;
        if (unit != NULL) {
            /* @hack redundant: UNIT_MASK, which leaves a byte shifted right
             * by UNIT_SHIFT as it is */
            *unit = (data[0] >> UNIT_SHIFT) & UNIT_MASK;
        }
        bits = data[1];
        unit_info->position = (data[0] >> POSITION_SHIFT) & POSITION_MASK;
        unit_info->axis = data[0] & AXIS_MASK;
        unit_info->pow_enable = (bits & POW_ENABLE) ? 1 : 0;
        unit_info->cont_enable = (bits & CONT_ENABLE) ? 1 : 0;
        unit_info->dir_enable = (bits & DIR_ENABLE) ? 1 : 0;
        unit_info->wave_enable = (bits & WAVE_ENABLE) ? 1 : 0;
        switch (bits & FREQ_FORM) {
        case FREQ_FORM_RANGE:
            /* @hack spelling: min_freq stored in each arm of an if, where
             * one store of a conditional expression would do */
            if (data[2] < FREQ_MIN) {
                unit_info->min_freq = FREQ_MIN;
            } else {
                unit_info->min_freq = data[2];
            }
            unit_info->max_freq = data[3];
            break;
        case FREQ_FORM_ONE:
            /* @hack spelling: min_freq stored in each arm of an if, where
             * one store of a conditional expression would do */
            if (data[2] < FREQ_MIN) {
                unit_info->min_freq = FREQ_MIN;
            } else {
                unit_info->min_freq = data[2];
            }
            unit_info->max_freq = data[2];
            break;
        case FREQ_FORM_ZERO:
            unit_info->min_freq = 0;
            unit_info->max_freq = 0;
            break;
        default:
            unit_info->min_freq = FREQ_UNKNOWN;
            unit_info->max_freq = FREQ_UNKNOWN;
            break;
        }
        err = PDD_VIBERR_OK;
    }
    return err;
}

/* a Kaede callback for a media information command. while the port's
 * info_func is set, it stores the description through unit_info, and
 * passes info_func the result */
void pdVibGetUnitInfoCallback(Sint32 device, Sint32 seqno, KDS_DEVICE *dev,
                              KDS_DEVICEINFO *info)
{
    struct vib_port *vib = &pdgVib[device];
    /* @hack spelling: the result through err, where the call as info_func's
     * argument would do */
    Sint32 err;

    if (vib->info_func != NULL) {
        err = pdVibSetUnitInfo(device, seqno, dev, info, vib->unit_info, NULL);
        vib->info_func(err, vib->param);
    }
}

/* the walk's callback for each media information command. it passes
 * unit_func the description, or the failure to get one. after a
 * description it asks for the next unit's, up to the pack's unit count */
void pdVibEnumUnitCallback(Sint32 device, Sint32 seqno, KDS_DEVICE *dev,
                           KDS_DEVICEINFO *info)
{
    struct vib_port *vib = &pdgVib[device];
    Uint32 unit;
    PDS_VIBUNITINFO unit_info;
    Sint32 err;

    /* @bug logic: clears busy at a walk's first answer, though the walk goes
     * on (MOV.L R2,@R14 at 0x8c137f76, JSR @R3 at 0x8c137fbc). a
     * pdVibEnumerateUnit call made before the walk ends then passes the busy
     * test. it replaces the walk's unit_func, param and unit (0x8c137fee to
     * 0x8c137ff6) before it asks for anything. the walk's later answers go
     * to the new unit_func, even when that call fails */
    vib->busy = FALSE;
#ifdef NONMATCHING
    /* this side keeps the result of the bug tagged below: kdRecvFrame, the one
     * caller, leaves 0 in R12, which the image's test reads as err without
     * unit_func. so err starts at 0, and such a walk goes on, as in the
     * image */
    err = PDD_VIBERR_OK;
#endif
    /* @bug uninitialized: tests err, which nothing has set, when unit_func
     * is null. BT at 0x8c137f7c skips the call, whose result MOV R0,R12 at
     * 0x8c137f98 keeps in R12. TST R12,R12 at 0x8c137f9e then tests R12 as
     * the caller left it, and the walk goes on only if that is 0 */
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    if (vib->unit_func != NULL) {
        err = pdVibSetUnitInfo(device, seqno, dev, info, &unit_info, &unit);
        /* @bug uninitialized: passes unit_func a unit that nothing set, when
         * pdVibSetUnitInfo fails. its failing path stores no *unit (BRA at
         * 0x8c137e5e), and MOV.L @R15,R4 at 0x8c137f9c reads unit all the
         * same */
        vib->unit_func(unit, err, &unit_info, vib->param);
    }
    if (err == PDD_VIBERR_OK && vib->info.units > vib->unit) {
        vib->unit++;
        /* @bug logic: drops kdSetGetMediaInfoEx's result (JSR @R3 at
         * 0x8c137fbc; nothing reads R0 before the RTS at 0x8c137fc8). a
         * request Kaede refuses registers no callback, so the walk ends
         * there with no call to unit_func */
        kdSetGetMediaInfoEx(device, KDD_FUNC_VIBRATION, vib->unit,
                            pdVibEnumUnitCallback);
    }
}

Sint32 pdVibEnumerateUnit(Uint32 port, PD_VIBENUMUNITCALLBACK func,
                          Uint32 param)
{
    struct vib_port *vib = &pdgVib[port];
    /* @hack spelling: every result through err to one return, where a
     * return on each path would do */
    Sint32 err;

    if (vib->busy) {
        err = PDD_VIBERR_BUSY;
    } else {
        vib->busy = TRUE;
        vib->unit_info = NULL;
        vib->param = param;
        vib->info_func = NULL;
        vib->unit_func = func;
        vib->unit = 1;
        err = pdVibGetInfo(port, &vib->info);
        /* @bug logic: leaves busy set when pdVibGetInfo finds no pack. BF at
         * 0x8c138002 returns its result, past the clear at 0x8c138016. no
         * request is under way whose answer would clear busy, so every later
         * call for the port returns PDD_VIBERR_BUSY */
        if (err >= 0) {
            err = kdSetGetMediaInfoEx(port, KDD_FUNC_VIBRATION, 1,
                                      pdVibEnumUnitCallback);
            if (err < 0) {
                vib->busy = FALSE;
                err = PDD_VIBERR_BUSY;
            } else {
                err = PDD_VIBERR_OK;
            }
        }
    }
    return err;
}

Sint32 pdVibGetDirection(Uint32 port)
{
    const KDS_DEVICEINFO *root;
    const KDS_DEVICEINFO *pack;
    Sint32 socket;

    root = kdGetDeviceInfo(vib_root_ports[port]);
    pack = kdGetDeviceInfo(port);
    switch (port) {
    case PDD_PORT_A2:
    case PDD_PORT_B2:
    case PDD_PORT_C2:
    case PDD_PORT_D2:
        socket = 1;
        break;
    default:
        socket = 0;
        break;
    }
    return vib_directions[(KD_CONNECTOR_DIRS * root->connector_dir[socket]) +
                          pack->connector_dir[0]];
}
