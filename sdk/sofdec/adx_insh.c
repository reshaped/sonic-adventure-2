/* ADXT_InsertHdrSfa, which has the encoder make an SFA stream's ADX header
 * and puts it on the player's input */
#include <adx_sfa.h>
#include <adx_sje.h>
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

#define RING_BYTES 1024
#define PLACEHOLDER_BYTES 32

static char adxt_hdbuf[RING_BYTES];
static char adxt_dmybuf[ADXT_MAX_NCH][PLACEHOLDER_BYTES];

void ADXT_InsertHdrSfa(ADXT player, Sint32 channels, Sint32 rate,
                       Sint32 samples)
{
    SJ header;
    SJ pcm[ADXT_MAX_NCH];
    SJ input;
    ADXSJE encoder;
    SJCK encoded;
    SJCK destination;

    ADXSJE_Init();
    header = SJRBF_Create(adxt_hdbuf, RING_BYTES, 0);
    /* each PCM ring claims RING_BYTES, more than its buffer holds. nothing
     * reads them: the one step ADXSJE_ExecServer runs below writes the
     * header alone */
    pcm[0] = SJRBF_Create(adxt_dmybuf[0], RING_BYTES, 0);
    pcm[1] = SJRBF_Create(adxt_dmybuf[1], RING_BYTES, 0);
    input = player->sji;
    encoder = ADXSJE_Create(ADXT_MAX_NCH, pcm, header);
    ADXSJE_SetConfigSfa(encoder, channels, rate, samples);
    ADXSJE_Start(encoder);
    ADXSJE_ExecServer();
    SJ_GetChunk(header, SJ_LIN_DATA, RING_BYTES, &encoded);
    if (encoded.len == 0) {
        for (;;) {
        }
    }
    SJ_GetChunk(input, SJ_LIN_FREE, encoded.len, &destination);
    if (destination.len < encoded.len) {
        for (;;) {
        }
    }
    memcpy(destination.data, encoded.data, encoded.len);
    SJ_PutChunk(header, SJ_LIN_FREE, &encoded);
    SJ_PutChunk(input, SJ_LIN_DATA, &destination);
    ADXSJE_Stop(encoder);
    ADXSJE_Destroy(encoder);
    SJ_Destroy(header);
    SJ_Destroy(pcm[1]);
    SJ_Destroy(pcm[0]);
    ADXSJE_Finish();
}
