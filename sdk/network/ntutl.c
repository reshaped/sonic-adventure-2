/* the Network SDK's utilities, whose record survives a jump to another
 * program and back */
#include <nt_utl.h>
#include <sg_xpt.h>
#include <string.h>

#define UTL_BUFFER_BYTES 64

/* the record at UTL_ADDRESS, below the program. each _size field counts the
 * bytes its buffer holds */
struct utl_record {
    Uint32 magic; /* UTL_MAGIC once the record is started */
    Uint32 flag; /* the NTUTL_ bits */
    Sint32 jump_number;
    Uint8 fix_size;
    Uint8 up_size;
    Uint8 down_size;
    Uint8 unk_0f;
    Uint8 fix[UTL_BUFFER_BYTES];
    Uint8 up[UTL_BUFFER_BYTES];
    Uint8 down[UTL_BUFFER_BYTES];
};
#define UTL_ADDRESS ((struct utl_record *)0x8c008200)
/* the bytes "AGES", read as a little-endian word */
#define UTL_MAGIC 0x53454741

static const char *const utl_build =
    "\nntUtl Ver 3.02 Build:Jan 15 2001 11:13:10\n";

static struct utl_record *utl_record;

/* point utl_record at UTL_ADDRESS, and start the record there where its
 * magic is missing. a record that holds its magic keeps what it holds */
static void utl_open(void)
{
    utl_record = UTL_ADDRESS;
    if (utl_record->magic != UTL_MAGIC) {
        utl_record->magic = UTL_MAGIC;
        utl_record->flag = NTUTL_FLAG_INIT;
    }
}

void ntUtlInit(void)
{
    utl_open();
}

void ntUtlExit(void)
{
}

Sint16 ntUtlSetFlag(Uint32 flag)
{
    utl_open();
    utl_record->flag = flag;
    return NTD_OK;
}

Sint16 ntUtlGetFlag(Uint32 *flag)
{
    utl_open();
    *flag = utl_record->flag;
    return NTD_OK;
}

Sint16 ntUtlSetJumpNumber(Sint32 number)
{
    utl_open();
    utl_record->jump_number = number;
    utl_record->flag |= NTUTL_JUMP;
    return NTD_OK;
}

Sint16 ntUtlGetJumpNumber(Sint32 *number)
{
    utl_open();
    if ((utl_record->flag & NTUTL_JUMP) != NTUTL_JUMP) {
        return NTD_ERR_NOINFO;
    }
    *number = utl_record->jump_number;
    return NTD_OK;
}

Sint16 ntUtlSetFix(Uint8 *fixdata, Sint32 size)
{
    utl_open();
    memcpy(utl_record->fix, fixdata, size);
    utl_record->fix_size = size;
    utl_record->flag |= NTUTL_FIX;
    return NTD_OK;
}

Sint16 ntUtlGetFix(Uint8 *fixdata, Sint32 *size)
{
    utl_open();
    if ((utl_record->flag & NTUTL_FIX) != NTUTL_FIX) {
        *size = 0;
        return NTD_ERR_NOINFO;
    }
    if (*size > utl_record->fix_size) {
        *size = utl_record->fix_size;
    }
    memcpy(fixdata, utl_record->fix, *size);
    return NTD_OK;
}

Sint16 ntUtlSetUpload(Uint8 *updata, Sint32 size)
{
    utl_open();
    memcpy(utl_record->up, updata, size);
    utl_record->up_size = size;
    utl_record->flag |= NTUTL_UP | NTUTL_UP_OK;
    return NTD_OK;
}

Sint16 ntUtlGetUpload(Uint8 *updata, Sint32 *size)
{
    utl_open();
    if ((utl_record->flag & NTUTL_UP) != NTUTL_UP) {
        *size = 0;
        return NTD_ERR_NOINFO;
    }
    if (*size > utl_record->up_size) {
        *size = utl_record->up_size;
    }
    memcpy(updata, utl_record->up, *size);
    return NTD_OK;
}

Sint16 ntUtlSetDownload(Uint8 *downdata, Sint32 size)
{
    utl_open();
    memcpy(utl_record->down, downdata, size);
    utl_record->down_size = size;
    utl_record->flag |= NTUTL_DOWN;
    return NTD_OK;
}

Sint16 ntUtlGetDownload(Uint8 *downdata, Sint32 *size)
{
    utl_open();
    if ((utl_record->flag & NTUTL_DOWN) != NTUTL_DOWN) {
        *size = 0;
        return NTD_ERR_NOINFO;
    }
    if (*size > utl_record->down_size) {
        *size = utl_record->down_size;
    }
    memcpy(downdata, utl_record->down, *size);
    return NTD_OK;
}

Sint32 ntUtlIsDownload(void)
{
    utl_open();
    if ((utl_record->flag & NTUTL_DOWN_DO) == NTUTL_DOWN_DO) {
        return NTD_OK;
    }
    return NTD_ERR_NOINFO;
}

Sint32 ntUtlIsUpload(void)
{
    utl_open();
    if ((utl_record->flag & NTUTL_UP_DO) == NTUTL_UP_DO) {
        return NTD_OK;
    }
    return NTD_ERR_NOINFO;
}

Sint16 ntUtlSetDownFlag(Bool flag)
{
    utl_open();
    if (flag) {
        utl_record->flag |= NTUTL_DOWN_OK;
    } else {
        utl_record->flag &= ~NTUTL_DOWN_OK;
    }
    return NTD_OK;
}

Sint16 ntUtlSetUpFlag(Bool flag)
{
    utl_open();
    if (flag) {
        utl_record->flag |= NTUTL_UP_OK;
    } else {
        utl_record->flag &= ~NTUTL_UP_OK;
    }
    return NTD_OK;
}

Sint16 ntUtlSetVirtualVM(Bool flag)
{
    utl_open();
    if (flag) {
        utl_record->flag |= NTUTL_VIRTUALVM;
    } else {
        utl_record->flag &= ~NTUTL_VIRTUALVM;
    }
    return NTD_OK;
}

Sint32 ntUtlIsVirtualVM(void)
{
    utl_open();
    if ((utl_record->flag & NTUTL_VIRTUALVM) == NTUTL_VIRTUALVM) {
        return NTD_OK;
    }
    return NTD_ERR_NOINFO;
}
