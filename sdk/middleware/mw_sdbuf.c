/* the middleware's ring buffers */
#include <mw_sdbuf.h>
#include <sg_xpt.h>
#include <string.h>

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mwSndBufSetNo(struct mw_snd_buf *buf, Sint32 no);

struct mw_snd_buf mwg_snd_buf[MW_SND_BUF_NUM];
/* NOLINTEND(misc-use-internal-linkage) */

void mwSndBufInit(void)
{
    Sint32 i;

    memset(mwg_snd_buf, 0, sizeof(mwg_snd_buf));
    for (i = 0; i < MW_SND_BUF_NUM; i++) {
        mwg_snd_buf[i].no = MW_SND_BUF_NO_FIRST - i;
    }
}

struct mw_snd_buf *mwSndBufCreate(void)
{
    Sint32 i;
    /* @hack spelling: found, a result the loop breaks out with, where the
     * buffer returned from the loop would do */
    struct mw_snd_buf *found = NULL;

    for (i = 0; i < MW_SND_BUF_NUM; i++) {
        if (mwg_snd_buf[i].used == FALSE) {
            found = &mwg_snd_buf[i];
            found->used = TRUE;
            break;
        }
    }
    return found;
}

void mwSndBufDestroy(struct mw_snd_buf *buf)
{
    buf->used = FALSE;
}

Sint32 mwSndBufGetNo(struct mw_snd_buf *buf)
{
    return buf->no;
}

void mwSndBufSetNo(struct mw_snd_buf *buf, Sint32 no)
{
    buf->no = no;
}
