/* the RTL8139 driver's state, which the interrupt handler and the card's
 * setup share. DLD_pd_rcv806 and DLD_pd_rcv800 are the receivers registered
 * for ARP and IP. no code reads or sets DLD_pd_rcvall */
#include <sg_chain.h>
#include <sg_xpt.h>
#include "../network/packet_driver.h"
#include "../nexgen/dld_state.h"
#include "../nexgen/rtl8139.h"

char *const dl_driver_build = "\ndl Ver 0.36 Build:Nov 22 2000 16:48:23\n";
void *dl_interrupt_argument = NULL;
pd_receiver DLD_pd_rcv806;
pd_receiver DLD_pd_rcv800;
pd_receiver DLD_pd_rcvall;
struct rtl_state dl_adapter_state;
struct rtl_config dl_adapter_config;
struct rtl_state *dl_adapter;
struct rtl_config *dl_config;
volatile Sint32 dl_receive_test;
SYCHAIN dl_interrupt;
