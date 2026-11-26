#include <sg_xpt.h>
#include "anxdef.h"
#include "command.h"
#include "dce.h"
#include "dte_vxd.h"
#include "ec_estab.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "messages.h"
#include "osif.h"
#include "tci_ecdc.h"
#include "v42def.h"

void setup_timer(Uint32 *deadline, Uint32 ticks)
{
    timer_expired(0);
    *deadline = TIM_10MS + ticks;
}

char timer_expired(Uint32 deadline)
{
    TIM_10MS = osifGet10msCount();
    return deadline <= TIM_10MS;
}

void utilStartMeasurementTimer(Uint32 *start)
{
    *start = TIM_10MS;
}

Uint32 utilReadMeasurementTimer(Uint32 start)
{
    /* @hack spelling: the difference held in start, where returning it would
     * do */
    start = TIM_10MS - start;
    return start;
}

void ini_link(void)
{
    Uint8 mode;

    if (STATUS_BITS & STATUS_HANGUP) {
        STATUS_BITS &= ~STATUS_HANGUP;
        link_made();
    } else if (!(REQ_LINK & 0x717)) {
        TCI_sw_reset();
    } else if (STATUS_BITS & STATUS_LAPM_FAILED) {
        mode = OIS(0) & LINK_MODE;
        if (mode == 2 || mode == 4 || mode == 5) {
            STATUS_BITS &= ~STATUS_LAPM_FAILED;
            TCI_sw_reset();
        } else {
            configure_dce(DCE_ASYNC);
            link_made();
        }
    } else if ((REQ_LINK & REQ_OFFER_LAPM) ||
               (!(REQ_LINK & REQ_ORIGINATE) && (OIS(9) & OIS9_ANSWER_DETECT))) {
        ec_estab_phase = INITIALIZE;
        scm_main_phase = ESTABLISH_PHASE;
    } else {
        if (REQ_LINK & REQ_RAW) {
            if (OIS(6) & 0x40) {
                BMR(28) |= BMR28_RAW_SYNC;
            } else {
                BMR(28) |= BMR28_RAW_ASYNC;
            }
        }
        link_made();
    }
}

void link_made(void)
{
    BMR(29) &= ~BMR29_ESTABLISHING;
    if ((OIS(11) & OIS11_ENCRYPTION) &&
        (!(BMR(27) & BMR27_LAPM) || !(BMR(29) & BMR29_COMPRESSION) ||
         !encryption)) {
        STATUS_BITS |= STATUS_HANGUP;
    }
    BMR(27) &= ~8;
    STATUS_BITS |= STATUS_CONNECTED;
    STATUS_BITS &= ~STATUS_LINKING;
    if (!(BMR(29) & BMR29_DCE_HDLC)) {
        re_configure_dce();
    }
    REQ_LINK &= 0x71;
    scm_main_phase = DATA_PHASE;
    Last_BMR27 = BMR(27);
    Last_BMR29 = BMR(29);
    if (!(BMR(26) & 0x20)) {
        cmd_resp_to_host(ECDC_RESPONSE_CONNECTED);
    }
}

void re_configure_dce(void)
{
    if (BMR(27) & BMR27_LAPM) {
        if (crc) {
            configure_dce(DCE_HDLC32);
        } else {
            configure_dce(DCE_HDLC16);
        }
    } else if (BMR(29) & BMR29_MNP) {
        if (bit_oriented_mode) {
            configure_dce(DCE_HDLC16);
        } else {
            configure_dce(DCE_FORMAT_FE);
        }
    } else if (BMR(28) & BMR28_RAW_SYNC) {
        configure_dce(DCE_DIRECT_SYNC);
    } else if (BMR(28) & BMR28_RAW_ASYNC) {
        if (OIS(6) & 0x80) {
            configure_dce(DCE_DIRECT_2);
        } else {
            configure_dce(DCE_DIRECT_1);
        }
    } else {
        configure_dce(DCE_ASYNC);
    }
}

void configure_dce(Uint8 mode)
{
    Uint8 speed = OIS(28);
    Uint8 format;
    Uint8 flow;
    Uint8 queued_break;

    if (mode == DCE_ASYNC) {
        format = 16;
        flow = 0;
        if ((OIS(11) & OIS11_XON_XOFF) && !(BMR(29) & BMR29_ESTABLISHING)) {
            if (OIS(11) & OIS11_KEEP_XON_XOFF) {
                flow = FLOW_KEEP_XON_XOFF;
            } else {
                flow = FLOW_XON_XOFF;
            }
        }
        if (OIS(1) & OIS1_BREAK_IN_SEQUENCE) {
            queued_break = 1;
        } else {
            queued_break = 0;
        }
    } else {
        if (mode == DCE_FORMAT_FE) {
            mode = DCE_ASYNC;
            format = 0xfe;
        } else if (mode == DCE_FORMAT_FF) {
            mode = DCE_ASYNC;
            format = 0xff;
        } else {
            format = OIS(6) & 0x18;
        }
        flow = 0;
        queued_break = 0;
    }
    dce_set_interface(mode, speed, format, flow, OIS(14) & 0x7f, OIS(15) & 0x7f,
                      queued_break);
}

void re_configure_dte(void)
{
    if (!(STATUS_BITS & STATUS_COMMAND_MODE)) {
        if (BMR(28) & BMR28_RAW_SYNC) {
            configure_dte(DTE_DIRECT_SYNC);
        } else if (BMR(28) & BMR28_RAW_ASYNC) {
            if (OIS(6) & 0x80) {
                configure_dte(DTE_DIRECT_2);
            } else {
                configure_dte(DTE_DIRECT_1);
            }
        } else {
            configure_dte(DTE_ASYNC_DATA);
        }
    }
}

void configure_dte(Uint8 mode)
{
    Uint8 speed = OIS(28);
    Uint8 format;
    Uint8 flow;
    Uint8 queued_break;

    if (!speed) {
        speed = TCI_SPEED_1200;
    }
    if (mode == DTE_ASYNC_DATA) {
        if (!(OIS(25) & OIS25_FOLLOW_LINE)) {
            speed = OIS(27);
        }
        format = OIS(6) & 0x3f;
        /* @bug uninitialized: passes vxd_dte_set_interface an unset flow where
         * OIS(5) & 0xe0 is 0xa0, 0xc0 or 0xe0, which no case takes. the switch
         * leaves for those at 0x0caf0ee0, and the call takes flow at
         * 0x0caf0f6c */
        switch (OIS(5) & 0xe0) {
        case 0:
            flow = 0;
            break;
        case 0x20:
            if (OIS(11) & OIS11_KEEP_XON_XOFF) {
                flow = FLOW_KEEP_XON_XOFF;
            } else {
                flow = FLOW_XON_XOFF;
            }
            break;
        case 0x40:
            flow = 5;
            break;
        case 0x60:
            flow = 1;
            break;
        case 0x80:
            flow = 3;
            break;
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: another value takes flow 0 */
        default:
            flow = 0;
            break;
#endif
        }
        if (OIS(1) & OIS1_BREAK_IN_SEQUENCE) {
            queued_break = 1;
        } else {
            queued_break = 0;
        }
    } else {
        format = OIS(6) & 0x18;
        if (mode == DTE_HELD && (OIS(4) & 0x80)) {
            flow = 5;
        } else {
            flow = 0;
        }
        queued_break = 0;
    }
    vxd_dte_set_interface(mode, dte_speed_restriction(speed), format, flow,
                          OIS(16), OIS(17), OIS(12), OIS(13), queued_break);
}
