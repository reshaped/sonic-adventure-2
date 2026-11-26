#include <sg_xpt.h>
#include "anxdef.h"
#include "bl.h"
#include "command.h"
#include "data_mod.h"
#include "dce.h"
#include "dte_vxd.h"
#include "ec_estab.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "line_mon.h"
#include "maintain.h"
#include "modemdef.h"
#include "scm_main.h"
#include "target.h"
#include "tci_ecdc.h"
#include "util_mod.h"
#include "v42.h"
#include "v42bis.h"
#include "v42def.h"

/* in setup_timer's 10 ms ticks */
#define HANGUP_GUARD_TICKS 100
#define RESET_DELAY_TICKS 10

/* the message for a CardBus modem the library can't reach. nothing reads it */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
const char *pstrDperror = "Problem accessing CardBus modem section.";

void scm_main(void)
{
    scm_main_init();
    for (;;) {
        scm_main_core();
    }
}

char rx_dte_empty(void)
{
    Uint8 *data;
    Uint16 length;
    vxd_dte_get_rx_data(&data, &length);
    if (length) {
        return 0;
    }
    return 1;
}

void scm_main_init(void)
{
    no_connection_yet = 1;
    target_control(0);
    cmd_DIAG();
    fhud_failed = 0;
    modem_reset(RESET_POWER_UP);
    maintain(0);
    TCI_sw_reset();
    set_command_mode();
    BL_Reset();
}

void scm_set_PTT_homologation_phase(void)
{
    BMR(7) &= ~BMR7_HOST_BYTE;
    scm_main_phase = HOMOLOGATION_PHASE;
}

void scm_main_core(void)
{
    if (target_HwFault()) {
        return;
    }
    maintain(1);
    target_control(1);
    switch (scm_main_phase) {
    case RESET_PHASE:
        TCI_sw_reset();
        break;
    case IDLE_PHASE:
        DTE_INAC_TIM = 0;
        DCE_INAC_TIM = 0;
        if (STATUS_BITS & STATUS_LINKING) {
            ini_link();
        }
        break;
    case ESTABLISH_PHASE:
        if (line_monitor_phase > LINE_MONITORING) {
            break;
        }
        ec_estab();
        break;
    case HALT_AFTER_LINE_PHASE:
        if (!(BMR(27) & BMR27_LAPM) && !(BMR(29) & BMR29_MNP)) {
            TCI_sw_reset();
            break;
        }
        /* once the peer has acknowledged all the host sent, take the link
         * down */
        if (rx_dte_empty() && !TM_CNT) {
            reason_code = 0xff;
            user_code = 0xff;
            if (!(STATUS_BITS & STATUS_HANGUP)) {
                setup_timer(&fhud_timer, HANGUP_GUARD_TICKS);
            }
            STATUS_BITS |= STATUS_HANGUP;
        }
        if (timer_expired(fhud_timer)) {
            if (!(STATUS_BITS & STATUS_HANGUP)) {
                fhud_failed = 1;
            }
            TCI_sw_reset();
            break;
        }
        /* fall through: HALT_AFTER_LINE_PHASE passes data as DATA_PHASE does */
    case DATA_PHASE:
        if (line_monitor_phase > LINE_MONITORING) {
            break;
        }
        if (STATUS_BITS & STATUS_INACTIVE) {
            force_h0_cmd();
            DTE_INAC_TIM = 0;
            DCE_INAC_TIM = 0;
            STATUS_BITS &= ~STATUS_INACTIVE;
        }
        if (common_job_in_data_mode()) {
            break;
        }
        if (BMR(27) & BMR27_LAPM) {
            if (BMR(29) & BMR29_COMPRESSION) {
                v42bis(1);
            }
            v42();
            if (BMR(29) & BMR29_COMPRESSION) {
                v42bis(0);
            }
        } else if (BMR(29) & BMR29_MNP) {
            if (BMR(29) & BMR29_COMPRESSION) {
                if (BMR(29) & BMR29_V42BIS) {
                    v42bis(1);
                }
            }
            if (BMR(29) & BMR29_COMPRESSION) {
                if (BMR(29) & BMR29_V42BIS) {
                    v42bis(0);
                }
            }
        } else if (!(BMR(28) & 0xc0) && !(SREG(16) & 0x0c)) {
            normal_data_mode();
        }
        break;
    case HALT_AFTER_TERMINAL_PHASE:
        if (timer_expired(fhud_timer) || vxd_dte_tx_buff_empty(1)) {
            if (!vxd_dte_tx_buff_empty(1)) {
                fhud_failed = 1;
            }
            TCI_sw_reset();
        } else if (BMR(27) & BMR27_LAPM) {
            if (BMR(29) & BMR29_COMPRESSION) {
                v42bis(1);
            }
            v42();
            if (BMR(29) & BMR29_COMPRESSION) {
                v42bis(0);
            }
        } else if (BMR(29) & BMR29_MNP) {
            if (BMR(29) & BMR29_COMPRESSION) {
                if (BMR(29) & BMR29_V42BIS) {
                    v42bis(1);
                } else {
                    /* @hack redundant: an empty else */
                }
            }
            if (BMR(29) & BMR29_COMPRESSION) {
                if (BMR(29) & BMR29_V42BIS) {
                    v42bis(0);
                }
            }
        }
        return;
    case DRAIN_PHASE:
        /* @hack type: dce_tx_buff_empty's Uint8 read as a char, where the
         * Uint8 would do */
        if ((char)dce_tx_buff_empty() || timer_expired(data_pass_timer)) {
            if (BMR(28) & BMR28_HALT_AFTER_LINE) {
                setup_timer(&data_pass_timer, RESET_DELAY_TICKS);
                scm_main_phase = RESET_DELAY_PHASE;
            } else if ((BMR(29) & BMR29_MNP) && reason_code == 0) {
                BMR(27) = 0;
                BMR(29) = 0;
                STATUS_BITS |= STATUS_LINKING;
                ini_link();
            } else if (STATUS_BITS & STATUS_HANGUP) {
                TCI_sw_reset();
            } else {
                cmd_parameter_buffer[0] = HALT_AFTER_TERMINAL;
                cmd_HALT();
            }
        }
        break;
    case RESET_DELAY_PHASE:
        if (timer_expired(data_pass_timer)) {
            TCI_sw_reset();
        }
        break;
    case HOMOLOGATION_PHASE:
        if (BMR(7) & BMR7_HOST_BYTE) {
            go_off_line();
        }
        break;
    default:
        scm_main_phase = RESET_PHASE;
        break;
    }
}
