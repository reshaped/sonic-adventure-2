/* bl: the dialing restrictions of the active country's modem profile */
#ifndef MODEM_BL_H
#define MODEM_BL_H
#include <sg_xpt.h>
#define BL_ALLOWED 1
#define BL_DELAYED 2
#define BL_BARRED 3
void BL_Dial_Delay(void);
void BL_Reset(void);
/* check the AT dial string up to its command terminator, and return
 * BL_ALLOWED, BL_DELAYED or BL_BARRED */
Uint8 BL_Checking(Uint8 *number);
/* the events of a call that BL_Condition takes, a bit each. cur_cnt's masks
 * say which count as a failure, and which clear blacklist records */
#define BL_EVENT_ABORTED 0x01
#define BL_EVENT_DTR_OFF 0x02
#define BL_EVENT_HANDSHAKE_TIMEOUT 0x04
#define BL_EVENT_BUSY 0x08
#define BL_EVENT_NO_ANSWER 0x10
#define BL_EVENT_ANSWER_TONE 0x40
#define BL_EVENT_CONNECTED 0x80
void BL_Condition(Uint8 event);
/* what BL_show_struct sends the terminal: the entries, or the clock's days
 * and time. BL_SHOW_CLEAR clears the entries instead */
#define BL_SHOW_ENTRIES 0
#define BL_SHOW_TIME 1
#define BL_SHOW_CLEAR 'K'
void BL_show_struct(Uint8 which);
/* point *buffer at the blacklist's table, and return its size in bytes */
Uint16 BL_GetBlackListBuffer(Uint8 **buffer);
#endif
