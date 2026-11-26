#ifndef NETWORK_SCIF_SERIAL_H
#define NETWORK_SCIF_SERIAL_H
#include <mdmAPI.h>
#include <sg_xpt.h>

extern volatile Bool scif_tx_paused, scif_rx_paused;
extern unsigned int scif_speed;
extern int scif_receive_count, scif_transmit_count, scif_error_count,
    scif_break_count;
extern int scif_line_breaks, scif_overruns;
extern volatile char scif_readback;
void scif_receive_interrupt(void);
void scif_transmit_interrupt(void);
void scif_error_interrupt(void);
void scif_break_interrupt(void);
void scif_set_speed(unsigned int speed);
void scif_install(void);
void scif_remove(void);
void scif_set_baud(Float rate);
/* scif_modem_set_default in the SHC build, declared as the driver calls it,
 * with the state alone. the modem API calls the same entry with a port
 * first, which the entry never reads */
void scif_apply_default(MDS_DCB state);

#endif
