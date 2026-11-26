/* vxd_int: the modem's side of the serial port, as far as the sources use
 * it */
#ifndef MODEM_VXD_INT_H
#define MODEM_VXD_INT_H
#include <sg_xpt.h>

/* serve the events the host's side has raised in the port's md_ier */
void com_memory_interrupt(void);
/* pass baud to the controller's TCI_got_format (0x0cae9eb4). then make the
 * port's baud its speed, and raise COM_EVENT_SPEED */
void save_rate(Uint8 baud);

/* Last sampled port registers. The modem archive exports SHADDOW with
 * this spelling. */
extern Uint8 PCRWER_SHADDOW;
extern Uint8 MD_IER_SHADDOW;
extern Uint8 RATE_SHADDOW;
extern Uint8 LCR_SHADDOW;
extern Uint8 MCR_SHADDOW;

void init_com_memory(void);
void empty_messg_buffer(void);
void update_PC_rx_cnt(void);
void update_PC_tx_cnt(void);
void rx_cmd_buff_rdy(void);
void save_LCR(void);
void get_format_from_PC(void);
void check_break_rx_from_PC(void);
void save_MCR(void);
/* count an escape character, dte_S2, that ends the receive buffer the host's
 * side fills. the third starts the guard time vxd_esc_seq_timr runs, and a
 * fourth starts the count over. under BMR21_LEAD_GUARD, a character counts
 * only once the guard time has passed since ESC_TOUT, and an earlier one
 * starts that wait over. while BMR21_TEXT_VALID is set, the character goes to
 * TIES_validation, and counts as the first only where that refuses it */
void escape_code(void);
void reset_esc_seq(void);

#endif
