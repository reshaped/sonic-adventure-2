/* the video cable, as the A/V connector's two cable pins report it */
#include <sg_xpt.h>
#include <shinobi.h>

/* PDTRA, the SH-4's port A data register. bits 8 and 9 hold the A/V
 * connector's cable pins */
/* @hack type: PDTRA not volatile, where a volatile register would do */
#define PDTRA (*(Uint16 *)0xff800030)
#define CABLE_PINS 0x0300
#define CABLE_SHIFT 8

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the library's version and build time */
const char *const sygCblBuild = "\nsyCbl Ver 1.10 Build:Apr 27 1999 13:54:09\n";
/* PDTRA's cable pins, left in bits 8 and 9 */
Uint32 syCblCheckCableType(void);
/* NOLINTEND(misc-use-internal-linkage) */

SYE_CBL_CABLE syCblCheckCable(void)
{
    /* @hack spelling: cable, the result through a local, where return would
     * take the expression */
    SYE_CBL_CABLE cable;

    cable = (SYE_CBL_CABLE)((syCblCheckCableType() >> CABLE_SHIFT) &
                            (CABLE_PINS >> CABLE_SHIFT));
    return cable;
}

Uint32 syCblCheckCableType(void)
{
    /* @hack spelling: pins, the register's read through a local, where return
     * would take the read */
    Uint16 pins;

    pins = PDTRA;
    return pins & CABLE_PINS;
}
