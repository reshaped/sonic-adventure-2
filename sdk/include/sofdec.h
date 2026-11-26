/* CRI's Sofdec movie library, as far as the sources use it */
#ifndef SOFDEC_H
#define SOFDEC_H
#include <sg_xpt.h>

/* copy nword 4-byte words from src to dst, from the first up. both must lie
 * on a 4-byte boundary. the two may overlap only where dst lies below src.
 * nword may be 0. the copy takes nword % 16 words first, then blocks of 16.
 * each block prefetches the line that holds the next block's first word. the
 * last block's prefetch reads the line past the source's end */
void UTY_MemcpyDword(Uint32 *dst, const Uint32 *src, Uint32 nword);
/* table's size in the 4-byte words that UTY_MemcpyDword counts */
#define UTY_DWORDS(table) (sizeof(table) / 4)

/* Fill nword aligned 32-bit words, starting at the last. */
void UTY_MemsetDword(Uint32 *dst, Uint32 value, Uint32 nword);

/* Start-code masks. Bsearch scans length bytes ending at cursor, Search
 * scans length bytes starting at cursor. Both return the first byte of a
 * matching start code or null. Copy consumes the matching code as well. */
#define MPV_DELIM_SLICE 1
#define MPV_DELIM_FIRST_SLICE 2
#define MPV_DELIM_PICTURE 4
#define MPV_DELIM_GROUP 8
#define MPV_DELIM_EXTENSION 16
#define MPV_DELIM_USER_DATA 32
#define MPV_DELIM_SEQUENCE 64
#define MPV_DELIM_END 128
Sint32 MPV_CheckDelim(const Uint8 *buffer);
Uint8 *MPV_BsearchDelim(Uint8 *cursor, Sint32 length, Sint32 mask);
Uint8 *MPV_SearchDelim(Uint8 *cursor, Sint32 length, Sint32 mask);
Uint8 *MPV_CopyStm(Uint8 *destination, Uint8 *source, Sint32 length,
                   Sint32 mask);
/* MPEG video's start codes, each as a big-endian word, its 00 00 01
 * included. a slice's is 0x100 plus its macroblock row, counted from 1 */
#define MPV_PICTURE_START_CODE 0x100
#define MPV_FIRST_SLICE_CODE 0x101
#define MPV_LAST_SLICE_CODE 0x1af
#define MPV_USER_DATA_START_CODE 0x1b2
#define MPV_SEQUENCE_HEADER_CODE 0x1b3
#define MPV_EXTENSION_START_CODE 0x1b5
#define MPV_SEQUENCE_END_CODE 0x1b7
#define MPV_GROUP_START_CODE 0x1b8
/* a start code's bytes: 00 00 01, then the byte that says which code it is */
#define MPV_START_CODE_BYTES 4

/* a block's coefficients, 8 by 8, and a macroblock's blocks: four of
 * luminance and two of chrominance */
#define MPV_COEFS 64
#define MPV_BLOCKS 6
#define MPV_MB_BYTES (MPV_BLOCKS * MPV_COEFS)
#define MPV_CONDITIONS 8
#define MPV_COND_RECOVER 1
#define MPV_COND_DECODE 2
#define MPV_COND_HALF_PEL_2 3
#define MPV_COND_TEXTURE_ONLY 4
#define MPV_COND_LUMA_OFFSET 5

/* the bytes of the tables MPVVLC_Init copies into its work */
#define MPVVLC_WORK_SIZE 1456

/* the tables the library's decoders share, which MPV_Init sets up */
struct mpv_ixa {
    Uint16 mask[16]; /* entry n keeps a halfword's low 16 - n bits */
    Sint8 zigzag[MPV_COEFS]; /* a copy of mpvbdec_zigzag */
    Uint8 dct[0x90]; /* inverse transform matrices and their alignment */
    Uint8 vlc[MPVVLC_WORK_SIZE]; /* variable-length code tables */
    Uint8 clip[0x400]; /* signed sample to saturated pixel */
};

/* copy 8 rows of 8 pixels to block from src. block's rows lie 8 bytes apart,
 * and src's rows pitch bytes apart. block must lie on a 4-byte boundary */
typedef void (*MPVMC_COPY)(Uint8 *block, const Uint8 *src, Sint32 pitch);
/* the copies a decoder picks from, one for each value of the source address's
 * two low bits */
#define MPVMC_COPIES 4
/* a decoder's motion compensation state */
struct mpv_mc {
    MPVMC_COPY copy[MPVMC_COPIES];
    Sint32 unk_10;
    /* where MPVACMC_PpicSkipBlk copies its block, 8 bytes a row */
    Uint8 *out;
    /* the block an MPVAMC call fills, 8 bytes a row */
    Uint8 *block;
    /* the bytes between src's rows, for MPVACMC_PpicSkipBlk */
    Sint32 skip_pitch;
    /* the bytes between src's rows, and src2's, for the MPVAMC calls */
    Sint32 pitch;
    /* the reference pixels: the first at the block's place, and a second
     * for the averages of two */
    const Uint8 *src;
    const Uint8 *src2;
};

/* MPVMC_COPYs. MPVAMC_OneRef1Byte reads src a byte at a time. it also reads
 * src[8 * pitch], which must be readable. MPVAMC_OneRef1Word reads src's rows
 * as 2-byte words, so src and pitch must be even. MPVAMC_OneRef1Long reads
 * them as 4-byte words, so both must be multiples of 4 */
void MPVAMC_OneRef1Byte(Uint8 *block, const Uint8 *src, Sint32 pitch);
void MPVAMC_OneRef1Word(Uint8 *block, const Uint8 *src, Sint32 pitch);
void MPVAMC_OneRef1Long(Uint8 *block, const Uint8 *src, Sint32 pitch);

/* a macroblock's inverse DCT takes its MPV_BLOCKS blocks. a block's
 * coefficients are MPV_COEFS floats in the DCT's order. the first 32 hold each
 * row's coefficients 0, 4, 2 and 6 as a 4-vector. the last 32 hold its 1, 3, 5
 * and 7. the rows lie in the order 0, 4, 2, 6, 1, 3, 5 and 7. a block's samples
 * are 64 Sint16s in raster order. the transforms that are SH-4 assembly need
 * FPSCR's PR and SZ clear, as SHC's code keeps them.
 *
 * the coded block pattern marks the blocks to transform. DCTFZ_CBP_FIRST is
 * the first block's bit, and each later block's is the next lower */
#define DCTFZ_CBP_FIRST 0x80000000
/* the transform adds DCTFZ_BIAS + 0.5 to each sample, truncates it to an
 * integer, and takes DCTFZ_BIAS off. so a sample of -DCTFZ_BIAS - 0.5 or more
 * rounds to the nearest integer, a half up */
#define DCTFZ_BIAS 512

/* the parameters of a macroblock's inverse DCT, which DCT_FzInitPa sets up */
struct dctfz_pa {
    /* each block's MPVABDEC result. 0 marks a block whose only coefficient
     * is its DC */
    Uint8 last[8];
    /* DCTAFZ_SparseDc, the transform of a block whose only coefficient is its
     * DC */
    void (*sparse)(const Float *coef, Sint16 *blk);
    /* DCTAFZ_DenseCbp, the transform of the blocks cbp marks */
    void (*dense)(struct dctfz_pa *pa);
    Uint32 first_bit; /* DCTFZ_CBP_FIRST */
    Uint32 first_keep; /* every bit but DCTFZ_CBP_FIRST */
    Sint32 count[2]; /* the counters returned by MPV_GetDctCnt */
    Float round; /* DCTFZ_BIAS + 0.5 */
    /* the bytes from a block's coefficients to the next block's */
    Sint32 coef_stride;
    Sint32 cbp; /* the coded block pattern dense transforms */
    Float *coef; /* the first block's coefficients */
    Sint16 **blk; /* where each block's samples go */
    Sint16 **next_blk; /* dense's place in blk */
    Sint32 bias; /* DCTFZ_BIAS */
    Sint32 unk_3c;
    Float *mtx_even; /* dctfz_p_mtx_even, as DCT_FzInitPa read it */
    Float *mtx_odd; /* dctfz_p_mtx_odd, as DCT_FzInitPa read it */
    /* dense's room for its row pass's results, 256 bytes a block */
    Float *work;
    /* nleft is dense's count of blocks left, and work_at its place in work.
     * dense keeps both here while it needs the registers */
    Sint32 nleft;
    Float *work_at;
};

/* fill blk's samples with coef[0] / 8, the transform of a block whose only
 * coefficient is its DC. coef[0] is truncated to an integer first, and the
 * quotient is rounded half away from zero. blk must lie on an 8-byte
 * boundary */
void DCTAFZ_SparseDc(const Float *coef, Sint16 *blk);
/* transform each block pa->cbp marks, from its coefficients to its samples.
 * pa->next_blk must point to the first block's pointer. the call steps it past
 * all six, the unmarked blocks' too. each sample has pa->round added, is
 * truncated, and has pa->bias taken off. it isn't clamped. the call
 * overwrites the marked blocks' coefficients and their room in pa->work. it
 * also reads the 16 bytes past each marked block's coefficients and past its
 * room in pa->work, which must be readable.
 *
 * pa, pa->coef, pa->work and the matrices must lie on an 8-byte boundary, and
 * pa->coef_stride must be 256. the call changes FR12 to FR15 and XMTRX, which
 * a C caller must save */
void DCTAFZ_DenseCbp(struct dctfz_pa *pa);
/* transform the six blocks. a block whose pa->last is 0 goes to pa->sparse.
 * pa->dense takes the rest, if any, which pa->cbp then marks */
void DCT_FzTrans6Blk(struct dctfz_pa *pa);
/* transform the blocks pa->cbp marks. a marked block whose pa->last is 0 goes
 * to pa->sparse, and its mark is cleared. pa->dense takes the rest, if any */
void DCT_FzTransCbp(struct dctfz_pa *pa);

/* macroblock_type's flags */
#define MPVVLC_MB_INTRA 0x01
#define MPVVLC_MB_PATTERN 0x02
#define MPVVLC_MB_BACKWARD 0x04
#define MPVVLC_MB_FORWARD 0x08
#define MPVVLC_MB_QUANT 0x10
/* marks a P or B picture's address increment entry that also takes
 * macroblock_type's code. the entry holds this flag beside the type's flags */
#define MPVVLC_MB_TYPED 0x20
/* the increments of the address increment entries for the stuffing code, the
 * escape and no valid code */
#define MPVVLC_MBAI_STUFFING 34
#define MPVVLC_MBAI_ESCAPE 35
#define MPVVLC_MBAI_INVALID 36
/* the code of a motion_code entry for no valid code */
#define MPVVLC_MOTION_INVALID 0x7f

/* the variable-length code tables, which mpv_vlc.c's MPVVLC_Init fills.
 * each mpvvlc_ pointer points at its table, or at the table's copy in the
 * work MPVVLC_Init was given. MPVVLC_MB_INVALID and MPVVLC_RUN_LEVEL_ESCAPE,
 * which these comments name, are mpv_vlc.c's */

/* macroblock_address_increment, a pair of tables for each picture type. _0
 * holds the codes that start with four zeros. its index is the stream's bits
 * after those zeros. _1 holds the other codes. its index is the stream's bits
 * from the code's first bit. for an I picture, _0's index is 8 bits wide and
 * _1's 6. for a P or B picture, they are 7 and 5.
 *
 * an entry's bits 0-3 are the stream bits it takes, and bits 4-9 the
 * increment. where the entry takes macroblock_type's code too, bits 10-15
 * hold the type's flags. any other entry holds 0 there. an I picture's
 * entries for the increments 1 to 33 all take the type's code. the entries
 * for no valid code take no bits */
#define MPVVLC_MBAI_LENGTH 15
#define MPVVLC_MBAI_INCREMENT_SHIFT 4
#define MPVVLC_MBAI_FLAGS_SHIFT 10
extern const Sint16 *mpvvlc_mbai_i_0;
extern const Sint16 *mpvvlc_mbai_i_1;
extern const Sint16 *mpvvlc_mbai_p_0;
extern const Sint16 *mpvvlc_mbai_p_1;
extern const Sint16 *mpvvlc_mbai_b_0;
extern const Sint16 *mpvvlc_mbai_b_1;
/* macroblock_type, for P and B pictures. p_mbtype is indexed by the stream's
 * next 5 bits, and b_mbtype by its next 6. an entry's bits 0-7 are the bits
 * it takes, and bits 8-12 the type's flags. b_mbtype's entry for no valid
 * code holds MPVVLC_MB_INVALID, all five flags, and takes no bits */
#define MPVVLC_MBTYPE_LENGTH 0xff
#define MPVVLC_MBTYPE_FLAGS_SHIFT 8
extern const Sint16 *mpvvlc_p_mbtype;
extern const Sint16 *mpvvlc_b_mbtype;
/* motion_code, a pair split as the address increment pairs are. _0 is indexed
 * by 7 bits, and _1 by 5. an entry's low byte is the code, from -16 to 16 in
 * two's complement. its high byte is the bits it takes. the entries for no
 * valid code hold MPVVLC_MOTION_INVALID as their code, and take no bits */
#define MPVVLC_MOTION_LENGTH_SHIFT 8
extern const Sint16 *mpvvlc_motion_0;
extern const Sint16 *mpvvlc_motion_1;
/* coded_block_pattern, indexed by 9 bits. the low byte of an entry is the
 * bits it takes. the high byte holds the pattern in bits 0-5, and the
 * pattern's bits 0-1 again in bits 6-7. an entry for no valid code is 0 */
#define MPVVLC_CBP_LENGTH 0xff
#define MPVVLC_CBP_PATTERN_SHIFT 8
extern const Sint16 *mpvvlc_cbp;
/* dct_dc_size for luminance and for chrominance, each indexed by 7 bits. the
 * low 4 bits of an entry are the bits it takes, and the high 4 the size */
#define MPVVLC_DCSIZ_SIZE_SHIFT 4
extern const Uint8 *mpvvlc_y_dcsiz;
extern const Uint8 *mpvvlc_c_dcsiz;
/* the run and level codes that start with a 0. the block decoder reads the
 * codes that start with a 1 itself. an entry holds the run in bits 0-7, and
 * the level's magnitude in bits 8-15.
 *
 * run_level_8 holds the codes with 1 to 5 leading zeros, indexed by a code's
 * first 8 bits. its entries hold the bits the code takes, its sign bit
 * included, in bits 16-23. its escape entries hold MPVVLC_RUN_LEVEL_ESCAPE as
 * run and level. its entries for 6 or more zeros are 0.
 *
 * run_level_4, 2, 1, 0a, 0b and 0c hold the codes with 6, 7, 8, 9, 10 and 11
 * leading zeros. a 1 ends a code's zeros. the bits after it index the code's
 * table, 3 for run_level_4 and 4 for the others */
#define MPVVLC_RUN_LEVEL_MAGNITUDE_SHIFT 8
#define MPVVLC_RUN_LEVEL_LENGTH_SHIFT 16
extern const Uint16 *mpvvlc_run_level_0c;
extern const Uint16 *mpvvlc_run_level_0b;
extern const Uint16 *mpvvlc_run_level_0a;
extern const Uint16 *mpvvlc_run_level_1;
extern const Uint16 *mpvvlc_run_level_2;
extern const Uint16 *mpvvlc_run_level_4;
extern const Uint32 *mpvvlc_run_level_8;

/* the block decoder's parameters for a block */
struct mpvbdec_pa {
    Sint32 unk_00;
    Sint32 unk_04;
    Sint32 unk_08;
    Sint32 unk_0c;
    /* the place in the DCT's order of the block's first coefficient */
    Sint32 first;
    Sint32 unk_14;
    Sint32 unk_18;
    Float *coef;
    const Uint8 *qm; /* the quantizer matrix, in the DCT's order */
    Sint32 qscale; /* quantizer_scale */
    Sint32 *dc_pred; /* the DC predictor of an intra block's component */
    const Uint8 *dcsiz; /* mpvvlc_y_dcsiz or mpvvlc_c_dcsiz */
    /* the term added to each level's doubled magnitude. MPEG-1 has it 0 in an
     * intra block and 1 in a non-intra block */
    Sint32 level_add;
};

/* the error function. it is passed the obj entered with it and the error's
 * code */
typedef void (*MPV_ERRFN)(void *obj, Sint32 ecode);
/* the errors of one decoder, or the library's own for no decoder. func is
 * null where nothing takes them */
struct mpv_errinf {
    MPV_ERRFN func;
    void *obj;
    Sint32 ecode; /* the last error's code, or 0 */
    Sint32 delimiter_misses;
    Sint32 other_errors;
};

struct mpv_input {
    Uint8 *data;
    Sint32 size;
};

struct mpv_picture {
    Sint32 width;
    Sint32 height;
    Sint32 mb_width;
    Sint32 mb_height;
    Sint32 frame_rate;
    Sint32 temporal_ref;
    Sint32 coding_type;
    Sint32 drop_frame;
    Sint32 hours;
    Sint32 minutes;
    Sint32 seconds;
    Sint32 pictures;
    Sint32 gop;
};

/* Reference planes and their row pitches, in bytes. */
struct mpv_frame {
    Uint8 *chroma_b;
    Uint8 *chroma_r;
    Uint8 *luma;
    Sint16 chroma_pitch;
    Sint16 luma_pitch;
};

struct mpv_request {
    struct mpv_frame first;
    struct mpv_frame second;
    Uint8 *output;
    struct mpv_picture *picture;
    Sint32 delimiter_misses;
    Sint32 other_errors;
};

/* the bytes UTY_PushFr saves: FR12 to FR15, and the other bank's sixteen
 * registers */
#define MPV_FPU_SAVE_BYTES 80
/* the area UTY_PushFr and UTY_PopFr take. they move the saved bytes from its
 * first 8-byte boundary on. the extra word is the room an area on a 4-byte
 * boundary needs to reach that boundary */
struct mpv_fpu_save {
    Uint32 words[(MPV_FPU_SAVE_BYTES / 4) + 1];
};

/* One macroblock's decoded residuals and its two motion predictions. */
struct mpv_mb {
    const Uint8 *clip;
    Sint16 *blocks;
    Uint8 *prediction;
    Uint8 *backward_prediction;
};

struct mpv_out_block {
    Uint8 *start;
    Sint32 pitch;
};

struct mpv_output {
    /* Remaining block count at which the clamp kernel begins to subtract 16.
     * Four selects the luma blocks. Minus one leaves every block untouched. */
    Sint32 subtract_at;
    struct mpv_out_block blocks[MPV_BLOCKS];
};

struct mpv_motion {
    Sint32 full_pel;
    Sint32 r_size;
    Sint32 shift;
    Sint32 f;
    Sint32 predict_x;
    Sint32 predict_y;
    Sint32 x;
    Sint32 y;
    Uint8 unk_20[4];
};

struct mpv_obj;
typedef Sint32 (*MPV_MB_DECODE)(struct mpv_obj *, Uint8 **, Sint32 *, Sint32 *);
typedef void (*MPV_MB_SKIP)(struct mpv_obj *, Sint32);
typedef Sint32 (*MPV_BLOCK_DECODE)(struct mpv_obj *);
typedef void (*MPV_MB_MAKE)(struct mpv_obj *);

/* a decoder, which an MPV handle names */
struct mpv_obj {
    /* the stream's bits. bits holds the current word's unread bits, from its
     * top bit. nused counts the bits read from that word. next is the word
     * after it, and rd points at the stream's word after that. the stream's
     * words are big-endian */
    Uint32 bits;
    Uint32 next;
    Sint32 nused;
    const Uint32 *rd;
    /* the tables of mpvvlc_run_level_8 to mpvvlc_run_level_0c. run_level_4
     * points 16 bytes below the start of its table. run_level_2 and
     * run_level_1 point 32 bytes below the starts of their own */
    const Uint32 *run_level_8;
    const Uint16 *run_level_4;
    const Uint16 *run_level_2;
    const Uint16 *run_level_1;
    const Uint16 *run_level_0a;
    const Uint16 *run_level_0b;
    const Uint16 *run_level_0c;
    const Sint8 *zigzag; /* ixa->zigzag */
    struct mpv_ixa *ixa;
    Uint8 *dct_work;
    const Uint8 *clip;
    struct mpvbdec_pa bpa;
    struct dctfz_pa dct;
    struct mpv_mc mc;
    Uint8 unk_f0[8];
    struct mpv_mb mb;
    struct mpv_output reference_output;
    struct mpv_output texture_output;
    Sint32 state;
    Sint32 busy_count;
    Sint32 cond[MPV_CONDITIONS];
    struct mpv_picture picture;
    struct mpv_errinf err;
    Uint8 unk_1e0[12];
    struct mpv_request request;
    Sint32 aspect_ratio;
    Sint32 bit_rate;
    Sint32 vbv_buffer_size;
    Sint32 constrained;
    Sint32 closed_gop;
    Sint32 broken_link;
    Sint32 vbv_delay;
    MPV_MB_DECODE decode_mb;
    MPV_MB_SKIP skipped;
    MPV_BLOCK_DECODE intra_blocks;
    MPV_BLOCK_DECODE nonintra_blocks;
    MPV_MB_MAKE intra_maker;
    MPV_MB_MAKE makers[4];
    Sint32 qscale; /* the macroblock's quantizer_scale */
    struct mpv_motion forward;
    struct mpv_motion backward;
    Sint32 index;
    Sint32 last_index;
    Sint32 mb_type;
    /* the macroblock's coded block pattern. shifted left by 2, it is the
     * DCT's pattern */
    Sint32 cbp;
    /* the DC predictors of luminance and of the two chrominance components */
    struct {
        Sint32 y;
        Sint32 cb;
        Sint32 cr;
    } dc_pred;
    Sint16 *block_ptr[MPV_BLOCKS]; /* inverse DCT output order */
    Uint8 unk_2dc[4];
    Sint16 blocks[MPV_BLOCKS][MPV_COEFS];
    Float coef[MPV_BLOCKS][MPV_COEFS]; /* the macroblock's coefficients */
    /* the intra quantizer matrix, in the DCT's order */
    Uint8 intra_qm[MPV_COEFS];
    /* the non-intra quantizer matrix, in that order */
    Uint8 nintra_qm[MPV_COEFS];
    Uint8 mb_work[4 * MPV_MB_BYTES]; /* shared by DCT and motion prediction */
    Sint8 mbpos[1200][2]; /* macroblock address to row and column */
    Uint8 unk_1bc0[0x440];
};
/* the indexes of an mbpos entry's row and column */
enum { MPV_MB_ROW, MPV_MB_COLUMN };

/* the faults MPV_Init checks the library for. MPV_ERR_SECTION_ORDER marks
 * sections PSGSFD00 to PSGSFD02 linked out of order, and MPV_Init returns it.
 * after any other fault MPV_Init never returns */
#define MPV_ERR_ALIGNMENT ((Sint32)0xff03ff01)
#define MPV_ERR_GUARD ((Sint32)0xff03ff02)
#define MPV_ERR_VLC_SIZE ((Sint32)0xff03ff03)
#define MPV_ERR_SECTION_SIZE ((Sint32)0xff03ff04)
#define MPV_ERR_SECTION_ORDER ((Sint32)0xff03ff05)
/* the error each call returns for a handle MPVLIB_CheckHn refuses.
 * MPV_DecodePicAtr returns MPV_GetPicAtr's */
#define MPV_ERR_DESTROY ((Sint32)0xff030201)
#define MPV_ERR_SET_COND ((Sint32)0xff030202)
#define MPV_ERR_SET_ERR_FUNC ((Sint32)0xff030203)
#define MPV_ERR_GET_ERR_INF ((Sint32)0xff030204)
#define MPV_ERR_DECODE_FRM ((Sint32)0xff030209)
#define MPV_ERR_SKIP_FRM ((Sint32)0xff03020a)
#define MPV_ERR_GET_PIC_ATR ((Sint32)0xff03020c)
#define MPV_ERR_GET_BIT_RATE ((Sint32)0xff03020d)
#define MPV_ERR_GET_LINK_FLG ((Sint32)0xff03020e)
/* MPV_SkipFrm's error for data with no start code after the picture */
#define MPV_ERR_NO_DELIMITER ((Sint32)0xff030305)

/* A pool of count decoders uses one 8 KB page per decoder and two more
 * pages for alignment and tables. Create returns a free decoder or null.
 * Passing a null handle to SetCond updates the defaults and every active
 * decoder. The condition index must be below MPV_CONDITIONS. */
Sint32 MPV_Init(Sint32 count, void *work);
void MPV_Finish(void);
struct mpv_obj *MPV_Create(void);
Sint32 MPV_Destroy(struct mpv_obj *handle);
Sint32 MPV_SetCond(struct mpv_obj *handle, Sint32 id, Sint32 value);
void MPV_GetDctCnt(struct mpv_obj *mpv, Sint32 *first, Sint32 *second);

Uint32 MPVLIB_ConvIix(Uint32 address);
Uint32 MPVLIB_ConvOix(Uint32 address);
Sint32 MPVASEC_CheckSectionSiz(void);
Sint32 MPVASEC_CheckSectionSeq(void);
/* clear the library's own errors, as MPVERR_InitErrInf does */
void MPVERR_Init(void);
/* clear *errinf's code and counts, and leave it no function to take its
 * errors */
void MPVERR_InitErrInf(struct mpv_errinf *errinf);
/* The inverse DCT's build string begins and ends with CRLF. */
extern const char dct_version[];
const char *DCT_FzGetVerStr(void);
void DCT_FzInit(void *work);
void DCT_FzInitPa(struct dctfz_pa *pa);
/* build the tables that aren't constant. each mpvvlc_ pointer then points at
 * its table. a work that isn't null takes copies of the macroblock_type,
 * motion_code, dct_dc_size and run and level tables. their pointers point at
 * the copies instead. such a work must address MPVVLC_WORK_SIZE writable bytes
 * on a 4-byte boundary */
void MPVVLC_Init(Uint8 *work);
/* whether the tables MPVVLC_Init copies need more than MPVVLC_WORK_SIZE
 * bytes. it returns FALSE, since they fit */
Bool MPVVLC_IsVlcSizErr(void);
/* fill mpvbdec_zigzag and mpvbdec_dfl_iqm, then copy the zigzag and the masks
 * into ixa, which must not be null */
void MPVBDEC_Init(struct mpv_ixa *ixa);
void MPVCMC_Init(Uint8 *clip);
void MPVCMC_InitObj(struct mpv_obj *mpv);
void MPVMC_Init(struct mpv_mc *mc);
Sint32 MPV_DecodePicAtr(struct mpv_obj *handle, struct mpv_input *input,
                        Sint32 *consumed);
/* decode a macroblock's blocks from mpv's stream, and transform them into
 * samples. MPVCDEC_IntraBlocks decodes all six as intra blocks, and
 * MPVCDEC_NintraBlocks those mpv->cbp marks as non-intra blocks. each
 * returns 0 */
Sint32 MPVCDEC_IntraBlocks(struct mpv_obj *mpv);
Sint32 MPVCDEC_NintraBlocks(struct mpv_obj *mpv);
Sint32 MPVMDEC_IntraBlocks(struct mpv_obj *mpv);
Sint32 MPVMDEC_NintraBlocks(struct mpv_obj *mpv);
void MPVMMC_PpicSkipped(struct mpv_obj *mpv, Sint32 nmb);
void MPVMMC_BpicSkipped(struct mpv_obj *mpv, Sint32 nmb);
extern Sint8 mpvbdec_zigzag[MPV_COEFS];
extern Uint8 mpvbdec_dfl_iqm[MPV_COEFS];
void MPVCMC_IntraRt(struct mpv_obj *mpv);
void MPVCMC_IntraTa(struct mpv_obj *mpv);
void MPVCMC_ForwardRt(struct mpv_obj *mpv);
void MPVCMC_ForwardTa(struct mpv_obj *mpv);
void MPVCMC_Backward(struct mpv_obj *mpv);
void MPVCMC_BiDirect(struct mpv_obj *mpv);
void MPVCMC_PpicSkipped(struct mpv_obj *mpv, Sint32 count);
void MPVCMC_BpicSkipped(struct mpv_obj *mpv, Sint32 count);
extern const Uint8 *mpv_clip_0_255_base;

void UTY_PushFr(struct mpv_fpu_save *area);
void UTY_PopFr(struct mpv_fpu_save *area);
void MPVCMC_InitMcOiRt(struct mpv_obj *mpv);
void MPVCMC_SetCcnt(struct mpv_obj *mpv);
Sint32 MPVDEC_DecPicture(struct mpv_obj *mpv, Uint8 *data, Sint32 size,
                         Sint32 *consumed);
/* have mpv's errors go to func with obj. it returns 0. an mpv that
 * MPVLIB_CheckHn refuses keeps its function. the call then returns the code
 * it reports as the library's error */
Sint32 MPV_SetErrFunc(struct mpv_obj *mpv, MPV_ERRFN func, void *obj);
/* Queries validate the decoder before writing their outputs. */
Sint32 MPV_GetPicAtr(struct mpv_obj *handle, struct mpv_picture *picture);
Sint32 MPV_GetBitRate(struct mpv_obj *handle, Sint32 *rate);
Sint32 MPV_GetLinkFlg(struct mpv_obj *handle, Sint32 *closed_gop,
                      Sint32 *broken_link);
Bool MPV_IsEmptyBpic(const Uint8 *data, Sint32 size, Sint32 macroblocks);
/* 0 where mpv is a decoder in use, and -1 otherwise */
Sint32 MPVLIB_CheckHn(struct mpv_obj *mpv);
/* make ecode the last error of mpv, or of the library for a null mpv. a
 * nonzero ecode also goes to the function that takes those errors, if any.
 * it returns ecode */
Sint32 MPVERR_SetCode(struct mpv_obj *mpv, Sint32 ecode);
Sint32 MPV_DecodeFrm(struct mpv_obj *handle, struct mpv_input *input,
                     Sint32 *consumed, struct mpv_request *request);
Sint32 MPV_SkipFrm(struct mpv_obj *handle, struct mpv_input *input,
                   Sint32 *consumed);
void MPVFRM_Init(void);

/* decode a block's coefficients from mpv's stream into bpa->coef, in the
 * DCT's order. mpv's reader moves past the block. each coefficient's run
 * places it through mpv->zigzag. a level's magnitude l becomes
 * (2l + bpa->level_add) x qscale x qm[place] / 16, rounded down. that product
 * is then made odd as (x - 1) | 1, and takes the level's sign. so a product
 * under 16 gives -1 before the sign.
 *
 * MPVABDEC_IntraBlock decodes an intra block, whose coefficients must be 0.
 * its DC is *bpa->dc_pred plus 8 times the difference the stream codes
 * through bpa->dcsiz. *bpa->dc_pred takes the DC. MPVABDEC_NintraBlock clears
 * the block first. it adds 1 to its first coefficient's doubled magnitude,
 * whatever bpa->level_add is.
 *
 * each call sets bpa->first to its first coefficient's place. where the block
 * holds no other coefficient, it returns that place. otherwise it returns the
 * last coefficient's place, negated. so an intra block returns 0 exactly
 * where its DC is its only coefficient. both need FPSCR's PR and SZ clear */
Sint32 MPVABDEC_IntraBlock(struct mpv_obj *mpv, struct mpvbdec_pa *bpa);
Sint32 MPVABDEC_NintraBlock(struct mpv_obj *mpv, struct mpvbdec_pa *bpa);
/* clear the six blocks' 64 floats at coef, which must lie on an 8-byte
 * boundary. the call needs FPSCR's PR and SZ clear */
void MPVABDEC_EraseBlock6(Float *coef);

/* Sofdec's setup before syHwInit, which is the manager's: mwMngPreInit */
void mwPlyPreInitSofdec(void);

#endif
