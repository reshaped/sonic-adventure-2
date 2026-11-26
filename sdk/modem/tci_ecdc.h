/* Break and timer events for the modem's link controller. */
#ifndef MODEM_TCI_ECDC_H
#define MODEM_TCI_ECDC_H
#include <sg_xpt.h>

/* Break lengths count 10 ms intervals. The get call takes the oldest break
 * the host queued for an error-corrected link and returns zero if empty. */
void TCI_dce_brk_rx(Uint16 length);
void TCI_dte_brk_rx(Uint16 length);
Uint16 TCI_get_dte_brk(void);
void TCI_dte_esc_seq_found(void);

/* Reset both interfaces and return the controller to command mode. */
void TCI_sw_reset(void);
/* Advance the host and line inactivity timers. Call every 10 ms. */
void TCI_timer_10ms(void);
#endif
