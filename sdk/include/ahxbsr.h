/* CRI's AHX bit reader, a pool of eight. a reader takes its joint's data a
 * chunk at a time, and gives its bits most significant first */
#ifndef AHXBSR_H
#define AHXBSR_H
#include <sg_xpt.h>
#include <sj.h>

typedef struct ahxbsr_obj *AHXBSR;

/* ahxbsr_init counts a use of the pool, and ahxbsr_finish ends one. the
 * first use and the last clear all eight readers */
void ahxbsr_init(void);
void ahxbsr_finish(void);
/* a reader of sj's data line, or NULL where all eight are in use. it takes the
 * data a chunk at a time. a chunk is a quarter of the bytes sj holds, data and
 * free, as the reader is made */
AHXBSR ahxbsr_create(SJ sj);
void ahxbsr_destroy(AHXBSR reader);

/* the next width bits, width from 0 to 32. where sj can't give that many, it
 * drops the bits it holds. it then reports an ADX error and returns 0 */
Uint32 AHXBSR_GetBitStm(AHXBSR reader, Sint32 width);
Sint32 ahxbsr_position(AHXBSR reader);
Bool ahxbsr_empty(AHXBSR reader);
/* skip to the next byte boundary, then read on four bits at a time. it returns
 * 1 once the last 12 bits read are an MPEG frame's sync word, all ones. it
 * returns 2 once the last 32 are the stream's end mark. it returns 0 where the
 * input runs out first */
Sint32 ahxbsr_find_frame(AHXBSR reader);
/* ahxbsr_find_frame's results */
#define AHXBSR_EMPTY 0
#define AHXBSR_SYNC 1
#define AHXBSR_END 2

/* the calls of AHXBSR_GetBitStm since mpeg_buffer_sample or
 * mpeg_decode_samples last cleared it */
extern Sint32 ahxbsr_read_count;

#endif
