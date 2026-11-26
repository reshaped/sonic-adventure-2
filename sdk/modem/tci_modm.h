/* tci_modm: the controller's timers and its test of escape text, as far as
 * the sources use them */
#ifndef MODEM_TCI_MODM_H
#define MODEM_TCI_MODM_H
#include <sg_xpt.h>

/* run the controller's timers. ModemMainCall calls this every 10 ms */
void TCI_timer(void);
/* test byte, the next byte of the text that follows an escape. a byte that
 * fails returns 0 */
char TIES_validation(Uint8 byte);

void TCI_got_format(Uint8 speed, Uint8 format);

void TCI_sw_reset(void);

void TCI_dte_rx_cmd_byte(Uint8 byte);

void restart_TIES(void);
/* test value, the last byte of ties_cmd_buff, as the next of a TIES command.
 * it returns 0 for a byte no command takes there */
char ties_analysis(Uint8 value);
/* take value for a command's prefix, as & is, and return whether it is one */
char get_cmd_prefix(Uint8 value);
/* read the number before value, the last byte of ties_cmd_buff. it returns 0
 * while value is a digit, and otherwise sets *number, 0 for no digits, and
 * returns 1. the GCC side returns -1 for a number past 255, or a ties_cmd_ptr
 * outside the buffer */
char get_cmd_argument(Uint8 value, Uint8 *number);
void TCI_tx_response(void);

#endif
