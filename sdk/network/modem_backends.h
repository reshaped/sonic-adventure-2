/* the three modem drivers: the G2 modem, the SH-4's SCIF and a UART. each
 * gives the same calls under a prefix of its own. the calls only some of
 * them give follow */
#ifndef NETWORK_MODEM_BACKENDS_H
#define NETWORK_MODEM_BACKENDS_H
#include <mdmAPI.h>

#define DECLARE_MODEM_BACKEND(name)                                            \
    void name##_drop_dtr(MDM handle);                                          \
    void name##_enable_dtr(MDM handle);                                        \
    int name##_rx_available(MDM handle);                                       \
    int name##_rx_free(MDM handle);                                            \
    int name##_tx_available(MDM handle);                                       \
    int name##_tx_free(MDM handle);                                            \
    void name##_init(char *tx, char *rx, int tx_size, int rx_size);            \
    MDM name##_open(MD_DEVICE device);                                         \
    int name##_read(MDM handle, char *buffer, int size);                       \
    int name##_write(MDM handle, char *buffer, int size);                      \
    void name##_set_default(MDM handle, MDS_DCB state);                        \
    void name##_get_state(MDM handle, MDS_DCB *state);                         \
    void name##_purge_tx(MDM handle);                                          \
    void name##_purge_rx(MDM handle);                                          \
    void name##_close(MDM handle);                                             \
    void name##_finish(void)
DECLARE_MODEM_BACKEND(g2_modem);
DECLARE_MODEM_BACKEND(scif_modem);
DECLARE_MODEM_BACKEND(uart_modem);
#undef DECLARE_MODEM_BACKEND

void scif_modem_set_state(MDM handle, MDS_DCB state);
void uart_modem_set_state(MDM handle, MDS_DCB state);
void scif_modem_get_default(MDS_DCB *state);
void uart_modem_get_default(MDS_DCB *state);
void g2_modem_select(MD_DEVICE device);
void uart_modem_select(MD_DEVICE device);
#endif
