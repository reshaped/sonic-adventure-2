/* the controller's work while the modem is online, passing bytes between
 * the host and the line */
#include <sg_xpt.h>

#include "data_mod.h"
#include "dce.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"

/* how long DRAIN_PHASE waits at most: a second, in 10 ms ticks */
#define DATA_PASS_TICKS 100

char common_job_in_data_mode(void)
{
    if (STATUS_BITS & STATUS_LINKING) {
        ini_link();
        return TRUE;
    }
    if (BMRregisters[27] & BMR27_LINK_DOWN) {
        setup_timer(&data_pass_timer, DATA_PASS_TICKS);
        scm_main_phase = DRAIN_PHASE;
        return TRUE;
    }
    return FALSE;
}

void normal_data_mode(void)
{
    Uint8 *from;
    Uint8 *to;
    Uint16 length;
    Uint16 left;

    vxd_dte_get_rx_data(&from, &length);
    if (length) {
        if (dce_set_tx_data(&to) >= length) {
            left = length;
            while (left--) {
                *to++ = *from++;
            }
            dce_tx_data_done(length, 1);
            vxd_dte_rx_data_done();
        }
    }
    dce_get_rx_data(&from, &length);
    if (length) {
        if (vxd_dte_set_tx_data(&to) >= length) {
            left = length;
            while (left--) {
                *to++ = *from++;
            }
            vxd_dte_tx_data_done(length);
            dce_rx_data_done();
        }
    }
}
