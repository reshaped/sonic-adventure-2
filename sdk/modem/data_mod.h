/* data_mod: the controller's work while the modem is online */
#ifndef MODEM_DATA_MOD_H
#define MODEM_DATA_MOD_H
#include <sg_xpt.h>

/* the checks scm_main_core makes while online, before normal_data_mode.
 * where STATUS_BITS holds STATUS_LINKING, it calls ini_link. failing that,
 * where BMRregisters[27] holds BMR27_LINK_DOWN, it sets scm_main_phase to
 * DRAIN_PHASE. it also sets data_pass_timer to run out a second later. it
 * returns TRUE where either check holds, and FALSE otherwise */
char common_job_in_data_mode(void);
/* pass the bytes waiting on each side to the other. the host's go to the
 * line first, then the line's to the host. a side's bytes pass only where
 * the other side has room for all of them. otherwise they wait for a later
 * call */
void normal_data_mode(void);

#endif
