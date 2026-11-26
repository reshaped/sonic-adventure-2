#include <sg_xpt.h>
#include "cmm.h"
#include "dteapi.h"
#include "dte_vxd.h"
#include "int_vect.h"

Uint8 WinQ[4];

#ifdef NONMATCHING
/* this side tests each event's bit by mask in raised, the events Comm_Int's
 * mask leaves. GCC would lay interrupt_bits out from the low bit */
#define UNMASKED(field, bit) (raised & (bit))
#else
#define UNMASKED(field, bit) (event.bits.field && !mask.bits.field)
/* the bits of the event word, and of its mask, from bit 15 down. idle lies
 * in the halfword after the word */
struct interrupt_bits {
    Uint16 : 4;
    Uint16 speed : 1;
    Uint16 reset_buffers : 1;
    Uint16 restart : 1;
    Uint16 receive_break : 1;
    Uint16 control_chars : 1;
    Uint16 receive_length : 1;
    Uint16 modem_status : 1;
    Uint16 mode : 1;
    Uint16 command_done : 1;
    Uint16 priority : 1;
    Uint16 : 2;
    Uint16 idle : 1;
};
/* @hack spelling: the word and its mask filled a byte at a time, through
 * pointers into their own bits, where shifts would do */
struct interrupt_register {
    struct interrupt_bits bits;
    Uint8 *low;
    Uint8 *high;
    Uint16 *word;
};

#pragma inline(interrupt_register_init)
static struct interrupt_register *
interrupt_register_init(struct interrupt_register *reg)
{
    reg->word = (Uint16 *)&reg->bits;
    reg->high = (Uint8 *)reg->word;
    reg->low = reg->high++;
    return reg;
}
#endif

void Comm_Int(void)
{
#ifdef NONMATCHING
    /* this side builds the event and mask words by shifts, the plain form the
     * hack tag on interrupt_register gives, and keeps in raised the events the
     * mask leaves */
    Uint16 raised;
    Uint16 masked;

    raised = glbScc_Pc->mem->event_lo;
    raised |= (Uint16)glbScc_Pc->mem->event_hi << 8;
    glbScc_Pc->mem->event_lo = 0;
    glbScc_Pc->mem->event_hi = 0;
    masked = glbScc_Pc->mem->event_mask_lo;
    masked |= (Uint16)glbScc_Pc->mem->event_mask_hi << 8;
    raised &= ~masked;
#else
    struct interrupt_register event;
    struct interrupt_register mask;

    interrupt_register_init(&event);
    interrupt_register_init(&mask);
    *event.low = glbScc_Pc->mem->event_lo;
    *event.high = glbScc_Pc->mem->event_hi;
    glbScc_Pc->mem->event_lo = 0;
    glbScc_Pc->mem->event_hi = 0;
    *mask.low = glbScc_Pc->mem->event_mask_lo;
    *mask.high = glbScc_Pc->mem->event_mask_hi;
#endif
    if (UNMASKED(mode, COM_EVENT_MODE)) {
        SCC_PC_Delta_MD(glbScc_Pc);
    }
    /* @hack redundant: the test of COM_EVENT_PRIO, whose arm is empty */
    if (UNMASKED(priority, COM_EVENT_PRIO)) {
    }
    if (UNMASKED(command_done, COM_EVENT_CMD_DONE)) {
        SCC_PC_RxCMD_CharBuff_Done(glbScc_Pc);
    }
    if (UNMASKED(modem_status, COM_EVENT_MSR)) {
        SCC_PC_Delta_MSR(glbScc_Pc);
    }
    if (UNMASKED(receive_length, COM_EVENT_MRBLR)) {
        SCC_PC_Delta_MRBLR(glbScc_Pc);
    }
    if (UNMASKED(control_chars, COM_EVENT_CCHAR)) {
        SCC_PC_Delta_CChar(glbScc_Pc);
    }
    if (UNMASKED(receive_break, COM_EVENT_BREAK)) {
        SCC_PC_PCRxBreak(glbScc_Pc);
    }
    if (UNMASKED(reset_buffers, COM_EVENT_TX_CLEAR)) {
        SCC_PC_ResetBD(glbScc_Pc);
    }
    if (UNMASKED(restart, COM_EVENT_RESTART)) {
        SCC_PC_RestartDevice(glbScc_Pc);
    }
    if (UNMASKED(speed, COM_EVENT_SPEED)) {
        SCC_PC_DeltaDeviceRate(glbScc_Pc);
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it reads no idle bit, and signals
     * no idle state */
#else
    /* @bug uninitialized: calls SCC_PC_SignalIdleState or not by stack bits
     * that nothing sets. event's and mask's idle lie past the bytes Comm_Int
     * fills, and the tests read them at 0x0caf10fc and 0x0caf1106 */
    if (event.bits.idle && !mask.bits.idle) {
        SCC_PC_SignalIdleState(glbScc_Pc);
    }
#endif
}
