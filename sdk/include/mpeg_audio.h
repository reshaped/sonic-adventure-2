/* the MPEG audio decoder AHX playback uses, a pool of two. a decoder
 * decodes layer II frames a granule at a time */
#ifndef MPEG_AUDIO_H
#define MPEG_AUDIO_H
#include <ahxbsr.h>
#include <sg_xpt.h>

typedef struct mpeg_decoder MPEG_DEC;

/* mpeg_init counts a use of the pool, and mpeg_finish ends one. the first
 * use clears both decoders and opens the synthesis filters. the last clears
 * the decoders and closes the filters */
void mpeg_init(void);
void mpeg_finish(void);
/* a free decoder, with a synthesis filter of its own. it is NULL where both
 * decoders or both filters are in use. it holds no stream and no frame */
MPEG_DEC *mpeg_create(void);
/* give dec, which mpeg_create returned, and its filter back to their pools,
 * and clear dec */
void mpeg_destroy(MPEG_DEC *dec);
/* forget what dec has read: the frame's header, allocations and scale
 * factors, the stream header's values and the granule's samples. dec keeps
 * its stream. it gives its filter back and takes one again, so the filter's
 * history starts clear */
void mpeg_reset(MPEG_DEC *dec);
/* reset dec, as mpeg_reset does, and have it read bsr */
void mpeg_set_stream(MPEG_DEC *dec, AHXBSR bsr);
/* read the ADX header that comes before the stream's first frame, where dec
 * has a stream. where ahx_read_header accepts it, dec takes the header's
 * total of samples and channel settings. it also takes the header's channels
 * and sample rate in place of each frame's. the header must fit in
 * MPEG_STREAM_HEADER_MAX bytes and hold 2 channels or fewer */
void mpeg_read_stream_header(MPEG_DEC *dec);
#define MPEG_STREAM_HEADER_MAX 512
/* read the next frame's header, bit allocations and scale factors, and start on
 * its first granule. it returns 0, or 1 where the input ends before a frame's
 * sync word. it also returns 1 where the stream's end mark comes first. a dec
 * without a stream reads nothing and returns 0. the frame must carry no CRC:
 * the call reads the bits after the header as the bit allocations */
Sint32 mpeg_read_frame_header(MPEG_DEC *dec);
/* TRUE once each of the frame's granules is decoded */
Bool mpeg_frame_done(MPEG_DEC *dec);
/* decode the frame's next granule, channel 0's 96 samples into left. a frame of
 * two channels also writes channel 1's into right. it returns 96, or 0 once the
 * frame's granules are done, with nothing decoded. samples goes unread, so left
 * and right must each hold 96 */
Sint32 mpeg_decode_granule(MPEG_DEC *dec, Sint16 *left, Sint16 *right,
                           Sint32 samples);
/* the frame's sample rate in hertz, its channels, and the stream header's
 * total of samples. each is 0 until a frame's header is read */
Sint32 mpeg_sample_rate(MPEG_DEC *dec);
Sint32 mpeg_channels(MPEG_DEC *dec);
Sint32 mpeg_total_samples(MPEG_DEC *dec);
/* the bits of a decoded sample: 16, whatever dec */
Sint32 mpeg_sample_bits(MPEG_DEC *dec);
/* the samples a granule decodes for each channel: 96, whatever dec */
Sint32 mpeg_block_samples(MPEG_DEC *dec);
/* channel 0's output volume, as ADXT_SetOutVol takes it. it is 0 where the
 * stream header gives none or no frame's header is read */
Sint32 mpeg_out_vol(MPEG_DEC *dec);
/* channel 0's direct level, as ADXT_SetDrctLvl takes it. it is 0 where the
 * stream header gives none or no frame's header is read */
Sint32 mpeg_drct_lvl(MPEG_DEC *dec);
/* set *fxch and *fxlev to channel 0's effect channel and level, as ADXT_SetFx
 * takes them. where the stream header gives none, or no frame's header is
 * read, they are 0 and MW_SB_FX_LVL_OPEN */
void mpeg_fx(MPEG_DEC *dec, Sint32 *fxch, Sint32 *fxlev);
/* channel's pan as the stream header gives it, from -16 to 15. it is 0 before a
 * frame's header is read. in a stereo frame, channel 1 takes its own pan and
 * any other channel takes channel 0's. in a mono frame, every channel takes
 * channel 0's. where the header gives no pan, channel 1 of a stereo frame takes
 * 15 and its others -15. a mono frame's channels then take 0 */
Sint32 mpeg_channel_pan(MPEG_DEC *dec, Sint32 channel);

#endif
