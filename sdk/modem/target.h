/* target: the modem's hardware around the data pump, as far as the sources
 * use it */
#ifndef MODEM_TARGET_H
#define MODEM_TARGET_H
#include <sg_xpt.h>

#include "countrydb.h"
#include "dspdb.h"
#include "osif.h"
#include "roc_k56.h"

/* keep api, not a copy of it, to read the country database through */
void targetSetOptApi(struct SegaOptionalInterface *api);
/* do nothing */
void targetOpen(void);
/* clear the hardware's flags, target_HwFault's among them, and hold the data
 * pump in reset */
void targetClose(void);
/* return dp_test's result */
char target_test_dp(void);
/* time the breaks to and from the host. ModemMainCall calls this every
 * 10 ms. a break to the host ends once DTE_BK_TIME runs out, and the call
 * then runs vxd_send_next_dte_break. a break from the host goes to
 * vxd_end_of_dte_brk_rx as it ends, with its length in 10 ms ticks */
void break_check(void);
/* do nothing */
void target_dpNeedsHookControl(Uint8 on);
/* hold the data pump in reset for a reset of 1, and release it for any
 * other */
void target_dp_reset(char reset);
/* report code, an error of the data pump. every DP_ERR_ code but
 * DP_ERR_INTERRUPT also marks the hardware failed */
void data_pump_error(Uint32 code);
/* return the address of the data pump's registers */
volatile struct dp_registers *targetGetComMemInterfaceAddr(void);
/* hook ModemIntrCall on the data pump's interrupt, or take it off where
 * handler is null. handler is only tested against null */
void target_InstallDpIrq(void (*handler)(void));
/* return 1 once data_pump_error has marked the hardware failed, and 0
 * before. targetClose clears the mark */
Uint8 target_HwFault(void);

/* Select a country profile, check its availability and obtain the active
 * hardware country record and its register limits. */
void target_set_country(Uint8 code);
char target_check_country(Uint8 code);
void targetGetCountryInfo(Uint8 code, struct country_st **record,
                          struct Slimit **limits);

/* Initialize the target controller for zero or poll it for one. */
void target_control(Uint8 phase);

/* Hardware hooks for the common Rockwell controller. Pins, external clocks
 * and the individual DCE cable connections have no operation on this board. */
char li_get_lcs(void);
char li_get_ring(void);
void li_set_hook(Uint8 on);
void li_set_mute(char on);
void li_set_pulse(Uint8 on);
void li_set_rj11(char on);
void target_dip_switch(Uint8 *registers);
void target_test_pin(Uint8 pin, Uint8 state);
char target_get_TD(void);
void target_local_digital_loop(char on);
void target_set_TCI_timer(Uint16 ticks);
void target_set_volume(Uint8 level);
char target_set_analog(Uint8 on);
void target_apply_extclk(Uint8 transmit, Uint8 receive);
void target_remove_extclk(void);
void led_control(void);
void set_direct_sync_interface(void);
void set_direct_async_interface(void);
void set_normal_interface(void);
void set_v25bis_sync_interface(void);
void disc_dce_clocks(void);
void connect_dce_clocks(void);
void disc_dce_tx_rx(void);
void connect_dce_tx_rx(void);
void SET_75BPS_TX(Uint8 on);
void SET_75BPS_RX(void);
void RESTART_SYSTEM(void);
char *target_id(void);
char *target_send_dp_version(void);
void target_message(char *text);
void targetSetComMemInterfaceAddr(Uint32 address);
void targetReadCurrentSregistersFile(void);
void targetSaveCurrentSregistersFile(void);
void targetReadCurrentFaxFrameFile(void);
void targetSaveCurrentFaxFrameFile(void);
Uint32 targetGetRealTimeClock(void);
Uint32 Compute_YYYYMM_to_seconds(volatile struct Sys_Time date);
char IsBissextile(char year);
char targetIsDpIrqInstalled(void);
void target_set_earth_relay(Uint8 on);
Uint8 targetGetCountryCode(void);
Uint8 targetGetDAA_Type(void);

#endif
