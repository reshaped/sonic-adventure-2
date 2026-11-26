/* line_mon: the line monitor, which keeps a connection up as its line
 * changes. it retrains the link, steps the receive rate down or up, or hangs
 * up */
#ifndef MODEM_LINE_MON_H
#define MODEM_LINE_MON_H
#include <sg_xpt.h>

/* the line's last SNR_SAMPLES grades from dp_snr_ratio, a window that
 * snr_win_pnt fills in turn. last_win_was_HIGH is set after one window of bad
 * grades. a second steps the rate down or retrains */
#define SNR_SAMPLES 16
extern Uint8 snr_window[SNR_SAMPLES];
extern Uint8 snr_win_pnt, last_win_was_HIGH;

/* line_monitor_phase's values. LINE_COMMAND_RETRAIN starts the retrain AT O1
 * asks for */
enum line_monitor_phase_id {
    LINE_RESET,
    LINE_SAMPLE_RESET,
    LINE_INITIAL_SAMPLES,
    LINE_MONITORING,
    LINE_COMMAND_RETRAIN,
    LINE_V22BIS_RETRAIN,
    LINE_RETRAIN,
    LINE_RESUME,
    LINE_HANG_UP
};

/* follow one pass of line_monitor_phase. the call does nothing unless the
 * connection is in data mode or in its command mode */
void line_monitor(void);

#endif
