#include <math.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the kind of samples: bits 4 and 5 of the play command's first parameter,
 * above the slot's number */
#define PLAY_16BIT 0x00
#define PLAY_8BIT 0x10
#define PLAY_ADPCM 0x30
/* the filter Q the play command gives a channel whose base_filter_lev is
 * SDD_PSTM_PORT_SLOT_FILTER_THROUGH, in place of its base_filter_q */
#define PLAY_Q_THROUGH 0x80
/* the play command takes a volume at twice a base volume's scale */
#define PLAY_VOL_SHIFT 1

/* the play command gives a sample rate as a base note and a fine tune. base
 * note 0 is NOTE_0_FREQ hertz, and every OCTAVE_NOTES notes above it halve the
 * rate. so 44100 hertz, five octaves lower, is base note 60. a fine tune
 * counts NOTE_UNIT to the note */
#define NOTE_0_FREQ 1411200.0F
#define OCTAVE_NOTES 12
#define NOTE_UNIT 256

/* the member exports this name, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 sdPstmCalcHzToBaseNote(Float freq, Sint8 *base_note, Sint8 *fine);
/* NOLINTEND(misc-use-internal-linkage) */

SDE_ERR sdPstmPlay(SDPSTM handle, const SDE_PCM_TYPE pcm_type,
                   const Uint16 freq)
{
    /* @hack spelling: the command as a static inside the function, where one
     * at file scope would do */
    static struct sd_host_cmd pstm_play_cmd;
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 port;
    Sint32 type;
    Sint32 i;
    Sint8 base_note;
    Sint8 fine;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    sdPstmCalcHzToBaseNote(freq, &base_note, &fine);
    /* @hack spelling: port, a local for the port's number, where ((struct
     * sd_port_head *)handle)->port as the index would do */
    port = ((struct sd_port_head *)handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    /* @bug logic: stores pcm_type even where the switch below refuses it
     * (MOV.L R1,@(R0,R14) at 0x8c177e32). sdPstmIsTransferWaveData and
     * sdPstmTransferWaveData then cap the ring buffer by a limit they never
     * set */
    ext->pcm_type = pcm_type;
    ext->freq = freq;
    if (ext->slot_num > SDD_PSTM_PORT_SLOT_NUM) {
        /* @hack spelling: a goto into the switch's default, where result =
         * SDE_ERR_PRM_OVER_RANGE and goto exit here would do */
        goto over_range;
    }
    switch (ext->pcm_type) {
    case SDE_PCM_TYPE_16BIT_LINEAR:
        type = PLAY_16BIT;
        break;
    case SDE_PCM_TYPE_8BIT_LINEAR:
        type = PLAY_8BIT;
        break;
    case SDE_PCM_TYPE_4BIT_ADPCM:
        type = PLAY_ADPCM;
        break;
    default:
    over_range:
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    for (i = 0; i < ext->slot_num; i++) {
        pstm_play_cmd.cmd = SD_HOST_CMD_PSTM_PLAY;
        pstm_play_cmd.prm[0] = ext->slot_no[i] | type;
        pstm_play_cmd.prm[1] = ext->ring_bank[i];
        pstm_play_cmd.prm[2] = ext->base[i].vol << PLAY_VOL_SHIFT;
        pstm_play_cmd.prm[3] = ext->base[i].pan;
        pstm_play_cmd.prm[4] = base_note;
        pstm_play_cmd.prm[5] = fine;
        pstm_play_cmd.prm[6] = ext->base[i].fx_ch;
        pstm_play_cmd.prm[7] = ext->base[i].fx_lev;
        pstm_play_cmd.prm[8] = ext->base[i].drct_lev;
        pstm_play_cmd.prm[9] = 0;
        if (ext->base_filter_lev[i] == SDD_PSTM_PORT_SLOT_FILTER_THROUGH) {
            pstm_play_cmd.prm[10] = 0;
            pstm_play_cmd.prm[11] = 0;
            pstm_play_cmd.prm[12] = PLAY_Q_THROUGH;
        } else {
            pstm_play_cmd.prm[10] = ext->base_filter_lev[i];
            pstm_play_cmd.prm[11] = ext->base_filter_lev[i] >> 8;
            pstm_play_cmd.prm[12] = ext->base_filter_q[i];
        }
        result = sdHostCmdSet(&pstm_play_cmd);
        if (result != SDE_ERR_NOTHING) {
            goto exit;
        }
        ext->first_transfer[i] = FALSE;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmPlay, result);
    }
    return result;
}

/* store in *base_note and *fine the play command's base note and fine tune
 * for samples at freq hertz. the base note is the nearest, a tie going down.
 * fine is the rounded note minus the exact one, in 1/NOTE_UNIT of a note. a
 * whole note in those units has a low byte of 0. so fine's byte is the exact
 * note's units, negated. the result is always 0 */
Sint32 sdPstmCalcHzToBaseNote(Float freq, Sint8 *base_note, Sint8 *fine)
{
    Float octaves;
    Float units;
    Sint32 count;

    /* @hack spelling: both zeroed in one chained assignment, where two
     * statements would do */
    *base_note = *fine = 0;
    octaves = log10(NOTE_0_FREQ / freq) / log10(2.0F);
    units = octaves * (OCTAVE_NOTES * NOTE_UNIT);
    count = (Sint32)units;
    *base_note = (count + ((NOTE_UNIT / 2) - 1)) / NOTE_UNIT;
    *fine = -count;
    return 0;
}
