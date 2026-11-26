#ifndef NEXGEN_DLD_STATE_H
#define NEXGEN_DLD_STATE_H
#include <sg_chain.h>
#include "../network/packet_driver.h"
#include "../nexgen/rtl8139.h"

extern char *const dl_driver_build;
extern void *dl_interrupt_argument;
extern pd_receiver DLD_pd_rcv806;
extern pd_receiver DLD_pd_rcv800;
extern pd_receiver DLD_pd_rcvall;
extern struct rtl_state dl_adapter_state;
extern struct rtl_config dl_adapter_config;
extern struct rtl_state *dl_adapter;
extern struct rtl_config *dl_config;
/* ETD_DiagMain sets this to -1 before it loops a test frame back. the
 * receive handler then sets it to 0 for the next frame, which it hands to no
 * receiver */
extern volatile Sint32 dl_receive_test;

extern SYCHAIN dl_interrupt;
#endif
