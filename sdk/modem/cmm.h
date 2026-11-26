/* cmm: the host's side of the serial port, as far as the sources use it */
#ifndef MODEM_CMM_H
#define MODEM_CMM_H
#include <sg_xpt.h>

#include "cmi.h"
#include "dte_vxd.h"

/* a copy of the port's mcr, whose dtr and rts are COM_MCR_DTR and
 * COM_MCR_RTS. ref points at value.byte */
struct Mcr {
    union {
        Uint8 byte;
        struct {
            Uint8 : 6;
            Uint8 rts : 1;
            Uint8 dtr : 1;
        } bits;
    } value;
    Uint8 *ref;
};

/* set up mcr as Md_ct sets up md */
#pragma inline(Mcr_ct)
static struct Mcr *Mcr_ct(struct Mcr *mcr)
{
    mcr->ref = &mcr->value.byte;
    *mcr->ref = 0;
    return mcr;
}

/* the host's side of the port. SCC_PC_ct_void leaves cmi null and mem
 * unset, and SCC_PC_SetCmi sets both. while cmi is null, a call that reads
 * cmi does nothing. such a call returns 0 where it returns a value. a call
 * that reads mem but not cmi must follow SCC_PC_SetCmi */
struct SCC_PC {
    /* 1 while SCC_PC_Delta_MD passes on the host's last bytes, as the port
     * enters command mode */
    volatile Uint8 entering_command;
    /* the port's speed, as vxd_dte_set_interface numbers it */
    volatile Uint32 speed;
    struct CommonMemoryInformation *cmi;
    /* @hack type: a pointer to volatile, where the volatile fields of
     * struct com_memory would do */
    volatile struct com_memory *mem; /* cmi's mem */
};

/* set up scc with no cmi and the default speed, and return it */
struct SCC_PC *SCC_PC_ct_void(struct SCC_PC *scc);
/* give scc cmi, and cmi's mem */
void SCC_PC_SetCmi(struct SCC_PC *scc, struct CommonMemoryInformation *cmi);
/* reset the port, take its settings into scc's cmi, and return 0. the
 * reset leaves the default baud, DTR alone in the mcr, and the lcr and mcr
 * events raised. the call first asks the modem's side to reset, with
 * COM_MD_RESET in md_ier. a mode of COM_MODE_INT_OFF alone skips that
 * request. where md_ier stays set after it, the call stops there and
 * returns 1 */
Uint8 SCC_PC_InitCom_Memory(struct SCC_PC *scc);
/* copy up to length bytes from data into the port's receive buffers.
 * *written is set to the count taken. the copy stops at a buffer the modem's
 * side hasn't emptied.
 *
 * while cmi's cchar_on is set, a control character closes the buffer. a
 * kept one goes into the buffer first. a rejected one goes to the port's
 * prio_char instead, and raises the cchar event.
 *
 * while the mode's hold_out is set, the call takes no byte, and does as
 * SCC_PC_CloseBuffTimeOut instead */
void SCC_PC_FillSccBuffer(struct SCC_PC *scc, Uint8 *data, Uint32 length,
                          Uint32 *written);
/* give the host the port's priority bytes through dteSetRxBuffer. once
 * the host takes them all, raise the prio_sent event. the bytes it doesn't take
 * move to the front of prio, for the next call */
void SCC_PC_TxPrioCharBuff(struct SCC_PC *scc);
/* mark the port's command bytes taken */
void SCC_PC_RxCMD_CharBuff_Done(struct SCC_PC *scc);
/* pass the port's msr to the host, through dteSetModemStatusRegister */
void SCC_PC_Delta_MSR(struct SCC_PC *scc);
/* empty the port's transmit buffers, and have the pacing drop the bytes it
 * holds for the host */
void SCC_PC_ResetBD(struct SCC_PC *scc);
/* take the port's mrblr into scc's cmi */
void SCC_PC_Delta_MRBLR(struct SCC_PC *scc);
/* take the port's control characters into scc's cmi */
void SCC_PC_Delta_CChar(struct SCC_PC *scc);
/* store mcr's byte as the port's mcr */
void SCC_PC_SetMCR(struct SCC_PC *scc, struct Mcr *mcr);
/* store lcr as the port's lcr, and as cmi's */
void SCC_PC_SetLCR(struct SCC_PC *scc, Uint8 lcr);
/* close the receive buffer the host's side fills, mark the port idle, and
 * raise the mcr event */
void SCC_PC_CloseBuffTimeOut(struct SCC_PC *scc);
/* store baud as the port's baud, and raise the mcr event */
void SCC_PC_Delta_BaudRate(struct SCC_PC *scc, Uint8 baud);
/* take the port's mode into scc's cmi, act on what changed, and return 1.
 * a newly set hold_out closes the receive buffer. a newly set command first
 * passes the host's last bytes on through MainPacingFunc. cmi's mode then
 * holds COM_MODE_COMMAND and no other bit. a mode that goes from
 * nonzero to 0 marks the port idle and raises the mcr event */
Uint8 SCC_PC_Delta_MD(struct SCC_PC *scc);
/* mark the transmit buffer the host's side takes next empty, and step to
 * the one after it. the call raises the tx event, unless the mode's
 * unk_bit3 is set */
void SCC_PC_Close_TxBuffer(struct SCC_PC *scc, Uint8 unused);
/* tell the host of a break, with the port's msr */
void SCC_PC_PCRxBreak(struct SCC_PC *scc);
/* take up to COM_CMD_SIZE command bytes from the host into the port's cmd,
 * through dteGetTxBuffer. a call that takes any raises the cmd event. it
 * takes none while cmd holds bytes */
void SCC_PC_RxInCMDMode(struct SCC_PC *scc);
/* raise md's events to the modem's side, and return 1. while the mode's
 * int_off is set, it raises none */
Uint8 SCC_PC_SendInt(struct SCC_PC *scc, struct Md *md);
/* return value unchanged. the port's two sides share one byte order, so a
 * halfword between them needs no swap */
Uint16 SCC_PC_SwapByte(struct SCC_PC *scc, Uint16 value);
/* take the port's speed into scc */
void SCC_PC_DeltaDeviceRate(struct SCC_PC *scc);
/* pass dteSetModemStatusRegister a state whose every field is 0 */
void SCC_PC_RestartDevice(struct SCC_PC *scc);
/* tell the host the port has gone idle, with its msr */
void SCC_PC_SignalIdleState(struct SCC_PC *scc);

#endif
