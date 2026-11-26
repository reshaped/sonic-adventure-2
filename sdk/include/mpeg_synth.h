/* the MPEG layer II synthesis filters, a pool of two. a filter turns one
 * sample of each of the 32 subbands into 32 PCM samples */
#ifndef MPEG_SYNTH_H
#define MPEG_SYNTH_H
#include <sg_xpt.h>

typedef struct mpeg_synth MPEG_SYNTH;

/* mpeg_synth_init counts a use of the pool, and mpeg_synth_finish ends one.
 * the first use and the last clear both filters. each mpeg_synth_init also
 * copies both coefficient tables up to their next 32-byte boundary, in
 * place */
void mpeg_synth_init(void);
void mpeg_synth_finish(void);
/* a free filter, with a history of its own for each of channels 0 and 1. it is
 * NULL where both filters are in use */
MPEG_SYNTH *mpeg_synth_create(void);
void mpeg_synth_destroy(MPEG_SYNTH *filter);
/* run one sample of each subband, the 32 floats at samples, through channel's
 * history. write the 32 16-bit samples that result to pcm. samples must be
 * 8-byte aligned */
void mpeg_synth_pcm(MPEG_SYNTH *filter, const Float *samples, Sint32 channel,
                    Sint16 *pcm);

#endif
