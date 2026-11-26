/* the state of the serial port between the host and the modem */
#include <sg_xpt.h>

#include "cmi.h"
#include "vxd_int.h"

/* @hack spelling: an inline function that sets up a cmi_cchar, where the
 * same assignments in CommonMemoryInformation_ct would do */
#pragma inline(cmi_cchar_init)
static struct cmi_cchar *cmi_cchar_init(struct cmi_cchar *cchar)
{
    cchar->ref_0 = &cchar->value.bytes.ch;
    /* @hack spelling: ref_1 set to ref_0 and then incremented, where
     * ref_1 = &cchar->value.bytes.flags would do */
    cchar->ref_1 = cchar->ref_0;
    cchar->ref_1++;
    /* @hack redundant: the return of cchar, which no call uses */
    return cchar;
}

/* @hack spelling: cmi_status_init, an inline function that points status's
 * ref, where the assignment in CommonMemoryInformation_ct would do */
#pragma inline(cmi_status_init)
static void cmi_status_init(struct SCC_Status *status)
{
    status->ref = &status->value.byte;
}

struct CommonMemoryInformation *
CommonMemoryInformation_ct(struct CommonMemoryInformation *cmi)
{
    cmi_cchar_init(&cmi->cchar[0]);
    cmi_cchar_init(&cmi->cchar[1]);
    cmi_cchar_init(&cmi->cchar[2]);
    cmi_mode_ct(&cmi->mode);
    Md_ct(&cmi->md_mask);
    Buffer_Descriptor_Pointer_Init_BD(&cmi->descriptor);
    cmi_status_init(&cmi->status);
    cmi->bd = &cmi->descriptor;
    cmi->unk_0 = 0;
    cmi->unk_2 = 0;
    cmi->unk_3 = 0;
    cmi->mem = NULL;
    /* @hack redundant: a second clear of mode, which cmi_mode_ct cleared */
    *cmi->mode.ref = 0;
    cmi->cchar_on = 0;
    /* @hack type: command set through a bitfield, where
     * mode.value.byte |= COM_MODE_COMMAND would do */
    cmi->mode.value.bits.command = 1;
    cmi->lcr.byte = 0;
    cmi->flush_out = 0;
    cmi->flush_in = 0;
    return cmi;
}

void CommonMemoryInformation_dt(struct CommonMemoryInformation *cmi)
{
}

Bool CommonMemoryInformation_SendIntToDevice(
    struct CommonMemoryInformation *cmi)
{
    com_memory_interrupt();
    return TRUE;
}

void Buffer_Descriptor_Pointer_Init_BD(struct Buffer_Descriptor_Pointer *bd)
{
    /* @hack spelling: rx_index and tx_index cleared last, where the struct's
     * order would do */
    bd->rx_count = 0;
    bd->unk_4 = 0;
    bd->unk_6 = 0;
    bd->unk_7 = 0;
    bd->rx_index = 0;
    bd->tx_index = 0;
}

void Buffer_Descriptor_Pointer_Init_Tx_BD(struct Buffer_Descriptor_Pointer *bd)
{
    /* @hack spelling: tx_index cleared last, where the struct's order would
     * do */
    bd->unk_4 = 0;
    bd->unk_7 = 0;
    bd->tx_index = 0;
}
