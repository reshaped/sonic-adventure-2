/* v42: LAPM, V.42's error control */
#ifndef MODEM_V42_H
#define MODEM_V42_H
#include <sg_xpt.h>

/* run LAPM once. pass data between the host and LAPM's rings, except in a
 * direction V.42bis codes. the host's data waits while BMR25_BREAK_PENDING
 * is set. take the frames the line received, and send what is due. the first
 * call after clear_EC_ram starts the link, and an originator sends its XID */
void v42(void);
/* take each frame the line has received, and send what it calls for. it
 * takes none while enter_disconnect is set. while a change to a 32-bit FCS is
 * pending, it takes one frame at most */
void v42_empty_rx(void);
/* take the oldest break from the line that waits in RxBreakLength. return
 * its length in 10 ms ticks, or 0 for none */
Uint16 get_EC_brk_length(void);

#endif
