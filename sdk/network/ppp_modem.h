/* the PPP library's one modem port. AvepppInitialize2 opens the modem with
 * these rings where it is given none */
#ifndef NETWORK_PPP_MODEM_H
#define NETWORK_PPP_MODEM_H
#include <access/aveppp.h>
#include <mdmAPI.h>

#define PPP_MODEM_BUFFER_BYTES 3200
#define PPP_DISCONNECT_SCRIPT_BYTES 512
extern long PPP_ConnectionStatus, PPP_LastError, PPP_ComUsing;
extern long PP_ParamMyIp, PP_ParamHisIp;
extern long PP_ParamMyDns1, PP_ParamMyDns2, PP_ParamHisDns1, PP_ParamHisDns2;
extern char store_login_script3[PPP_DISCONNECT_SCRIPT_BYTES];
extern MD_DEVICE ppp_modem_device;
extern MDM ppp_modem_port[1];
extern char ppp_rx_storage[PPP_MODEM_BUFFER_BYTES];
extern char ppp_tx_storage[PPP_MODEM_BUFFER_BYTES];
/* ppp_modem_buffer_size's queries of the port's rings. it returns -1 for any
 * other */
enum { PPP_RX_AVAILABLE, PPP_RX_FREE, PPP_TX_AVAILABLE, PPP_TX_FREE };
int ppp_modem_buffer_size(int query);
int ppp_set_modem_device(MD_DEVICE device);
void ppp_ini_recv_buff(void);
/* pppcall's commands. it returns -99 for any other */
enum { PPPCALL_OPEN, PPPCALL_CLOSE, PPPCALL_STATUS, PPPCALL_COUNTERS };
int pppcall(int command, const AvepppOpenParam *parameters, void *result);
#endif
