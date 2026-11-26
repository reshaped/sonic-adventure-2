/* cmi: the state of the serial port between the host and the modem */
#ifndef MODEM_CMI_H
#define MODEM_CMI_H
#include <sg_xpt.h>

#include "dte_vxd.h"

/* the events the host's side raises to the modem's side in the port's
 * md_ier, a bit each. ref points at value.byte */
struct Md {
    union {
        Uint8 byte;
        struct {
            Uint8 lcr : 1; /* the port's lcr changed */
            Uint8 mcr : 1; /* the port's mcr or baud changed */
            Uint8 cmd : 1; /* command bytes wait in the port's cmd */
            Uint8 prio_sent : 1; /* the host took the priority bytes */
            Uint8 cchar : 1; /* prio_char holds a rejected control character */
            Uint8 : 1;
            Uint8 tx : 1; /* the host's side emptied a transmit buffer */
            Uint8 rx : 1; /* the host's side closed a receive buffer */
        } bits;
    } value;
    Uint8 *ref;
};

/* the port's mode, as the modem's side sets it. ref points at value.byte.
 * while command is set, the host's bytes are commands to the modem.
 * hold_out stops the host's bytes to the line, and hold_in those to the
 * host. int_off stops the host's side raising events. PacingOutQ_MoveOutQ
 * holds the host's bytes while any bit is set, unless unk_bit3 is.
 * SCC_PC_Close_TxBuffer raises no tx event while unk_bit3 is set */
struct cmi_mode {
    union {
        Uint8 byte;
        struct {
            Uint8 : 3;
            Uint8 int_off : 1;
            Uint8 unk_bit3 : 1;
            Uint8 hold_out : 1;
            Uint8 command : 1;
            Uint8 hold_in : 1;
        } bits;
    } value;
    Uint8 *ref;
};

/* the host's side's status. ref points at value.byte. idle is set and
 * cleared with COM_MCR_IDLE in the port's mcr, which marks the port idle.
 * the host's next bytes clear both */
struct SCC_Status {
    union {
        Uint8 byte;
        struct {
            Uint8 : 1;
            Uint8 idle : 1;
            Uint8 : 6;
        } bits;
    } value;
    Uint8 *ref;
};

/* point md's ref at its byte, clear the byte, and return md */
#pragma inline(Md_ct, cmi_mode_ct)
static struct Md *Md_ct(struct Md *md)
{
    md->ref = &md->value.byte;
    *md->ref = 0;
    return md;
}

/* set up mode as Md_ct sets up md */
static struct cmi_mode *cmi_mode_ct(struct cmi_mode *mode)
{
    mode->ref = &mode->value.byte;
    *mode->ref = 0;
    return mode;
}

/* one of the port's control characters, ch, and its flags, as the modem's
 * side sets them. ref_0 points at ch's byte, and ref_1 at the flags' byte. a
 * control character from the host closes the receive buffer it comes in */
struct cmi_cchar {
    union {
        struct {
            Uint8 ch;
            Uint8 flags;
        } bytes;
        struct {
            Uint16 end : 1; /* this entry and those after it are unused */
            Uint16 reject : 1; /* ch goes to prio_char, not into the buffer */
            Uint16 : 6;
            Uint16 ch : 8;
        } bits;
    } value;
    Uint8 *ref_0;
    Uint8 *ref_1;
};

/* a copy of the port's lcr. parity is a PARITY_ value, or any other value
 * for no parity */
union cmi_lcr {
    Uint8 byte;
    struct {
        Uint8 : 2;
        Uint8 parity : 3;
        Uint8 : 3;
    } bits;
};

/* the parity bit of each byte the host's side gives the modem */
#define PARITY_BIT 0x80

/* the parities of union cmi_lcr, each a rule for PARITY_BIT. PARITY_ODD and
 * PARITY_EVEN make a byte's count of 1 bits odd or even. PARITY_MARK sets the
 * bit, and PARITY_SPACE clears it */
#define PARITY_ODD 1
#define PARITY_EVEN 3
#define PARITY_MARK 5
#define PARITY_SPACE 7

/* the host's side's place in the port's buffers. rx_index is the receive
 * buffer it fills, and rx_count the bytes it has put there. tx_index is the
 * transmit buffer it takes next */
struct Buffer_Descriptor_Pointer {
    Uint8 rx_index;
    Uint8 tx_index;
    Uint16 rx_count;
    Uint16 unk_4;
    Uint8 unk_6;
    Uint8 unk_7;
};

/* the port's state, as the host's side keeps it */
struct CommonMemoryInformation {
    Uint16 unk_0;
    Uint8 unk_2;
    Uint8 unk_3;
    struct com_memory *mem; /* the memory the port's two sides share */
    Uint32 mrblr; /* the bytes a receive buffer takes */
    struct cmi_cchar cchar[COM_CCHARS];
    struct cmi_mode mode;
    /* the port's md_mask, as SCC_PC_InitCom_Memory last read it */
    struct Md md_mask;
    union cmi_lcr lcr;
    Uint8 cchar_on; /* 1 while cchar[0] is in use */
    struct Buffer_Descriptor_Pointer descriptor;
    struct Buffer_Descriptor_Pointer *bd; /* &descriptor, once constructed */
    struct SCC_Status status;
    /* requests to the pacing, which clears each once it has acted on it */
    Uint8 flush_out; /* drop the bytes queued from the host */
    Uint8 flush_in; /* drop the bytes queued for the host */
};

/* set up cmi and return it. bd points at descriptor, and each ref, ref_0
 * and ref_1 at its own field's bytes. mode is COM_MODE_COMMAND. mrblr,
 * status's byte and each cchar's bytes keep what they held. every other
 * field is 0 or null */
struct CommonMemoryInformation *
CommonMemoryInformation_ct(struct CommonMemoryInformation *cmi);
/* do nothing */
void CommonMemoryInformation_dt(struct CommonMemoryInformation *cmi);
/* call com_memory_interrupt, the modem side's handler of md_ier's events,
 * and return TRUE */
Bool CommonMemoryInformation_SendIntToDevice(
    struct CommonMemoryInformation *cmi);
/* clear every field of bd */
void Buffer_Descriptor_Pointer_Init_BD(struct Buffer_Descriptor_Pointer *bd);
/* clear the transmit side's fields of bd: tx_index, unk_4 and unk_7 */
void Buffer_Descriptor_Pointer_Init_Tx_BD(struct Buffer_Descriptor_Pointer *bd);

#endif
