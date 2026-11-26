/* the prep state's step, which adx_tsvr.c builds for ADX and for AHX, each
 * under the name PREP_FUNCTION gives. each build gives its decoder in
 * PREP_DECODER, its block's samples in PREP_BLOCK_SAMPLES and its end test in
 * PREP_AT_END. PREP_OUTPUTS gives the stream joint, an SJ, of each channel */
#include <adx_inis.h>
#include <string.h>

PREP_FUNCTION(ADXT adxt)
{
    PREP_DECODER;
    ADXRNA renderer = adxt->rna;
    Sint32 buffered;
    Sint32 room;
    Sint32 channels;
    Sint32 bytes;
    Sint32 channel;
    SJ output;
    SJCK chunk;

    buffered = ADXRNA_GetNumData(renderer);
    room = ADXRNA_GetNumRoom(renderer);
    if (buffered >= adxt->maxdecsmpl || room <= PREP_BLOCK_SAMPLES ||
        PREP_AT_END) {
        if (adxt->pstwait_flag == 0) {
            if (adxt->pause_flag == 0) {
                ADXRNA_SetPlaySw(renderer, TRUE);
                adxt->tvofst = 0;
                adxt->svcnt = adxt_vsync_cnt;
            }
            adxt->stat = ADXT_STAT_PLAYING;
        }
        adxt->pstready_flag = TRUE;
    }
    if (PREP_AT_END) {
        channels = ADXT_GetNumChan(adxt);
        bytes = adxt->maxdecsmpl * channels * sizeof(Sint16);
        for (channel = 0; channel < channels; channel++) {
            output = PREP_OUTPUTS[channel];
            SJ_GetChunk(output, SJ_LIN_FREE, bytes, &chunk);
            memset(chunk.data, 0, chunk.len);
            SJ_PutChunk(output, SJ_LIN_DATA, &chunk);
        }
    }
}

#undef PREP_FUNCTION
#undef PREP_DECODER
#undef PREP_BLOCK_SAMPLES
#undef PREP_AT_END
#undef PREP_OUTPUTS
