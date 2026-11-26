/* ecdcutil: the modem's 10 ms timers, link transitions and choice of
 * interfaces */
#ifndef MODEM_ECDCUTIL_H
#define MODEM_ECDCUTIL_H
#include <sg_xpt.h>

/* set *deadline to osifGet10msCount's reading plus ticks */
void setup_timer(Uint32 *deadline, Uint32 ticks);
#define TICKS_PER_SECOND 100
/* return 1 once osifGet10msCount's reading reaches deadline, and 0 before.
 * the call keeps the reading in TIM_10MS */
char timer_expired(Uint32 deadline);
/* utilStartMeasurementTimer and utilReadMeasurementTimer read TIM_10MS as
 * timer_expired last took it, and never the clock itself */
void utilStartMeasurementTimer(Uint32 *start);
Uint32 utilReadMeasurementTimer(Uint32 start);

/* reset the controller, or set scm_main_phase to ESTABLISH_PHASE or to
 * DATA_PHASE, as the controller's state decides */
void ini_link(void);
/* enter DATA_PHASE. link_made keeps the link's BMR settings for the host's
 * AT responses, and reports the connection */
void link_made(void);
void re_configure_dce(void);
/* modes configure_dce takes beside dce_set_interface's types of line. each
 * passes DCE_ASYNC with a format of its own, which v14Initialize reads.
 * DCE_FORMAT_FF, for V.42's detection phase, passes 0xff. DCE_FORMAT_FE, for
 * MNP over asynchronous characters, passes 0xfe */
#define DCE_FORMAT_FF 100
#define DCE_FORMAT_FE 101
void configure_dce(Uint8 mode);
void re_configure_dte(void);
void configure_dte(Uint8 mode);
#endif
