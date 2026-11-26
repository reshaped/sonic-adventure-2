/* the host's side of the serial port. it fills the receive buffers, and
 * hands back the transmit buffers the pacing empties. an event of struct Md
 * tells the modem's side of each change */
#include <sg_xpt.h>
#include <string.h>

#include "cmi.h"
#include "cmm.h"
#include "dte_vxd.h"
#include "dteapi.h"
#include "pacing.h"

/* the speed SCC_PC's constructors give scc, and the baud
 * SCC_PC_InitCom_Memory gives the port */
#define SPEED_DEFAULT 10
#define BAUD_DEFAULT 10

/* what SCC_PC_Check_char finds a byte to be */
#define CHAR_PLAIN 0 /* no control character */
#define CHAR_REJECT 1 /* a control character whose reject is set */
#define CHAR_KEEP 2 /* a control character whose reject is clear */

/* an event mask byte that masks every event */
#define EVENTS_MASKED 0xff

/* @hack spelling: SCC_Status_ct, an inline function that points a status's
 * ref, where the same assignment in SCC_PC_InitCom_Memory would do */
#pragma inline(SCC_Status_ct)
static struct SCC_Status *SCC_Status_ct(struct SCC_Status *status)
{
    status->ref = &status->value.byte;
    return status;
}

/* the member exports these names, though no other member uses them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set up scc with cmi, cmi's mem and the default speed, and return it */
struct SCC_PC *SCC_PC_ct_cmi(struct SCC_PC *scc,
                             struct CommonMemoryInformation *cmi);
/* take the port's COM_CCHARS control characters into scc's cmi. cchar_on
 * becomes 1 unless cchar[0]'s end is set */
void SCC_PC_GetControlChar(struct SCC_PC *scc);
/* add length bytes of data to the receive buffer the host's side fills,
 * which must have room. the buffer closes at mrblr bytes. where the port is
 * marked idle, the call first clears the mark and raises the mcr event */
void SCC_PC_EnqueueDataInSccBuffer(struct SCC_PC *scc, Uint8 *data,
                                   Uint32 length);
/* whether the receive buffer the host's side fills is empty, as the modem's
 * side left it */
Uint8 SCC_PC_RxBuffRdy(struct SCC_PC *scc);
/* what c is among cmi's control characters, a CHAR_ value. the search
 * stops at the first entry whose end is set. c is signed, so a control
 * character of 0x80 or more never matches */
int SCC_PC_Check_char(struct SCC_PC *scc, char c);
/* hand the receive buffer the host's side fills to the modem's side, and
 * raise the rx event. the buffer takes status and its length. where cmi's
 * lcr asks for a parity, each byte first takes its parity bit. a buffer
 * that holds no byte stays open */
void SCC_PC_Close_RxBuffer(struct SCC_PC *scc, Uint8 status);
/* add md's events to the port's md_ier. while the mode's int_off is set,
 * clear md_ier instead */
void SCC_PC_SetMD_IER(struct SCC_PC *scc, struct Md *md);
Uint8 SCC_PC_ModemStatus(struct SCC_PC *scc);
/* set COM_MCR_DTR in the port's mcr to bit 0 of on, and raise the mcr
 * event. SCC_PC_SetRTS does the same for COM_MCR_RTS */
void SCC_PC_SetDTR(struct SCC_PC *scc, Uint8 on);
Uint8 SCC_PC_GetMCR(struct SCC_PC *scc);
void SCC_PC_SetRTS(struct SCC_PC *scc, Uint8 on);
/* empty the port's buffers, its command bytes and its priority bytes. cmi's
 * place in the buffers goes back to the start */
void SCC_PC_ResetDevice(struct SCC_PC *scc);
/* store status's byte as cmi's status */
void SCC_PC_SetSCCStatus(struct SCC_PC *scc, struct SCC_Status *status);
Uint8 SCC_PC_GetMode(struct SCC_PC *scc);
/* set PARITY_BIT of *c where the parity cmi's lcr asks for needs it, and
 * return *c. only PARITY_SPACE clears the bit. a parity that isn't a PARITY_
 * value leaves *c as it was */
Uint8 SCC_PC_AddParity(struct SCC_PC *scc, Uint8 *c);
/* raise every event of struct Md but rx */
void SCC_PC_Close_Device(struct SCC_PC *scc);
/* NOLINTEND(misc-use-internal-linkage) */

struct SCC_PC *SCC_PC_ct_cmi(struct SCC_PC *scc,
                             struct CommonMemoryInformation *cmi)
{
    scc->cmi = cmi;
    scc->mem = cmi->mem;
    scc->entering_command = 0;
    scc->speed = SPEED_DEFAULT;
    return scc;
}

struct SCC_PC *SCC_PC_ct_void(struct SCC_PC *scc)
{
    scc->cmi = NULL;
    scc->entering_command = 0;
    scc->speed = SPEED_DEFAULT;
    return scc;
}

void SCC_PC_SetCmi(struct SCC_PC *scc, struct CommonMemoryInformation *cmi)
{
    scc->cmi = cmi;
    scc->mem = cmi->mem;
}

Uint8 SCC_PC_InitCom_Memory(struct SCC_PC *scc)
{
    /* @hack redundant: spare, an SCC_Status the function constructs and
     * never reads */
    struct SCC_Status spare;
    struct Md md;
    struct SCC_Status status;
    int i;
    volatile struct com_memory *mem;

    SCC_Status_ct(&spare);
    Md_ct(&md);
    SCC_Status_ct(&status);
    if (scc->cmi == NULL) {
        return 0;
    }
    if (scc->mem->mode != COM_MODE_INT_OFF) {
        scc->mem->unk_47 = 0;
        scc->mem->md_ier = COM_MD_RESET;
        scc->mem->event_mask_lo = EVENTS_MASKED;
        scc->mem->event_mask_hi = EVENTS_MASKED;
        CommonMemoryInformation_SendIntToDevice(scc->cmi);
        if (scc->mem->md_ier != 0) {
            return 1;
        }
    } else {
        scc->cmi->unk_3 = 1;
    }
    SCC_PC_Delta_MRBLR(scc);
    SCC_PC_Delta_CChar(scc);
    SCC_PC_Delta_MSR(scc);
    SCC_PC_Delta_MD(scc);
    scc->mem->unk_3e = 0;
    SCC_PC_ResetDevice(scc);
    scc->mem->prio_char = 0;
    scc->mem->cmd_count = 0;
    for (i = 0; i < COM_CMD_SIZE; i++) {
        /* @hack spelling: mem, a copy of scc->mem taken at each store, where
         * scc->mem->cmd[i] would do */
        (mem = scc->mem)->cmd[i] = 0;
    }
    scc->mem->baud = BAUD_DEFAULT;
    scc->mem->mcr = COM_MCR_DTR;
    *status.ref = 0;
    SCC_PC_SetSCCStatus(scc, &status);
    md.value.bits.lcr = 1;
    md.value.bits.mcr = 1;
    SCC_PC_SetMD_IER(scc, &md);
    *scc->cmi->md_mask.ref = scc->mem->md_mask;
    if (scc->cmi->md_mask.value.bits.lcr == 0 ||
        scc->cmi->md_mask.value.bits.mcr == 0) {
        CommonMemoryInformation_SendIntToDevice(scc->cmi);
    }
    return 0;
}

void SCC_PC_GetControlChar(struct SCC_PC *scc)
{
    Uint8 *from = (Uint8 *)scc->mem->cchar.byte;
    int i;

    if (scc->cmi == NULL) {
        return;
    }
    /* @hack spelling: a loop whose switch takes cchar[0], cchar[1] and
     * cchar[2] in turn, where its three arms in a row would do */
    for (i = 0; i < COM_CCHARS; i++) {
        switch (i) {
        case 0:
            *scc->cmi->cchar[0].ref_1 = *from++;
            scc->cmi->cchar[0].value.bits.ch = *from++;
            scc->cmi->cchar_on = !scc->cmi->cchar[0].value.bits.end;
            break;
        case 1:
            *scc->cmi->cchar[1].ref_1 = *from++;
            scc->cmi->cchar[1].value.bits.ch = *from++;
            break;
        case 2:
            *scc->cmi->cchar[2].ref_1 = *from++;
            scc->cmi->cchar[2].value.bits.ch = *from++;
            break;
        }
    }
}

void SCC_PC_EnqueueDataInSccBuffer(struct SCC_PC *scc, Uint8 *data,
                                   Uint32 length)
{
    Uint8 *to;

    if (scc->cmi == NULL) {
        return;
    }
    to = (Uint8 *)scc->mem->rx[scc->cmi->bd->rx_index].data;
    to += scc->cmi->bd->rx_count;
    if (scc->cmi->status.value.bits.idle == 1) {
        struct Md md;

        Md_ct(&md);
        scc->mem->mcr &= (Uint8)~COM_MCR_IDLE;
        scc->cmi->status.value.bits.idle = 0;
        md.value.bits.mcr = 1;
        SCC_PC_SendInt(scc, &md);
    }
    memcpy(to, data, length);
    /* @hack type: length cast to Uint16, where the conversion += makes would
     * do */
    scc->cmi->bd->rx_count += (Uint16)length;
    if (scc->cmi->bd->rx_count == scc->cmi->mrblr) {
        SCC_PC_Close_RxBuffer(scc, 0);
    }
}

void SCC_PC_FillSccBuffer(struct SCC_PC *scc, Uint8 *data, Uint32 length,
                          Uint32 *written)
{
    Uint32 room;
    int kind;

    *written = 0;
    if (scc->cmi == NULL) {
        return;
    }
    if (scc->cmi->mode.value.bits.hold_out) {
        SCC_PC_CloseBuffTimeOut(scc);
        return;
    }
    /* @bug uninitialized: the switch below reads kind unset where room is 0
     * on the first pass, as a mrblr of 0 makes it. MOV.L @R15,R0 at
     * 0x0cae1fc0 reads the slot that only MOV.L R0,@R15 at 0x0cae1fae
     * writes */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: kind starts at CHAR_PLAIN, as a
     * scan that meets no control character leaves it. the image reads kind's
     * stack word as an earlier call left it, which C can't give */
    kind = CHAR_PLAIN;
#endif
    while (*written < length) {
        if (!SCC_PC_RxBuffRdy(scc)) {
            return;
        }
        room = scc->cmi->mrblr - scc->cmi->bd->rx_count;
        if (room > length - *written) {
            room = length - *written;
        }
        if (scc->cmi->cchar_on) {
            Uint32 i = 0;
            struct Md md;

            Md_ct(&md);
            /* @hack spelling: the assignment to kind inside the loop's test,
             * where a statement of its own would do */
            while (i < room &&
                   (kind = SCC_PC_Check_char(scc, data[i])) == CHAR_PLAIN) {
                i++;
            }
            SCC_PC_EnqueueDataInSccBuffer(scc, data, i);
            *written += i;
            data += i;
            switch (kind) {
            case CHAR_PLAIN:
                break;
            case CHAR_REJECT:
                scc->mem->prio_char = *data++;
                (*written)++;
                SCC_PC_Close_RxBuffer(scc, 0);
                md.value.bits.cchar = 1;
                SCC_PC_SendInt(scc, &md);
                break;
            case CHAR_KEEP:
                SCC_PC_EnqueueDataInSccBuffer(scc, data, 1);
                data++;
                (*written)++;
                SCC_PC_Close_RxBuffer(scc, COM_STATUS_CCHAR);
                break;
            }
        } else {
            SCC_PC_EnqueueDataInSccBuffer(scc, data, room);
            data += room;
            *written += room;
        }
    }
}

Uint8 SCC_PC_RxBuffRdy(struct SCC_PC *scc)
{
    if (scc->cmi == NULL) {
        return 0;
    }
    return scc->mem->rx[scc->cmi->bd->rx_index].status.bit.empty;
}

int SCC_PC_Check_char(struct SCC_PC *scc, char c)
{
    int kind = CHAR_PLAIN;
    int i;

    if (scc->cmi == NULL) {
        return CHAR_PLAIN;
    }
    /* @hack spelling: a loop with an arm for each value of i, where cchar[i]
     * in one arm would do */
    for (i = 0; i < COM_CCHARS; i++) {
        if (i == 0) {
            if (scc->cmi->cchar[0].value.bits.end) {
                kind = CHAR_PLAIN;
                break;
            }
            if (c == scc->cmi->cchar[0].value.bits.ch) {
                if (scc->cmi->cchar[0].value.bits.reject) {
                    kind = CHAR_REJECT;
                } else {
                    kind = CHAR_KEEP;
                }
                break;
            }
        } else if (i == 1) {
            if (scc->cmi->cchar[1].value.bits.end) {
                kind = CHAR_PLAIN;
                break;
            }
            if (c == scc->cmi->cchar[1].value.bits.ch) {
                if (scc->cmi->cchar[1].value.bits.reject) {
                    kind = CHAR_REJECT;
                } else {
                    kind = CHAR_KEEP;
                }
                break;
            }
        } else if (i == 2) {
            if (scc->cmi->cchar[2].value.bits.end) {
                kind = CHAR_PLAIN;
                break;
            }
            if (c == scc->cmi->cchar[2].value.bits.ch) {
                if (scc->cmi->cchar[2].value.bits.reject) {
                    kind = CHAR_REJECT;
                } else {
                    kind = CHAR_KEEP;
                }
                break;
            }
            /* @hack redundant: a break that ends the loop, which ends here
             * anyway */
            break;
        }
    }
    return kind;
}

void SCC_PC_Close_RxBuffer(struct SCC_PC *scc, Uint8 status)
{
    struct Md md;
    Uint16 i;
    Uint8 *data;
    union cmi_lcr *lcr;

    Md_ct(&md);
    if (scc->cmi == NULL) {
        return;
    }
    if (scc->cmi->bd->rx_count == 0) {
        return;
    }
    /* @hack spelling: lcr, a pointer to scc->cmi->lcr, where scc->cmi->lcr
     * itself would do */
    lcr = &scc->cmi->lcr;
    if (lcr->bits.parity) {
        data = (Uint8 *)scc->mem->rx[scc->cmi->bd->rx_index].data;
        for (i = 0; i < scc->cmi->bd->rx_count; i++) {
            SCC_PC_AddParity(scc, &data[i]);
        }
    }
    scc->mem->rx[scc->cmi->bd->rx_index].status.all = status;
    scc->mem->rx[scc->cmi->bd->rx_index].length =
        SCC_PC_SwapByte(scc, scc->cmi->bd->rx_count);
    scc->cmi->bd->rx_index++;
    if (scc->cmi->bd->rx_index == COM_BUFFERS) {
        scc->cmi->bd->rx_index = 0;
    }
    scc->cmi->bd->rx_count = 0;
    scc->mem->rx_index = scc->cmi->bd->rx_index;
    md.value.bits.rx = 1;
    SCC_PC_SendInt(scc, &md);
}

void SCC_PC_TxPrioCharBuff(struct SCC_PC *scc)
{
    /* the priority bytes, copied for the host */
    static Uint8 prio[COM_PRIO_SIZE];
    struct Md md;
    struct ddalDataTransfer transfer;
    struct ddal_result *result;

    Md_ct(&md);
    if (scc->mem->prio_count == 0) {
        return;
    }
    memcpy(prio, (Uint8 *)scc->mem->prio, scc->mem->prio_count);
    transfer.data = prio;
    transfer.length = scc->mem->prio_count;
    result = dteSetRxBuffer(&transfer);
    if (result->length != scc->mem->prio_count) {
        if (result->length != 0) {
            memcpy((Uint8 *)scc->mem->prio,
                   (Uint8 *)scc->mem->prio + result->length,
                   scc->mem->prio_count - result->length);
            scc->mem->prio_count -= result->length;
        }
    } else {
        md.value.bits.prio_sent = 1;
        SCC_PC_SendInt(scc, &md);
    }
}

void SCC_PC_RxCMD_CharBuff_Done(struct SCC_PC *scc)
{
    scc->mem->cmd_count = 0;
}

void SCC_PC_Delta_MSR(struct SCC_PC *scc)
{
    static struct dteModemStatusRegister status;
    static Uint32 made;

    /* @hack redundant: made, a static the first call sets to 1, which
     * nothing reads */
    made ? (void)0 : (void)made++;
    status.msr = SCC_PC_ModemStatus(scc);
    status.rx_break = 0;
    status.unk_2 = 1;
    dteSetModemStatusRegister(&status);
}

void SCC_PC_ResetBD(struct SCC_PC *scc)
{
    int i;

    if (scc->cmi == NULL) {
        return;
    }
    scc->mem->tx_index = 0;
    scc->cmi->bd->tx_index = 0;
    scc->cmi->bd->unk_7 = 0;
    for (i = 0; i < COM_BUFFERS; i++) {
        scc->mem->tx[i].length = 0;
        scc->mem->tx[i].status.bit.empty = 1;
    }
    scc->cmi->flush_in = 1;
    Buffer_Descriptor_Pointer_Init_Tx_BD(scc->cmi->bd);
}

void SCC_PC_Delta_MRBLR(struct SCC_PC *scc)
{
    if (scc->cmi == NULL) {
        return;
    }
    scc->cmi->mrblr = SCC_PC_SwapByte(scc, scc->mem->mrblr);
}

void SCC_PC_Delta_CChar(struct SCC_PC *scc)
{
    SCC_PC_GetControlChar(scc);
}

void SCC_PC_SetMD_IER(struct SCC_PC *scc, struct Md *md)
{
    if (scc->cmi == NULL) {
        return;
    }
    if (scc->cmi->mode.value.bits.int_off) {
        scc->mem->md_ier = 0;
        return;
    }
    scc->mem->md_ier |= *md->ref;
}

Uint8 SCC_PC_ModemStatus(struct SCC_PC *scc)
{
    return scc->mem->msr;
}

void SCC_PC_SetDTR(struct SCC_PC *scc, Uint8 on)
{
    struct Mcr mcr;
    struct Md md;

    Mcr_ct(&mcr);
    Md_ct(&md);
    *mcr.ref = SCC_PC_GetMCR(scc);
    mcr.value.bits.dtr = on;
    SCC_PC_SetMCR(scc, &mcr);
    md.value.bits.mcr = 1;
    SCC_PC_SendInt(scc, &md);
}

Uint8 SCC_PC_GetMCR(struct SCC_PC *scc)
{
    return scc->mem->mcr;
}

void SCC_PC_SetMCR(struct SCC_PC *scc, struct Mcr *mcr)
{
    scc->mem->mcr = *mcr->ref;
}

void SCC_PC_SetRTS(struct SCC_PC *scc, Uint8 on)
{
    struct Mcr mcr;
    struct Md md;

    Mcr_ct(&mcr);
    Md_ct(&md);
    *mcr.ref = SCC_PC_GetMCR(scc);
    mcr.value.bits.rts = on;
    SCC_PC_SetMCR(scc, &mcr);
    md.value.bits.mcr = 1;
    SCC_PC_SendInt(scc, &md);
}

void SCC_PC_ResetDevice(struct SCC_PC *scc)
{
    int i;

    if (scc->cmi == NULL) {
        return;
    }
    scc->mem->cmd_count = 0;
    scc->mem->rx_index = 0;
    scc->cmi->bd->rx_index = 0;
    scc->cmi->bd->unk_6 = 0;
    for (i = 0; i < COM_BUFFERS; i++) {
        scc->mem->rx[i].length = 0;
        scc->mem->rx[i].status.bit.empty = 1;
    }
    scc->mem->prio_count = 0;
    scc->mem->tx_index = 0;
    scc->cmi->bd->tx_index = 0;
    scc->cmi->bd->unk_7 = 0;
    for (i = 0; i < COM_BUFFERS; i++) {
        scc->mem->tx[i].length = 0;
        scc->mem->tx[i].status.bit.empty = 1;
    }
    Buffer_Descriptor_Pointer_Init_BD(scc->cmi->bd);
}

void SCC_PC_SetSCCStatus(struct SCC_PC *scc, struct SCC_Status *status)
{
    if (scc->cmi == NULL) {
        return;
    }
    *scc->cmi->status.ref = *status->ref;
}

void SCC_PC_SetLCR(struct SCC_PC *scc, Uint8 lcr)
{
    if (scc->cmi == NULL) {
        return;
    }
    scc->mem->lcr = lcr;
    scc->cmi->lcr.byte = lcr;
}

void SCC_PC_CloseBuffTimeOut(struct SCC_PC *scc)
{
    struct Md md;

    if (scc->cmi == NULL) {
        return;
    }
    Md_ct(&md);
    SCC_PC_Close_RxBuffer(scc, 0);
    scc->mem->mcr |= COM_MCR_IDLE;
    scc->cmi->status.value.bits.idle = 1;
    md.value.bits.mcr = 1;
    SCC_PC_SendInt(scc, &md);
}

void SCC_PC_Delta_BaudRate(struct SCC_PC *scc, Uint8 baud)
{
    struct Md md;

    if (scc->cmi == NULL) {
        return;
    }
    Md_ct(&md);
    scc->mem->baud = baud;
    md.value.bits.mcr = 1;
    SCC_PC_SetMD_IER(scc, &md);
    CommonMemoryInformation_SendIntToDevice(scc->cmi);
}

Uint8 SCC_PC_Delta_MD(struct SCC_PC *scc)
{
    struct cmi_mode old;

    if (scc->cmi == NULL) {
        return 0;
    }
    cmi_mode_ct(&old);
    *old.ref = *scc->cmi->mode.ref;
    *scc->cmi->mode.ref = SCC_PC_GetMode(scc);
    /* @hack spelling: hold_out and command each tested for a change with ^,
     * and then for being set, where a test that old is clear and new is set
     * would do */
    if (old.value.bits.hold_out ^ scc->cmi->mode.value.bits.hold_out) {
        if (scc->cmi->mode.value.bits.hold_out) {
            SCC_PC_Close_RxBuffer(scc, 0);
        }
    }
    if (old.value.bits.command ^ scc->cmi->mode.value.bits.command) {
        if (scc->cmi->mode.value.bits.command) {
            scc->entering_command = 1;
            *scc->cmi->mode.ref = 0;
            if (scc->entering_command) {
                MainPacingFunc();
            }
            scc->cmi->mode.value.bits.command = 1;
            scc->entering_command = 0;
        }
    }
    if (*old.ref != 0 && *scc->cmi->mode.ref == 0) {
        struct Md md;

        Md_ct(&md);
        scc->mem->mcr |= COM_MCR_IDLE;
        scc->cmi->status.value.bits.idle = 1;
        md.value.bits.mcr = 1;
        SCC_PC_SendInt(scc, &md);
    }
    return 1;
}

Uint8 SCC_PC_GetMode(struct SCC_PC *scc)
{
    return scc->mem->mode;
}

void SCC_PC_Close_TxBuffer(struct SCC_PC *scc, Uint8 unused)
{
    struct Md md;

    Md_ct(&md);
    if (scc->cmi == NULL) {
        return;
    }
    scc->mem->tx[scc->cmi->bd->tx_index].status.bit.empty = 1;
    scc->cmi->bd->tx_index++;
    if (scc->cmi->bd->tx_index == COM_BUFFERS) {
        scc->cmi->bd->tx_index = 0;
    }
    scc->cmi->bd->unk_4 = 0;
    scc->mem->tx_index = scc->cmi->bd->tx_index;
    if (!scc->cmi->mode.value.bits.unk_bit3) {
        /* @hack redundant: a second clear of md, which Md_ct cleared */
        *md.ref = 0;
        md.value.bits.tx = 1;
        SCC_PC_SendInt(scc, &md);
    }
}

void SCC_PC_PCRxBreak(struct SCC_PC *scc)
{
    static struct dteModemStatusRegister status;
    static Uint32 made;

    /* @hack redundant: made, a static the first call sets to 1, which
     * nothing reads */
    made ? (void)0 : (void)made++;
    status.msr = SCC_PC_ModemStatus(scc);
    status.rx_break = 1;
    status.unk_2 = 1;
    dteSetModemStatusRegister(&status);
}

void SCC_PC_RxInCMDMode(struct SCC_PC *scc)
{
    struct Md md;
    struct ddalDataTransfer transfer;
    struct ddal_result *result;

    Md_ct(&md);
    if (scc->mem->cmd_count != 0) {
        return;
    }
    transfer.data = (Uint8 *)scc->mem->cmd;
    transfer.length = COM_CMD_SIZE;
    result = dteGetTxBuffer(&transfer);
    if (result->length == 0) {
        return;
    }
    scc->mem->cmd_count = result->length;
    md.value.bits.cmd = 1;
    SCC_PC_SendInt(scc, &md);
}

Uint8 SCC_PC_SendInt(struct SCC_PC *scc, struct Md *md)
{
    if (scc->cmi == NULL) {
        return 0;
    }
    /* @hack spelling: the else after a return, where the statements after the
     * if would do */
    if (scc->cmi->mode.value.bits.int_off) {
        return 1;
    } else {
        SCC_PC_SetMD_IER(scc, md);
        CommonMemoryInformation_SendIntToDevice(scc->cmi);
    }
    return 1;
}

Uint8 SCC_PC_AddParity(struct SCC_PC *scc, Uint8 *c)
{
    Uint8 ones = 0;
    union cmi_lcr *lcr;

    if (scc->cmi == NULL) {
        return 0;
    }
    if (*c & 0x01) {
        ones++;
    }
    if (*c & 0x02) {
        ones++;
    }
    if (*c & 0x04) {
        ones++;
    }
    if (*c & 0x08) {
        ones++;
    }
    if (*c & 0x10) {
        ones++;
    }
    if (*c & 0x20) {
        ones++;
    }
    if (*c & 0x40) {
        ones++;
    }
    ones &= 1;
    /* @hack spelling: lcr, a pointer to scc->cmi->lcr, where scc->cmi->lcr
     * itself would do */
    lcr = &scc->cmi->lcr;
    /* @bug logic: under PARITY_ODD and PARITY_EVEN, a byte whose bit 7 is set
     * keeps it where the parity needs a 0, so the byte goes out with the
     * wrong parity. both arms only set the bit, with OR #128 at 0x0cae29a0,
     * while PARITY_SPACE clears it with AND #127 at 0x0cae29ae */
    switch (lcr->bits.parity) {
    case PARITY_ODD:
        if (ones == 0) {
            *c |= PARITY_BIT;
        }
        break;
    case PARITY_EVEN:
        if (ones != 0) {
            *c |= PARITY_BIT;
        }
        break;
    case PARITY_MARK:
        *c |= PARITY_BIT;
        break;
    case PARITY_SPACE:
        *c &= (Uint8)~PARITY_BIT;
        break;
    }
    return *c;
}

Uint16 SCC_PC_SwapByte(struct SCC_PC *scc, Uint16 value)
{
    return value;
}

void SCC_PC_Close_Device(struct SCC_PC *scc)
{
    struct Md md;

    Md_ct(&md);
    *md.ref = (Uint8)~COM_MD_RX;
    SCC_PC_SendInt(scc, &md);
}

void SCC_PC_DeltaDeviceRate(struct SCC_PC *scc)
{
    scc->speed = scc->mem->speed;
}

void SCC_PC_RestartDevice(struct SCC_PC *scc)
{
    static struct dteModemStatusRegister status;
    static Uint32 made;

    /* @hack redundant: made, a static the first call sets to 1, which
     * nothing reads */
    made ? (void)0 : (void)made++;
    status.unk_2 = 0;
    dteSetModemStatusRegister(&status);
}

void SCC_PC_SignalIdleState(struct SCC_PC *scc)
{
    static struct dteModemStatusRegister status;
    static Uint32 made;

    /* @hack redundant: made, a static the first call sets to 1, which
     * nothing reads */
    made ? (void)0 : (void)made++;
    status.msr = SCC_PC_ModemStatus(scc);
    status.rx_break = 0;
    status.idle = 1;
    status.unk_2 = 1;
    dteSetModemStatusRegister(&status);
}
