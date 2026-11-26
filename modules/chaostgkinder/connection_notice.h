/* the notice a menu shows while it waits on the server. a cancel at the
 * notice closes it and opens a confirmation. the confirmation's yes marks
 * the wait cancelled, and its no shows the notice again. a cancel at the
 * confirmation closes it, and leaves the notice closed while the wait goes
 * on */
#ifndef CHAOSTGKINDER_CONNECTION_NOTICE_H
#define CHAOSTGKINDER_CONNECTION_NOTICE_H
#include <sg_xpt.h>

struct task;
/* start the notice closed, with a task under parent */
void connection_notice_start(struct task *parent);
/* show the notice if nothing shows and the wait isn't cancelled */
void connection_notice_open(void);
/* close the notice if it shows. the confirmation stays */
void connection_notice_close(void);
/* whether the confirmation's yes marked the wait cancelled. it stays so
 * until connection_notice_start */
Bool connection_notice_cancelled(void);
#endif
