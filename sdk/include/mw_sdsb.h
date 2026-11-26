/* the middleware's sound buffers: each plays one or two channels of samples
 * through a stream port of the sound library */
#ifndef MW_SDSB_H
#define MW_SDSB_H
#include <mw_sdbuf.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* how many sound buffers there are, and the most channels one plays */
#define MW_SB_NUM 8
#define MW_SB_CHAN_MAX 2
/* a channel's ring is MW_SB_RING_SIZE bytes of sound memory. rings lie
 * MW_SB_RING_PITCH bytes apart, which leaves a gap of 0x40 bytes after each.
 * mwSbTrns repeats a ring's first MW_SB_EX_SIZE bytes at the start of that
 * gap */
#define MW_SB_RING_SIZE 0x4000
#define MW_SB_EX_SIZE 32
#define MW_SB_RING_PITCH 0x4040
/* the calls that rotate a ring move it in blocks of MW_SB_BLOCK_SIZE
 * bytes, of which a ring has MW_SB_BLOCK_NUM */
#define MW_SB_BLOCK_SIZE 256
#define MW_SB_BLOCK_NUM (MW_SB_RING_SIZE / MW_SB_BLOCK_SIZE)

/* the lowest volume mwSbSetVol takes. the highest is 0 */
#define MW_SB_VOL_MIN (-959)
/* a pan runs from -MW_SB_PAN_MAX to MW_SB_PAN_MAX */
#define MW_SB_PAN_MAX 15
/* the lowest effect level and direct level. the highest is 0 */
#define MW_SB_LVL_MIN (-42)
/* the effect level a sound buffer opens with. like any level below
 * MW_SB_LVL_MIN, it makes mwlSbSetFx set the sound library's lowest level */
#define MW_SB_FX_LVL_OPEN (-45)
/* the most semitones and the most cents a pitch moves, either way */
#define MW_SB_PITCH_NOTES_MAX 96
#define MW_SB_PITCH_CENTS_MAX 99
/* the sound library's own levels run from -MW_SB_SD_LVL_MAX to
 * MW_SB_SD_LVL_MAX */
#define MW_SB_SD_LVL_MAX 127
/* a semitone, in the sound library's units of pitch */
#define MW_SB_SD_SEMITONE 256

/* the kinds of value set_ex_prm sets and get_ex_prm gets. a carries the
 * first value each kind names below, and b carries the second.
 * mwRnaSetStmHdInfo reads get_ex_prm's a for MW_SB_EX_PRM_STM_HD as a count.
 * where the count is over 0, it copies the sound buffer's settings into the
 * rna. the sound buffer's mwSbSetExPrm and mwSbGetExPrm take only the first
 * three kinds, and give that count as 0 */
#define MW_SB_EX_PRM_FX 0 /* the effect channel and the effect level */
#define MW_SB_EX_PRM_PITCH 1 /* the pitch's semitones and its cents */
#define MW_SB_EX_PRM_DRCT_LVL 2 /* the direct level alone */
#define MW_SB_EX_PRM_FILTER 3 /* the filter's cutoff and its resonance */
#define MW_SB_EX_PRM_STM_HD 4 /* a stream header's address alone */

struct mw_sb;
/* the calls on the sound buffers as a whole, for a caller that reaches
 * them through a table */
struct mw_sblib_vtbl {
    void *unk_00[3];
    void (*init)(void *obj);
    void (*finish)(void);
    struct mw_sb *(*open_port)(Sint32 nch);
};
struct mw_sblib_vtbl *mwSndGetLibHn(void);
/* the calls on one sound buffer, in a table of the same kind */
struct mw_sb_vtbl {
    void *unk_00[3];
    void (*close_port)(void *mwsnd);
    void (*play)(void *mwsnd);
    void (*stop)(void *mwsnd);
    Sint32 (*get_buf_size)(void *mwsnd);
    void (*set_num_chan)(void *mwsnd, Sint32 nch);
    Sint32 (*get_play_pos)(void *mwsnd, Sint32 ch);
    void (*set_sfreq)(void *mwsnd, Sint32 sfreq);
    Sint32 (*get_sfreq)(void *mwsnd);
    void (*set_bit_per_smpl)(void *mwsnd, Sint32 bits);
    Sint32 (*get_bit_per_smpl)(void *mwsnd);
    void (*set_vol)(void *mwsnd, Sint32 vol);
    Sint32 (*get_vol)(void *mwsnd);
    void (*set_pan)(void *mwsnd, Sint32 ch, Sint32 pan);
    Sint32 (*get_pan)(void *mwsnd, Sint32 ch);
    void (*set_ex_prm)(void *mwsnd, Sint32 kind, Sint32 a, Sint32 b);
    void (*get_ex_prm)(void *mwsnd, Sint32 kind, Sint32 *a, Sint32 *b);
    void (*trns)(void *mwsnd, Sint32 ch, Sint32 ofs, void *src, Sint32 nsmpl);
    void (*stop_trns)(void *mwsnd);
    Sint32 (*get_trns_stat)(void *mwsnd, Sint32 ch);
    Sint32 (*adjst_buf_pause)(void *mwsnd, Sint32 nsmpl, Sint32 *adjusted);
    Sint32 (*adjst_buf_dscrd)(void *mwsnd, Sint32 nsmpl);
};

struct mw_sb {
    /* mwg_sb_vtbl's address. mwSbOpenPort sets it before any step that can
     * fail, and mwSbClosePort clears it */
    const struct mw_sb_vtbl *vtbl;
    Bool used; /* TRUE while it is open */
    Sint32 max_chan; /* how many channels it was opened with */
    Sint32 num_chan; /* how many channels it plays */
    /* the bits of a sample: 4, 8 or 16, or 0 in a sound buffer just opened */
    Sint32 bit_per_smpl;
    Sint32 sfreq; /* the sample rate, in hertz, or 0 in one just opened */
    struct mw_snd_buf *buf[MW_SB_CHAN_MAX]; /* each channel's ring */
    /* the memory block mwlSbTrnsDat carries a channel's samples to its ring
     * with */
    SDMEMBLK trns_blk[MW_SB_CHAN_MAX];
    /* the memory block mwlSbTrnsEx repeats a ring's first bytes past its end
     * with */
    SDMEMBLK ex_blk[MW_SB_CHAN_MAX];
    SDPSTM port;
    Sint32 unk_34;
    Sint32 ring_adr[MW_SB_CHAN_MAX]; /* each ring's address in sound memory */
    Sint32 ring_size; /* a ring's size in bytes */
    /* how far the nsmpl last given to an adjust lay below a block boundary,
     * in samples. mwSbAdjstBufPause counts up to the next boundary at or
     * above nsmpl, and returns the count. mwSbAdjstBufDscrd counts up to the
     * next boundary above it, and stores nothing for an nsmpl under a
     * block */
    Sint32 to_bound_nsmpl;
    /* the values that mwSbSetVol, mwSbSetPan, mwlSbSetFx, mwlSbSetPitch and
     * mwlSbSetDrctLvl last set */
    Sint32 vol;
    Sint32 pan[MW_SB_CHAN_MAX];
    Sint32 fx_ch;
    Sint32 fx_lvl;
    Sint32 pitch_notes;
    Sint32 pitch_cents;
    Sint32 drct_lvl;
};

#endif
