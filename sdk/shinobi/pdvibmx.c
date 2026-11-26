/* the vibration calls for a pack's unit 1 alone, over pdvib's calls */
#include <kaede.h>
#include <sg_pdvib.h>
#include <sg_xpt.h>

/* the unit these calls drive */
#define UNIT 1
/* UNIT's bit in a stop time block's mask, whose high byte comes first */
#define UNIT_MASK_HIGH 0
#define UNIT_MASK_LOW (1 << UNIT)
/* the block's words of 4 bytes: the 2-byte mask, UNIT's time and a byte of
 * padding */
#define STOP_BLOCK_WORDS 1

/* UNIT at power 0 */
static const PDS_VIBPARAM vib_stop = {UNIT, 0, 0, 0, 0, {0, 0, 0}};

Sint32 pdVibMxIsReady(Uint32 port)
{
    return pdVibIsReady(port);
}

Sint32 pdVibMxSetStopTime(Uint32 port, Uint32 time)
{
    Uint32 block;
    Uint8 *byte;
    Sint32 err;

    if (!pdVibMxIsReady(port)) {
        return PDD_VIBERR_NO_VIBRATOR;
    }
    block = 0;
    /* @hack spelling: a cursor over block's bytes, where indexing them would
     * do */
    byte = (Uint8 *)&block;
    *byte++ = UNIT_MASK_HIGH;
    *byte++ = UNIT_MASK_LOW;
    *byte++ = time;
    /* @hack spelling: err, where the call in the test would do */
    err = kdSetBlockWrite(port, KDD_FUNC_VIBRATION, 0, 0, 0, &block,
                          STOP_BLOCK_WORDS);
    if (err < 0) {
        return PDD_VIBERR_BUSY;
    }
    return PDD_VIBERR_OK;
}

Sint32 pdVibMxStart(Uint32 port, const PDS_VIBPARAM *param)
{
    if (!pdVibMxIsReady(port)) {
        return PDD_VIBERR_NO_VIBRATOR;
    }
    return pdVibStart(port, param, 1);
}

Sint32 pdVibMxStop(Uint32 port)
{
    if (!pdVibMxIsReady(port)) {
        return PDD_VIBERR_NO_VIBRATOR;
    }
    return pdVibStart(port, &vib_stop, 1);
}
