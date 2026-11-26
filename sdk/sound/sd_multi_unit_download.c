#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the head of a multi unit, data of SDE_DATA_TYPE_MULTI_UNIT that carries
 * banks of other types. num records of struct multi_unit_bank follow it */
struct multi_unit_head {
    SDE_DATA_TYPE type;
    Uint8 ver;
    Uint8 unk_05[3];
    Sint32 num;
    Uint8 unk_0c[20];
};
/* the version of multi unit the library takes */
#define MULTI_UNIT_VER 2
/* a bank the multi unit carries: its type, and its number among the banks of
 * that type */
struct multi_unit_bank {
    SDE_DATA_TYPE type;
    Sint32 bank;
    Sint32 adr; /* where the bank lies in sound memory */
    Sint32 size; /* its bytes, or negative to leave its mapping as it is */
    Sint32 data_ofs; /* where its data lies, in bytes from the unit's start */
    Sint32 data_size; /* the data's bytes, or 0 or less for none */
    Sint32 unk_18[2];
};

SDE_ERR sdMultiUnitDownload(SDMEMBLK handle)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct multi_unit_head *head;
    struct multi_unit_bank *bank;
    Sint32 num;
    Sint32 i;

    /* @hack redundant: head's address taken, which nothing uses */
    (void)&head;
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    head = (struct multi_unit_head *)((struct sd_memblk *)handle)->set_ptr;
    if (head == NULL || head == (struct multi_unit_head *)-1) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    if (head->type != SDE_DATA_TYPE_MULTI_UNIT) {
        result = SDE_ERR_DATA_ILLEGAL_TYPE;
        goto exit;
    }
    num = head->num;
    bank = (struct multi_unit_bank *)(head + 1);
    if (head->ver != MULTI_UNIT_VER) {
        result = SDE_ERR_DATA_ILLEGAL_VER;
        goto exit;
    }
    for (i = 0; i < num; i++, bank++) {
        if (bank->size >= 0) {
            result = sdSndMemAllocateBank(bank->type, bank->bank, bank->adr,
                                          bank->size);
            if (result != SDE_ERR_NOTHING) {
                goto exit;
            }
        }
        if (bank->data_size > 0) {
            /* @hack spelling: data and size, locals for two of
             * sdMemBlkSetPrm's arguments, where the expressions in the call
             * would do */
            void *data = (char *)head + bank->data_ofs;
            Sint32 size = bank->data_size;

            result = sdMemBlkSetPrm(sdgWrkPtr->unit_blk, data, size,
                                    SDD_MEMBLK_SYNC_FUNC, NULL);
            if (result != SDE_ERR_NOTHING) {
                goto exit;
            }
            result =
                sdBankDownload(sdgWrkPtr->unit_blk, bank->type, bank->bank);
            if (result != SDE_ERR_NOTHING) {
                goto exit;
            }
        }
    }
exit:
    /* @bug memory: reads through a null handle after refusing it with
     * SDE_ERR_HANDLE_NULL. every return passes through this read of
     * set_ptr (MOV.L @(12,R8),R3 at 0x8c12ebc2) */
    /* @hack redundant: a store to head that nothing reads */
    head = (struct multi_unit_head *)((struct sd_memblk *)handle)->set_ptr;
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMultiUnitDownload, result);
    }
    return result;
}
