/* the console's broadcast standard, and the display it and the video cable
 * call for */
#include <sg_xpt.h>
#include <shinobi.h>

/* the byte of kbCfgGetId's record that holds the broadcast standard, an
 * ASCII digit */
#define ID_BROADCAST 12

SYE_CBL_BROADCAST syCblCheckBroadcast(void)
{
    /* @hack spelling: the record's address through record, not read from
     * kbCfgGetId's result in place */
    Sint8 *record;
    /* @hack spelling: the standard through broadcast to one return, not
     * returned from each case */
    SYE_CBL_BROADCAST broadcast;

    kbCfgIni(KBD_CFGINI);
    record = kbCfgGetId();
    /* @hack redundant: the case for '0', which does what the default does */
    switch (record[ID_BROADCAST]) {
    case '1':
        broadcast = SYE_CBL_BROADCAST_PAL;
        break;
    case '2':
        broadcast = SYE_CBL_BROADCAST_PALM;
        break;
    case '3':
        broadcast = SYE_CBL_BROADCAST_PALN;
        break;
    case '0':
    default:
        broadcast = SYE_CBL_BROADCAST_NTSC;
        break;
    }
    return broadcast;
}

SYE_CBL syCblCheck(void)
{
    SYE_CBL_CABLE cable;
    SYE_CBL_BROADCAST broadcast;

    cable = syCblCheckCable();
    broadcast = syCblCheckBroadcast();
    if (cable == SYE_CBL_CABLE_VGA) {
        return SYE_CBL_VGA;
    }
    /* @hack redundant: the cases for PAL-M and NTSC, which do what the
     * default does */
    switch (broadcast) {
    case SYE_CBL_BROADCAST_PAL:
    case SYE_CBL_BROADCAST_PALN:
        return SYE_CBL_PAL;
    case SYE_CBL_BROADCAST_PALM:
    case SYE_CBL_BROADCAST_NTSC:
    default:
        return SYE_CBL_NTSC;
    }
}
