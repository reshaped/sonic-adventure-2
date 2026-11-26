/* the ring buffers the middleware's stream sound plays from, as bank
 * numbers. a bank is a region of sound memory, which the sound library maps
 * by kind and number. mw_sdsb.c maps these as SDE_DATA_TYPE_PSTM_RING_BUF */
#ifndef MW_SDBUF_H
#define MW_SDBUF_H
#include <sg_xpt.h>

/* how many ring buffers the middleware has */
#define MW_SND_BUF_NUM 8
/* the first ring's bank number. each ring after it has a number one
 * lower */
#define MW_SND_BUF_NO_FIRST 15

/* a ring buffer. no is its bank number, and used is TRUE while it is
 * taken */
struct mw_snd_buf {
    Bool used;
    Sint32 no;
};

/* mark every ring buffer free, and give each its bank number */
void mwSndBufInit(void);
/* take the first free ring buffer. it returns null where none is free */
struct mw_snd_buf *mwSndBufCreate(void);
/* give buf back */
void mwSndBufDestroy(struct mw_snd_buf *buf);
/* buf's bank number */
Sint32 mwSndBufGetNo(struct mw_snd_buf *buf);

#endif
