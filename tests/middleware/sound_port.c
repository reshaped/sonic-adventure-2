/* the sound buffer's calls where the sound library fails them. mwSbOpenPort,
 * where the stream port won't open, gives back the two rings it took, each
 * once, and no other. mwSbGetPlayPos gives position 0 where the play address
 * can't be read, and mwSbGetTrnsStat a finished transfer where the block's
 * status can't be. the build fills each local without an initializer with a
 * pattern, so an unset value isn't 0 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <mw_rna.h>
#include <mw_sdbuf.h>
#include <mw_sdsb.h>
#include <sd_mw.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

struct mw_sb mwg_sb_obj[MW_SB_NUM];
struct mw_sb_vtbl mwg_sb_vtbl;

/* the rings mwSndBufCreate hands out, and the rings given back, in order */
static struct mw_snd_buf rings[MW_SB_CHAN_MAX];
static Sint32 created;
static struct mw_snd_buf *destroyed[2 * MW_SB_CHAN_MAX];
static Sint32 destroy_count;
/* the memory blocks sdMemBlkCreate hands out */
static char blocks[2 * MW_SB_CHAN_MAX];
static Sint32 made;

struct mw_snd_buf *mwSndBufCreate(void)
{
    struct mw_snd_buf *ring = &rings[created];

    ring->used = TRUE;
    ring->no = created;
    created++;
    return ring;
}

void mwSndBufDestroy(struct mw_snd_buf *buf)
{
    assert(destroy_count < (Sint32)(sizeof(destroyed) / sizeof(destroyed[0])));
    destroyed[destroy_count++] = buf;
}

Sint32 mwSndBufGetNo(struct mw_snd_buf *buf)
{
    return buf->no;
}

SDE_ERR sdSndMemAllocateBank(const SDE_DATA_TYPE data_type,
                             const Sint8 bank_num, Sint32 adr, Sint32 size)
{
    (void)data_type;
    (void)bank_num;
    (void)adr;
    (void)size;
    return SDE_ERR_NOTHING;
}

SDE_ERR sdMemBlkCreate(SDMEMBLK *handle)
{
    *handle = (SDMEMBLK)&blocks[made++];
    return SDE_ERR_NOTHING;
}

SDE_ERR sdMemBlkDestroy(SDMEMBLK handle)
{
    (void)handle;
    return SDE_ERR_NOTHING;
}

/* the stream port: no slot is free */
SDE_ERR sdPstmOpenPortMw(SDPSTM *handle, const Sint8 use_slot_num, ...)
{
    (void)handle;
    (void)use_slot_num;
    return SDE_ERR_MODULE_SLOT_NO_ENOUGH;
}

/* the open's calls past the port, which a failed port never reaches */
SDE_ERR sdPstmResetPrmMw(SDPSTM handle, const Sint8 target_slot)
{
    (void)handle;
    (void)target_slot;
    abort();
}

SDE_ERR sdPstmSetBasePrmMw(SDPSTM handle, const Sint8 target_slot,
                           const Sint8 vol, const Sint8 pan, const Sint8 fx_ch,
                           const Sint8 fx_lev, const Sint8 drct_lev)
{
    (void)handle;
    (void)target_slot;
    (void)vol;
    (void)pan;
    (void)fx_ch;
    (void)fx_lev;
    (void)drct_lev;
    abort();
}

/* the port plays, but its play address and its blocks can't be read */
SDE_ERR sdPstmGetFlg(SDPSTM handle, Sint8 ch, Uint32 *flg)
{
    (void)handle;
    (void)ch;
    *flg = SDD_PORT_FLG_PLAY;
    return SDE_ERR_NOTHING;
}

SDE_ERR sdPstmGetCurAdrMw(SDPSTM handle, const Sint8 target_slot,
                          Sint32 *cur_smp_adr)
{
    (void)handle;
    (void)target_slot;
    (void)cur_smp_adr;
    return SDE_ERR_NO_INIT;
}

SDE_ERR sdMemBlkServer(void)
{
    return SDE_ERR_NO_INIT;
}

SDE_ERR sdMemBlkGetStat(SDMEMBLK handle, SDE_MEMBLK_STAT *stat)
{
    (void)handle;
    (void)stat;
    return SDE_ERR_NO_INIT;
}

void mwRnaLockCs(Sint32 *mask)
{
    (void)mask;
}

void mwRnaUnlockCs(const Sint32 *mask)
{
    (void)mask;
}

void mwRnaErrCallFunc(const char *format, ...)
{
    (void)format;
}

#include "extracted.inc"

static void check_open(void)
{
    Sint32 i;

    assert(mwSbOpenPort(MW_SB_CHAN_MAX) == NULL);
    assert(created == MW_SB_CHAN_MAX);
    assert(destroy_count == MW_SB_CHAN_MAX);
    for (i = 0; i < MW_SB_CHAN_MAX; i++) {
        assert(destroyed[i] == &rings[i]);
    }
}

static void check_reads(void)
{
    struct mw_sb sb;

    memset(&sb, 0, sizeof(sb));
    sb.bit_per_smpl = 16;
    assert(mwSbGetPlayPos(&sb, 0) == 0);
    assert(mwSbGetTrnsStat(&sb, 0) == SDE_MEMBLK_STAT_TRANSFER_FINISHED);
}

int main(void)
{
    check_open();
    check_reads();
    return 0;
}
