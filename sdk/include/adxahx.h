/* ADXT's AHX player, a pool of two. a player decodes the MPEG layer II stream
 * in its input joint. it writes each channel's 16-bit PCM to a joint of its own
 */
#ifndef ADXAHX_H
#define ADXAHX_H
#include <sg_xpt.h>
#include <sj.h>

typedef struct adxahx_player *ADXAHX;

#define ADXAHX_STOP 0
#define ADXAHX_HEADER 1
#define ADXAHX_DECODE 2
#define ADXAHX_END 3

/* adxahx_init clears both players and counts a use of the MPEG decoders, as
 * mpeg_init does. adxahx_finish clears the players and ends that use */
void adxahx_init(void);
void adxahx_finish(void);
/* a player that reads input and writes outputs[0] to outputs[channels - 1]. it
 * is NULL where both players, all the bit readers or both MPEG decoders are in
 * use. a step hangs where outputs[0]'s free line can't give a granule's samples
 * in one chunk */
ADXAHX adxahx_create(SJ input, Sint32 channels, SJ *outputs);
void adxahx_destroy(ADXAHX player);
Sint32 adxahx_state(ADXAHX player);
void adxahx_set_input(ADXAHX player, SJ input);
void adxahx_set_sample_limit(ADXAHX player, Sint32 samples);

/* adxahx_start clears the counts and reads the input joint through a new bit
 * reader. adxahx_step then reads the stream's header, once the input holds 36
 * bytes. each later step decodes a granule, where outputs[0] has room for
 * one. the player is ADXAHX_END once the stream ends */
void adxahx_start(ADXAHX player);
void adxahx_stop(ADXAHX player);
void adxahx_step(ADXAHX player);

Sint32 adxahx_sample_rate(ADXAHX player);
Sint32 adxahx_channels(ADXAHX player);
Sint32 adxahx_sample_bits(ADXAHX player);
Sint32 adxahx_total_samples(ADXAHX player);
Sint32 adxahx_output_volume(ADXAHX player);
Sint32 adxahx_direct_level(ADXAHX player);
void adxahx_effect(ADXAHX player, Sint32 *channel, Sint32 *level);
Sint32 adxahx_pan(ADXAHX player, Sint32 channel);

#endif
