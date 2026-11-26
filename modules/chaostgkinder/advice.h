/* the advice menu's calls that services.h doesn't declare */
#ifndef CHAOSTGKINDER_ADVICE_H
#define CHAOSTGKINDER_ADVICE_H
#include <sg_xpt.h>

/* end the menu at its task's next step. without a menu, do nothing */
void kinder_advice_close(void);
/* set the menu's state, an enum advice_state. without a menu, do nothing */
void kinder_advice_set_state(Sint8 state);

#endif
