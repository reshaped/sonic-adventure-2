/* under LAPM or MNP, TCI_dte_brk_rx queues a break from the host in
 * TxBreakLength. TCI_dce_brk_rx then drops one from the line. otherwise each
 * crosses to the other side as a break */
#include <sg_xpt.h>
#include "command.h"
#include "dce.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "tci_ecdc.h"
#include "v42def.h"

/* in 10 ms ticks */
#define BREAK_HANGUP_TICKS 160U
#define SECOND_TICKS 100
#define MINUTE_SECONDS 60

#pragma inline(clear_link_buffers)
static void clear_link_buffers(void)
{
    vxd_dte_clr_rx_buff();
    vxd_dte_clr_tx_buff();
    dce_clr_rx_buff();
    dce_clr_tx_buff();
}

void TCI_dce_brk_rx(Uint16 length)
{
    if (!(BMR(27) & BMR27_LAPM) && !(BMR(29) & (BMR29_MNP | 0x10)) &&
        !(BMR(28) & 0x80)) {
        cmd_resp_to_host(ECDC_RESPONSE_DCE_BREAK);
        if (!(OIS(1) & OIS1_BREAK_TYPE)) {
            clear_link_buffers();
        }
        if (length >= BREAK_HANGUP_TICKS &&
            (OIS(25) & OIS25_LONG_SPACE_DISCONNECT)) {
            cmd_HALT();
        } else {
            if (!(OIS(1) & OIS1_KEEP_LENGTH)) {
                length = BREAK_DEFAULT_TICKS;
            }
            vxd_dte_tx_brk(length, (OIS(1) & OIS1_BREAK_IN_SEQUENCE) ? 1 : 0);
        }
    }
}

void TCI_dte_brk_rx(Uint16 length)
{
    Uint8 slot;

    if (!(OIS(1) & OIS1_SEND_BREAK)) {
        cmd_resp_to_host(ECDC_RESPONSE_DTE_BREAK);
        return;
    }
    if (!(OIS(1) & OIS1_KEEP_LENGTH)) {
        length = BREAK_DEFAULT_TICKS;
    }
    if (!(BMR(27) & BMR27_LAPM) && !(BMR(29) & BMR29_MNP) &&
        !(BMR(28) & 0x80)) {
        if (!(OIS(1) & OIS1_BREAK_TYPE)) {
            clear_link_buffers();
        }
        dce_tx_brk(length, (OIS(1) & OIS1_BREAK_IN_SEQUENCE) ? 1 : 0);
    } else {
        for (slot = 0; TxBreakLength[slot]; slot++) {
            if (slot == LAPM_BREAKS - 1) {
                return;
            }
        }
        TxBreakLength[slot] = length;
        BMR(25) |= BMR25_BREAK_PENDING;
        /* @hack redundant: a test of BMRregisters that never runs */
        if (0) {
            if ((Uint32)BMRregisters != 0) {
            }
        }
    }
}

Uint16 TCI_get_dte_brk(void)
{
    Uint16 length = TxBreakLength[0];

    TxBreakLength[0] = TxBreakLength[1];
    TxBreakLength[1] = TxBreakLength[2];
    TxBreakLength[2] = TxBreakLength[3];
    TxBreakLength[3] = 0;
    if (!TxBreakLength[0]) {
        BMR(25) &= ~BMR25_BREAK_PENDING;
    }
    return length;
}

void TCI_dte_esc_seq_found(void)
{
    cmd_resp_to_host(ECDC_RESPONSE_ESCAPE);
}

void TCI_sw_reset(void)
{
    Uint8 *status;

    REQ_LINK = 0;
    /* @hack spelling: STATUS_BITS written through a pointer, where an
     * assignment would do */
    status = &STATUS_BITS;
    *status = STATUS_COMMAND_MODE;
    BMR(28) &= 0x10;
    BMR(27) = 0;
    BMR(29) = 0;
    configure_dte(DTE_HELD);
    configure_dce(DCE_HELD);
    scm_main_phase = IDLE_PHASE;
    clear_link_buffers();
    cmd_resp_to_host(ECDC_RESPONSE_RESET);
}

void TCI_timer_10ms(void)
{
    int minutes;

    if (++prsc_1 >= SECOND_TICKS) {
        prsc_1 = 0;
        if (STATUS_BITS & STATUS_COMMAND_MODE) {
            DTE_INAC_TIM = 0;
        } else {
            DTE_INAC_TIM++;
            minutes = OIS(4) & ~0x80;
            if (minutes && minutes * MINUTE_SECONDS <= DTE_INAC_TIM) {
                STATUS_BITS |= STATUS_INACTIVE;
            }
        }
        DCE_INAC_TIM++;
        minutes = OIS(3) & OIS3_LINE_IDLE;
        if (minutes && minutes * MINUTE_SECONDS <= DCE_INAC_TIM) {
            STATUS_BITS |= STATUS_INACTIVE;
        }
    }
}
