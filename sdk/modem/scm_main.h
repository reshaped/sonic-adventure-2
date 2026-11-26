/* scm_main: the modem's controller, as far as the sources use it */
#ifndef MODEM_SCM_MAIN_H
#define MODEM_SCM_MAIN_H

void scm_main(void);
/* reset the controller */
void scm_main_init(void);
/* run one pass of the controller. what a pass does depends on
 * scm_main_phase. it does nothing while target_HwFault returns 1 */
void scm_main_core(void);

char rx_dte_empty(void);
void scm_set_PTT_homologation_phase(void);

#endif
