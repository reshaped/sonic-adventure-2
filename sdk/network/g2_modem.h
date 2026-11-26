/* the G2 modem driver's state, which g2_modem.c shares with the calls the
 * modem makes into g2_callbacks.c. each ring is capacity bytes from buffer,
 * and its space counts the bytes free. the host writes the transmit ring at
 * g2_tx_writer, and the modem reads it at g2_tx_reader. the modem writes the
 * receive ring at g2_rx_writer, and the host reads it at g2_rx_reader */
#ifndef NETWORK_G2_MODEM_H
#define NETWORK_G2_MODEM_H
#include <sg_xpt.h>
#include <access/dte_if.h>

extern struct dte_line g2_line; /* the host's line settings */
/* the modem's status register, as the modem last set it */
extern struct dteModemStatusRegister g2_status;
/* g2_event is the word the driver passes set_event, and g2_event_mask the
 * events the modem takes */
extern struct _DTEDEV_SETEVENT g2_event, g2_event_mask;
extern Uint32 g2_tx_capacity;
extern Uint8 *g2_tx_buffer, *g2_tx_reader, *g2_tx_writer;
extern int g2_tx_space;
extern Uint32 g2_rx_capacity;
extern Uint8 *g2_rx_buffer, *g2_rx_reader, *g2_rx_writer;
extern int g2_rx_space;
/* the ddal_api calls g2_modem_open gives the modem */
struct ddal_result *GetTxBuffer(struct ddalDataTransfer *transfer);
struct ddal_result *SetRxBuffer(struct ddalDataTransfer *transfer);
void SetModemStatusRegister(struct dteModemStatusRegister *status);
struct dte_line *GetModemControlRegister(void);
void SetEventMask(struct _DTEDEV_SETEVENT *mask);

#endif
