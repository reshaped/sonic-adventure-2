#ifndef ASK_SESSION_H
#define ASK_SESSION_H
#include <sg_xpt.h>

extern int func_mode, flush_mode, lock_mode, echo_lock;
/* func_mode's states */
enum {
    ASK_SESSION_CLOSED = 0,
    ASK_SESSION_EDITING = 1,
    ASK_SESSION_CONVERTING = 2,
    ASK_SESSION_SELECTING = 3
};
extern Uint8 prev_yomi[80], prev_tango[80];
extern Uint8 *entry2code;
extern int entry2upd;
#endif
