/* sysmain: the modem's controller, driven a pass at a time */
#ifndef MODEM_SYSMAIN_H
#define MODEM_SYSMAIN_H

/* reset the controller */
void sysmainOpen(void);
/* do nothing */
void sysmainClose(void);
/* run one pass of the controller */
void sysmainCall(void);

#endif
