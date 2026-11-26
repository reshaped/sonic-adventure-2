/* the audio output's pitch, which follows the handle's speed */
#include <mathf.h>
#include <mw_rna.h>
#include <sfd.h>
#include <sg_xpt.h>

/* the natural log of SFD_NORMAL_SPEED */
#define LOG_NORMAL_SPEED 6.9077554F
/* the cents in a pitch shift by a factor of e: 1200 an octave over logf(2) */
#define CENTS_PER_E 1731.234F
#define SEMITONES_PER_CENT 0.01F
#define CENTS_PER_SEMITONE 100

/* move the audio output's pitch by log2(speed / SFD_NORMAL_SPEED) octaves,
 * as semitones and cents */
void SFRNA_SetSpeed(struct sfd_handle *handle, Sint32 speed)
{
    void *output;
    Float shift;
    Sint32 semitones;
    Sint32 cents;

    SFSET_GetTrHn(handle, SFD_AUDIO_OUT, &output);
    if (output == NULL) {
        return;
    }
    shift = (logf((Float)speed) - LOG_NORMAL_SPEED) * CENTS_PER_E;
    semitones = (Sint32)(shift * SEMITONES_PER_CENT);
    cents = (Sint32)shift - (semitones * CENTS_PER_SEMITONE);
    mwRnaSetPitch(output, semitones, cents);
}
