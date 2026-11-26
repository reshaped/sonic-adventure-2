/* read MPEG-1 video's headers, from the sequence's to the slice's. decode the
 * macroblocks of I, P and B pictures. mpvdec_DecSlice leaves the bit reader
 * in mpv's bits, next, nused and rd. the macroblock, block and motion
 * decoders read on from there */
#include <sg_xpt.h>
#include <sofdec.h>

#include "../sofdec/mpv_section.h"

#include "../sofdec/mpvadec_bits.h"

/* mpvdec_GoNextLayer's result where no start code follows, or one of a kind
 * its mask leaves out */
#define NO_LAYER (-1)
/* the increment a macroblock decoder takes for bits that are no increment
 * code. it takes it too for an increment past the picture's last macroblock */
#define INVALID_INCREMENT (-2)
/* mpvdec_Recover's error where *data isn't at a start code it wants and
 * MPV_COND_RECOVER is clear */
#define MISSED_DELIMITER (-2)
/* the error where the data ends inside a header, or before a start code
 * mpvdec_Recover looks for */
#define END_OF_INPUT (-3)
/* a mask of every kind of start code */
#define ANY_DELIMITER (-1)
/* a start code opens with 23 zero bits, a run no slice's coded data holds. a
 * peek of them all zero ends a slice's macroblocks */
#define START_CODE_ZEROS 23
/* a macroblock's side, MB_SIDE pixels, is 1 << MB_SHIFT */
#define MB_SHIFT 4
#define MB_SIDE (1 << MB_SHIFT)
#define QUANTIZER_BITS 5
/* an extra_bit_picture or extra_bit_slice that is set, and the byte of extra
 * information it announces */
#define EXTRA_BITS 9
/* the increment macroblock_escape adds */
#define ESCAPE_INCREMENT 33
/* a motion vector's signed bits where r_size is 0. each bit of r_size adds
 * one */
#define MOTION_BITS 5
/* each weight of MPEG's default non-intra quantizer matrix, 16, four to a
 * word */
#define NONINTRA_WEIGHTS 0x10101010
/* a DC prediction's reset: a pixel value of 128, at the DC coefficient's scale
 * of 8 */
#define DC_ORIGIN (128 * 8)
/* a group's time_code, and its fields from its low end. a marker bit stands
 * between the seconds and the minutes. drop_frame_flag is the top bit */
#define TIME_CODE_BITS 25
#define TIME_PICTURE_BITS 6
#define TIME_SECONDS_BITS 6
#define TIME_MINUTES_BITS 6
#define TIME_HOURS_BITS 5
#define LOW_BITS(width) ((1 << (width)) - 1)

/* picture_coding_type's values, the last index of the tables below that take
 * a coding type. each holds NULL at 0, and all but decode_macroblock hold NULL
 * at PICTURE_D too */
enum { PICTURE_I = 1, PICTURE_P, PICTURE_B, PICTURE_D, PICTURE_TYPES };

/* the index in mpv->makers of a macroblock's maker, by its directions: 1
 * backward, 2 forward and 3 both. makers[0] holds the maker the last
 * non-intra macroblock of a B picture took. MAKER_SHIFT is
 * MPVVLC_MB_BACKWARD's bit */
#define MAKER_SHIFT 2
#define MAKER_INDEX(type)                                                      \
    (((type) & (MPVVLC_MB_BACKWARD | MPVVLC_MB_FORWARD)) >> MAKER_SHIFT)

/* the static functions below that take data, size and consumed read the
 * stream at *data. *size counts the bytes from *data on. each steps *data past
 * what it reads, takes that many from *size, and adds them to *consumed */
static Sint32 mpvdec_DecShc(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                            Sint32 *consumed);
/* the member exports these two, which #pragma inline expands in
 * mpvdec_DecShc */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mpvdec_InitIqm(struct mpv_obj *mpv);
void mpvdec_InitNqm(struct mpv_obj *mpv);
/* NOLINTEND(misc-use-internal-linkage) */
static void mpvdec_InitMbAdr2Pos(struct mpv_obj *mpv);
static Sint32 mpvdec_DecGsc(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                            Sint32 *consumed);
static Sint32 mpvdec_DecPsc(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                            Sint32 *consumed);
static Sint32 mpvdec_GoNextLayer(struct mpv_obj *mpv, Uint8 **data,
                                 Sint32 *size, Sint32 *consumed, Sint32 mask);
static Sint32 mpvdec_GoNextDelim(Uint8 **data, Sint32 *size, Sint32 *consumed);
static Sint32 mpvdec_Recover(struct mpv_obj *mpv, Sint32 mask, Uint8 **data,
                             Sint32 *size, Sint32 *consumed);
static Sint32 mpvdec_DecSlice(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                              Sint32 *consumed);
static Sint32 mpvdec_DecIpicMb(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                               Sint32 *consumed);
static Sint32 mpvdec_DecPpicMb(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                               Sint32 *consumed);
static Sint32 mpvdec_DecBpicMb(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                               Sint32 *consumed);
static Sint32 mpvdec_DecDpicMb(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                               Sint32 *consumed);
static void mpvdec_ResetMv(struct mpv_motion *motion);
static void mpvdec_ResetDc(struct mpv_obj *mpv);
static Sint32 mpvdec_MotionSub(struct mpv_obj *mpv, struct mpv_motion *motion,
                               Sint32 *vector, Sint32 *predictor);

/* what mpvdec_DecPsc sets in mpv for a picture. decode_macroblock is indexed
 * by the picture's coding type, and decode_intra and decode_nonintra by
 * MPV_COND_DECODE's value. skip_macroblock and the make_ tables take that
 * value first and the coding type second. make_intra takes
 * MPV_COND_TEXTURE_ONLY's value ahead of both */
static const MPV_MB_DECODE decode_macroblock[PICTURE_TYPES] = {
    NULL,
    mpvdec_DecIpicMb,
    mpvdec_DecPpicMb,
    mpvdec_DecBpicMb,
    mpvdec_DecDpicMb,
};
static const MPV_MB_SKIP skip_macroblock[2][PICTURE_TYPES] = {
    {NULL, NULL, MPVMMC_PpicSkipped, MPVMMC_BpicSkipped, NULL},
    {NULL, NULL, MPVCMC_PpicSkipped, MPVCMC_BpicSkipped, NULL},
};
static const MPV_BLOCK_DECODE decode_intra[2] = {
    MPVMDEC_IntraBlocks,
    MPVCDEC_IntraBlocks,
};
static const MPV_BLOCK_DECODE decode_nonintra[2] = {
    MPVMDEC_NintraBlocks,
    MPVCDEC_NintraBlocks,
};
/* a maker whose name ends in Rt writes the reference picture too. one whose
 * name ends in Ta writes the texture alone. an I or P picture is a reference
 * unless the caller asks for the texture only. a B picture never is */
static const MPV_MB_MAKE make_intra[2][2][PICTURE_TYPES] = {
    {{NULL, NULL, NULL, NULL, NULL},
     {NULL, MPVCMC_IntraRt, MPVCMC_IntraRt, MPVCMC_IntraTa, NULL}},
    {{NULL, NULL, NULL, NULL, NULL},
     {NULL, MPVCMC_IntraTa, MPVCMC_IntraTa, MPVCMC_IntraTa, NULL}},
};
static const MPV_MB_MAKE make_forward[2][PICTURE_TYPES] = {
    {NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, MPVCMC_ForwardRt, MPVCMC_ForwardTa, NULL},
};
static const MPV_MB_MAKE make_backward[2][PICTURE_TYPES] = {
    {NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, MPVCMC_Backward, NULL},
};
static const MPV_MB_MAKE make_bidirectional[2][PICTURE_TYPES] = {
    {NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, MPVCMC_BiDirect, NULL},
};

#pragma inline(mpvdec_InitIqm, mpvdec_InitNqm)

Sint32 MPV_DecodePicAtr(struct mpv_obj *handle, struct mpv_input *input,
                        Sint32 *consumed)
{
    /* @hack spelling: mpv, a copy of handle, where handle would do */
    struct mpv_obj *mpv = handle;
    Uint8 *data;
    Sint32 size;
    Sint32 status;
    Sint32 delimiter;

    if (MPVLIB_CheckHn(mpv) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_GET_PIC_ATR);
    }
    data = input->data;
    size = input->size;
    *consumed = 0;
    for (;;) {
        status = mpvdec_Recover(
            mpv, MPV_DELIM_SEQUENCE | MPV_DELIM_GROUP | MPV_DELIM_PICTURE,
            &data, &size, consumed);
        if (status != 0) {
            return MPVERR_SetCode(mpv, status);
        }
        /* mpvdec_DecShc, mpvdec_DecGsc and mpvdec_DecPsc each step data to
         * the next start code. each returns 0 where that code opens the next
         * case's layer. otherwise the loop starts over at that code */
        delimiter = MPV_CheckDelim(data);
        switch (delimiter) {
        case MPV_DELIM_SEQUENCE:
            status = mpvdec_DecShc(mpv, &data, &size, consumed);
            if (status == NO_LAYER) {
                continue;
            }
            /* @hack spelling: the gotos to done, where returning status would
             * do */
            if (status != 0) {
                goto done;
            }
            /* fall through */
        case MPV_DELIM_GROUP:
            status = mpvdec_DecGsc(mpv, &data, &size, consumed);
            if (status != 0) {
                continue;
            }
            /* fall through */
        case MPV_DELIM_PICTURE:
            status = mpvdec_DecPsc(mpv, &data, &size, consumed);
            if (status != 0) {
                continue;
            }
            goto done;
        }
    }
done:
    return status;
}

/* read the sequence header at *data into mpv and mpv->picture. a quantizer
 * matrix the header leaves out takes its default. it returns 0 where a
 * group's header follows, as mpvdec_GoNextLayer finds it, and NO_LAYER where
 * none does */
static Sint32 mpvdec_DecShc(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                            Sint32 *consumed)
{
    struct mpv_picture *picture;
    /* @hack redundant: cursor, nused, lookahead, bits, present and weight
     * declared register, where plain locals would do */
    register Uint8 *cursor;
    register Sint32 nused;
    register Uint32 lookahead;
    register Uint32 bits;
    register Sint32 present;
    Sint32 i;
    register Uint32 weight;
    Uint32 end;
    Sint32 used;

    picture = &mpv->picture;
    OPEN_HEADER(data);
    TAKE_FIELD(12, picture->width);
    TAKE_FIELD(12, picture->height);
    TAKE_FIELD(4, mpv->aspect_ratio);
    TAKE_FIELD(4, picture->frame_rate);
    TAKE_FIELD(18, mpv->bit_rate);
    SKIP_BITS(1); /* a marker bit */
    TAKE_FIELD(10, mpv->vbv_buffer_size);
    TAKE_FLAG(mpv->constrained);
    /* each matrix comes in zigzag order */
    TAKE_FLAG(present);
    if (present) {
        for (i = 0; i < MPV_COEFS; i++) {
            TAKE_FIELD(8, weight);
            mpv->intra_qm[mpvbdec_zigzag[i]] = weight;
        }
    } else {
        mpvdec_InitIqm(mpv);
    }
    TAKE_FLAG(present);
    if (present) {
        for (i = 0; i < MPV_COEFS; i++) {
            TAKE_FIELD(8, weight);
            mpv->nintra_qm[mpvbdec_zigzag[i]] = weight;
        }
    } else {
        mpvdec_InitNqm(mpv);
    }
    picture->mb_width = (picture->width + MB_SIDE - 1) >> MB_SHIFT;
    picture->mb_height = (picture->height + MB_SIDE - 1) >> MB_SHIFT;
    mpv->last_index = (picture->mb_height * picture->mb_width) - 1;
    mpvdec_InitMbAdr2Pos(mpv);
    /* @hack type: end a byte count and then an address, as a Uint32, where a
     * pointer to the byte past the last bit would do */
    end = (nused + 7) >> 3;
    cursor -= AHEAD_BYTES;
    end += (Uint32)cursor;
    used = end - (Uint32)*data;
    *consumed += used;
    *size -= used;
    *data = (Uint8 *)end;
    return mpvdec_GoNextLayer(mpv, data, size, consumed, MPV_DELIM_GROUP);
}

void mpvdec_InitIqm(struct mpv_obj *mpv)
{
    UTY_MemcpyDword((Uint32 *)mpv->intra_qm, (const Uint32 *)mpvbdec_dfl_iqm,
                    UTY_DWORDS(mpv->intra_qm));
    /* @hack redundant: a return at the function's end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void mpvdec_InitNqm(struct mpv_obj *mpv)
{
    UTY_MemsetDword((Uint32 *)mpv->nintra_qm, NONINTRA_WEIGHTS,
                    UTY_DWORDS(mpv->nintra_qm));
    /* @hack redundant: a return at the function's end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* fill all of mbpos, past the picture's last macroblock too */
static void mpvdec_InitMbAdr2Pos(struct mpv_obj *mpv)
{
    Sint32 row;
    Sint32 column;
    Sint32 width;
    Sint8 *position;
    Sint8 *end;

    width = mpv->picture.mb_width;
    row = 0;
    column = 0;
    position = (Sint8 *)&mpv->mbpos;
    end = position + sizeof(mpv->mbpos);
    for (; position < end; position += 2, column++) {
        if (column >= width) {
            row++;
            column = 0;
        }
        position[MPV_MB_ROW] = row;
        position[MPV_MB_COLUMN] = column;
    }
}

/* read the group header at *data: its time code into mpv->picture, and
 * closed_gop and broken_link into mpv. it returns 0 where a picture's header
 * follows, as mpvdec_GoNextLayer finds it, and NO_LAYER where none does */
static Sint32 mpvdec_DecGsc(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                            Sint32 *consumed)
{
    struct mpv_picture *picture;
    Uint8 *cursor;
    Sint32 nused;
    Uint32 lookahead;
    Uint32 bits;
    Uint32 time_code;
    Sint32 end;
    Sint32 used;

    picture = &mpv->picture;
    picture->gop++;
    OPEN_HEADER(data);
    TAKE_FIELD(TIME_CODE_BITS, time_code);
    picture->pictures = time_code & LOW_BITS(TIME_PICTURE_BITS);
    time_code >>= TIME_PICTURE_BITS;
    picture->seconds = time_code & LOW_BITS(TIME_SECONDS_BITS);
    time_code >>= TIME_SECONDS_BITS + 1;
    picture->minutes = time_code & LOW_BITS(TIME_MINUTES_BITS);
    time_code >>= TIME_MINUTES_BITS;
    picture->hours = time_code & LOW_BITS(TIME_HOURS_BITS);
    time_code >>= TIME_HOURS_BITS;
    picture->drop_frame = time_code;
    TAKE_FLAG(mpv->closed_gop);
    TAKE_FLAG(mpv->broken_link);
    /* @hack type: end an address as an Sint32, where a Uint8 * would do */
    end = (Sint32)(cursor - AHEAD_BYTES) + ((nused + 7) >> 3);
    used = end - (Sint32)*data;
    *consumed += used;
    *size -= used;
    *data = (Uint8 *)end;
    return mpvdec_GoNextLayer(mpv, data, size, consumed, MPV_DELIM_PICTURE);
}

/* read the picture header at *data, and set mpv's decoders and makers for the
 * picture. it returns 0 where the picture's first slice follows, as
 * mpvdec_GoNextLayer finds it, and NO_LAYER where none does. where the
 * header's extra information runs past the data, it returns END_OF_INPUT with
 * *data at the data's end. the coding type must be 1 to 4, which the tables
 * take unchecked */
static Sint32 mpvdec_DecPsc(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                            Sint32 *consumed)
{
    struct mpv_picture *picture;
    struct mpv_motion *motion;
    /* @hack redundant: cursor, nused, lookahead, bits, code and used declared
     * register, where plain locals would do */
    register Uint8 *cursor;
    register Sint32 nused;
    register Uint32 lookahead;
    register Uint32 bits;
    Sint32 type;
    register Sint32 code;
    Sint32 decode;
    Sint32 extra;
    Sint32 end;
    register Sint32 used;

    picture = &mpv->picture;
    OPEN_HEADER(data);
    TAKE_FIELD(10, picture->temporal_ref);
    TAKE_FIELD(3, picture->coding_type);
    TAKE_FIELD(16, mpv->vbv_delay);
    type = picture->coding_type;
    if (type == PICTURE_P || type == PICTURE_B) {
        /* full_pel_forward_vector and forward_f_code, then the backward pair */
        motion = &mpv->forward;
        TAKE_FLAG(motion->full_pel);
        TAKE_FIELD(3, code);
        motion->r_size = --code;
        motion->shift = WORD_BITS - MOTION_BITS - code;
        motion->f = 1 << code;
        if (type == PICTURE_B) {
            motion = &mpv->backward;
            TAKE_FLAG(motion->full_pel);
            /* @hack spelling: TAKE_FIELD(3, code) written out, with bits >> 29
             * taken ahead of the test, where TAKE_FIELD would do */
            {
                Uint32 peek = bits >> 29;
                if (nused >= 29) {
                    nused -= 29;
                    if (nused != 0) {
                        bits |= lookahead >> (3 - nused);
                        code = bits >> 29;
                        bits = lookahead << nused;
                    } else {
                        code = peek;
                        bits = lookahead;
                    }
                    LOAD_WORD(lookahead);
                } else {
                    nused += 3;
                    code = peek;
                    bits <<= 3;
                }
            }
            motion->r_size = --code;
            motion->shift = WORD_BITS - MOTION_BITS - code;
            motion->f = 1 << code;
        }
    }
    decode = mpv->cond[MPV_COND_DECODE];
    mpv->decode_mb = decode_macroblock[type];
    mpv->skipped = skip_macroblock[decode][type];
    mpv->intra_blocks = decode_intra[decode];
    mpv->nonintra_blocks = decode_nonintra[decode];
    /* @bug memory: MPV_COND_DECODE 0 sets every maker to null: make_intra's
     * first row, at 0x8c17a1b0, and the other tables' alike. a picture's first
     * macroblock then calls one, an I picture's at 0x8c18d422 */
    mpv->intra_maker =
        make_intra[mpv->cond[MPV_COND_TEXTURE_ONLY]][decode][type];
    mpv->makers[MAKER_INDEX(MPVVLC_MB_BACKWARD)] = make_backward[decode][type];
    mpv->makers[MAKER_INDEX(MPVVLC_MB_FORWARD)] = make_forward[decode][type];
    mpv->makers[MAKER_INDEX(MPVVLC_MB_BACKWARD | MPVVLC_MB_FORWARD)] =
        make_bidirectional[decode][type];
    /* skip the extra information: a byte after each set extra bit, up to a
     * clear one */
    for (;;) {
        extra = (Sint32)bits < 0;
        if (extra == 0) {
            nused++;
            if (nused >= WORD_BITS) {
                nused -= WORD_BITS;
                cursor += WORD_BYTES;
            }
            break;
        }
        /* @hack spelling: SKIP_BITS(EXTRA_BITS) written out, its first byte
         * read through cursor++, where SKIP_BITS would do */
        nused += EXTRA_BITS;
        if (nused >= WORD_BITS) {
            nused -= WORD_BITS;
            bits = lookahead << nused;
            lookahead = (Sint8)*cursor++;
            lookahead <<= 8;
            lookahead |= *cursor;
            cursor = cursor + 1;
            lookahead <<= 8;
            lookahead |= *cursor;
            cursor = cursor + 1;
            lookahead <<= 8;
            lookahead |= *cursor;
            cursor = cursor + 1;
        } else {
            bits <<= EXTRA_BITS;
        }
        /* @hack type: cursor and *data added as Sint32s, where a pointer
         * difference would do */
        used = ((nused + 7) >> 3) + (Sint32)(cursor - AHEAD_BYTES);
        used -= (Sint32)*data;
        if (*size <= used) {
            *consumed += *size;
            *data += *size;
            *size = 0;
            return END_OF_INPUT;
        }
    }
    /* @hack type: end a byte count and then an address, as an Sint32, where a
     * pointer to the byte past the last bit would do */
    end = (nused + 7) >> 3;
    cursor -= AHEAD_BYTES;
    end += (Sint32)cursor;
    used = end - (Sint32)*data;
    *consumed += used;
    *size -= used;
    *data = (Uint8 *)end;
    return mpvdec_GoNextLayer(mpv, data, size, consumed, MPV_DELIM_FIRST_SLICE);
}

/* step to the next start code, past an extension's and then a user data's.
 * it returns 0 where the code it stops at is of a kind in mask. it returns
 * NO_LAYER where it isn't, or where none follows. *data is left at that
 * code */
static Sint32 mpvdec_GoNextLayer(struct mpv_obj *mpv, Uint8 **data,
                                 Sint32 *size, Sint32 *consumed, Sint32 mask)
{
    Sint32 kind = mpvdec_GoNextDelim(data, size, consumed);

    if (kind == 0) {
        return NO_LAYER;
    }
    if (kind == MPV_DELIM_EXTENSION) {
        *data += MPV_START_CODE_BYTES;
        *size -= MPV_START_CODE_BYTES;
        *consumed += MPV_START_CODE_BYTES;
        kind = mpvdec_GoNextDelim(data, size, consumed);
        if (kind == 0) {
            return NO_LAYER;
        }
    }
    if (kind == MPV_DELIM_USER_DATA) {
        *data += MPV_START_CODE_BYTES;
        *size -= MPV_START_CODE_BYTES;
        *consumed += MPV_START_CODE_BYTES;
        kind = mpvdec_GoNextDelim(data, size, consumed);
        if (kind == 0) {
            return NO_LAYER;
        }
    }
    if (!(kind & mask)) {
        return NO_LAYER;
    }
    return 0;
}

/* step to the first start code at or after *data, or to the data's end. it
 * returns the code's MPV_DELIM_ kind, or 0 for none */
static Sint32 mpvdec_GoNextDelim(Uint8 **data, Sint32 *size, Sint32 *consumed)
{
    Uint8 *delimiter;
    Sint32 kind;
    Sint32 skip;

    delimiter = MPV_SearchDelim(*data, *size, ANY_DELIMITER);
    if (delimiter == NULL) {
        kind = 0;
        skip = *size;
    } else {
        kind = MPV_CheckDelim(delimiter);
        skip = delimiter - *data;
    }
    *data += skip;
    *size -= skip;
    *consumed += skip;
    return kind;
}

Sint32 MPVDEC_DecPicture(struct mpv_obj *mpv, Uint8 *data, Sint32 size,
                         Sint32 *consumed)
{
    Sint32 status;
    Sint32 kind;

    *consumed = 0;
    for (;;) {
        status = mpvdec_Recover(mpv, ANY_DELIMITER, &data, &size, consumed);
        if (status != 0) {
            return MPVERR_SetCode(mpv, status);
        }
        kind = MPV_CheckDelim(data);
        if (!(kind & MPV_DELIM_SLICE)) {
            break;
        }
        mpvdec_DecSlice(mpv, &data, &size, consumed);
    }
    return 0;
}

/* return 0 where *data is at a start code of a kind in mask. otherwise count
 * a miss in mpv->err. with MPV_COND_RECOVER clear, it returns
 * MISSED_DELIMITER. with it set, it steps to the next start code of a kind in
 * mask. it counts the step in other_errors and returns 0. where none follows,
 * it returns END_OF_INPUT with *data at the data's end */
static Sint32 mpvdec_Recover(struct mpv_obj *mpv, Sint32 mask, Uint8 **data,
                             Sint32 *size, Sint32 *consumed)
{
    struct mpv_errinf *errors;
    Uint8 *delimiter;

    /* @hack spelling: the work nested under the test, with an else after each
     * return, where early returns would do */
    if (!(MPV_CheckDelim(*data) & mask)) {
        errors = &mpv->err;
        errors->delimiter_misses++;
        if (mpv->cond[MPV_COND_RECOVER] == 0) {
            return MISSED_DELIMITER;
        } else {
            delimiter = MPV_SearchDelim(*data, *size, mask);
            if (delimiter == NULL) {
                *consumed += *size;
                *data += *size;
                *size = 0;
                return END_OF_INPUT;
            } else {
                *consumed += delimiter - *data;
                *size -= delimiter - *data;
                *data = delimiter;
                errors->other_errors++;
            }
        }
    }
    return 0;
}

/* read the slice header at *data, then decode the slice's macroblocks with
 * mpv->decode_mb and return its result. the header's row sets mpv->index to
 * the macroblock before the row's first. the first address increment counts
 * from there. where the header's extra information runs past the data, it
 * returns END_OF_INPUT with *data at the data's end */
static Sint32 mpvdec_DecSlice(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                              Sint32 *consumed)
{
    Uint8 *cursor;
    Sint32 nused;
    /* @hack redundant: lookahead, bits and extra declared register, where plain
     * locals would do */
    register Uint32 lookahead;
    register Uint32 bits;
    Uint32 start_code;
    register Sint32 extra;
    Sint32 used;

    /* start_code takes the slice's start code whole. its last byte is the
     * slice's row, counted from 1 */
    cursor = (Uint8 *)((Uint32)*data & ~3UL);
    /* @hack type: nused taken from *data read again as an Sint32, where the
     * address cursor came from would do */
    nused = *(const Sint32 *)data - (Sint32)cursor;
    nused *= 8;
    LOAD_WORD(bits);
    bits <<= nused;
    LOAD_WORD(lookahead);
    if (nused != 0) {
        bits |= lookahead >> (32 - nused);
        start_code = bits;
        bits = lookahead << nused;
    } else {
        start_code = bits;
        bits = lookahead;
    }
    LOAD_WORD(lookahead);
    mpv->index = (((start_code & 0xff) - 1) * mpv->picture.mb_width) - 1;
    TAKE_FIELD(QUANTIZER_BITS, mpv->qscale);
    mpvdec_ResetMv(&mpv->forward);
    mpvdec_ResetMv(&mpv->backward);
    mpvdec_ResetDc(mpv);
    /* skip the extra information, as mpvdec_DecPsc does */
    for (;;) {
        extra = (Sint32)bits < 0;
        /* @hack redundant: an else after the break */
        if (extra == 0) {
            SKIP_BITS(1);
            break;
        } else {
            SKIP_BITS(EXTRA_BITS);
            /* @hack type: cursor and *data added as Uint32s, where a pointer
             * difference would do */
            used = (nused + 7) >> 3;
            used += (Uint32)(cursor - AHEAD_BYTES);
            used -= (Uint32)*data;
            if (*size <= used) {
                *consumed += *size;
                *data += *size;
                *size = 0;
                return END_OF_INPUT;
            }
        }
    }
    SAVE_READER(mpv);
    return mpv->decode_mb(mpv, data, size, consumed);
}

/* PSGSFD02, the macroblock decoders' section, runs 0x6000 bytes, the size
 * MPVASEC_CheckSectionSiz wants of it. a reservation of unwritten bytes opens
 * each 8 KB of it. code fills the rest of each: mpvdec_DecIpicMb the first,
 * mpvdec_DecPpicMb the second, and mpvdec_ResetMv to mpvdec_DecBpicMb the
 * third.
 *
 * each decoder reads a slice's macroblocks from the reader mpvdec_DecSlice
 * leaves in mpv. it stops at the next start code, or at bits that are no code
 * it expects. an increment past the picture's last macroblock stops it too.
 * it then
 * steps *data to the next start code and returns 0 */
#pragma section SFD02

/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* SHC writes this block ahead of its .SECTION line for mpvdec_DecIpicMb, so
 * the block opens PSGSFD02 itself */
/* @asm open: no inspect rule reads the bytes its .RES.B reserves */
#pragma asm
        .SECTION    PSGSFD02,CODE,ALIGN=4
        .RES.B      7704
#pragma endasm
#endif
#endif
/* clang-format on */

MPV_CODE(PSGSFD02)
static Sint32 mpvdec_DecIpicMb(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                               Sint32 *consumed)
{
    /* @hack redundant: cursor, nused, bits, lookahead and entry declared
     * register, where plain locals would do */
    register Uint8 *cursor;
    register Sint32 nused;
    register Uint32 bits;
    register Uint32 lookahead;
    Uint32 code;
    register Uint32 entry;
    Sint32 length;
    Sint32 token;
    Sint32 start;
    Sint32 increment;
    Uint8 *end;
    Sint32 used;

    LOAD_READER(mpv);
    /* @hack redundant: a return of data != NULL in a block no path runs */
    if (0) {
        return data != NULL;
    }
    for (;;) {
        PEEK_FIELD(START_CODE_ZEROS, code);
        if (code == 0) {
            break;
        }
        start = mpv->index;
        for (;;) {
            PEEK_FIELD(12, code);
            if ((code >> 8) == 0) {
                entry = mpvvlc_mbai_i_0[code];
            } else {
                entry = mpvvlc_mbai_i_1[code >> 6];
            }
            length = entry & MPVVLC_MBAI_LENGTH;
            SKIP_BITS(length);
            /* @hack type: the increment taken as (Uint8)(entry >> 2) >> 2,
             * where (entry >> 4) & 63 would do */
            token = (Uint8)(entry >> 2) >> 2;
            if (token == MPVVLC_MBAI_STUFFING) {
                continue;
            }
            if (token == MPVVLC_MBAI_ESCAPE) {
                mpv->index += ESCAPE_INCREMENT;
                continue;
            }
            if (token == MPVVLC_MBAI_INVALID) {
                increment = INVALID_INCREMENT;
            } else {
                mpv->index += token;
                mpv->mb_type = entry >> MPVVLC_MBAI_FLAGS_SHIFT;
                if (mpv->index > mpv->last_index) {
                    increment = INVALID_INCREMENT;
                } else {
                    increment = mpv->index - start;
                }
            }
            if (increment == INVALID_INCREMENT) {
                goto finish;
            }
            break;
        }
        if (mpv->mb_type & MPVVLC_MB_QUANT) {
            TAKE_FIELD(QUANTIZER_BITS, mpv->qscale);
        }
        SAVE_READER(mpv);
        mpv->intra_blocks(mpv);
        mpv->intra_maker(mpv);
        LOAD_READER(mpv);
    }
finish:
    /* @hack spelling: the bytes counted before cursor steps back, in a block
     * of their own, where one expression for end would do */
    {
        Sint32 bytes = (nused + 7) >> 3;
        cursor -= AHEAD_BYTES;
        end = cursor + bytes;
    }
    used = end - *data;
    *consumed += used;
    *size -= used;
    *data = end;
    mpvdec_GoNextDelim(data, size, consumed);
    return 0;
}

/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the bytes its .RES.B reserves */
#pragma asm
        .RES.B      7284
#pragma endasm
#endif
#endif
/* clang-format on */

/* read a P or B picture's macroblock_address_increment through the table pair
 * zero and one, in the caller's locals. stuffing is skipped, and each escape
 * adds ESCAPE_INCREMENT to mpv->index. mpv->mb_type takes the type flags the
 * entry holds, which are 0 where it takes no type code. entry ends with the
 * increment. bits that are no code, and an increment past the picture's last
 * macroblock, go to picture_done */
#define PREDICTED_INCREMENT(zero, one)                                         \
    {                                                                          \
        start = mpv->index;                                                    \
    read_increment:                                                            \
        PEEK_FIELD(11, code);                                                  \
        if ((code >> 7) == 0) {                                                \
            entry = (zero)[code];                                              \
        } else {                                                               \
            entry = (one)[code >> 6];                                          \
        }                                                                      \
        length = entry & MPVVLC_MBAI_LENGTH;                                   \
        SKIP_BITS(length);                                                     \
        /* @hack redundant: a goto in a block no path runs */                  \
        if (0) {                                                               \
            goto picture_done;                                                 \
        }                                                                      \
        /* @hack type: the increment taken as (Uint8)(entry >> 2) >> 2, where  \
         * (entry >> 4) & 63 would do */                                       \
        token = (Uint8)(entry >> 2) >> 2;                                      \
        if (token == MPVVLC_MBAI_STUFFING) {                                   \
            goto read_increment;                                               \
        }                                                                      \
        if (token == MPVVLC_MBAI_ESCAPE) {                                     \
            mpv->index += ESCAPE_INCREMENT;                                    \
            goto read_increment;                                               \
        }                                                                      \
        if (token == MPVVLC_MBAI_INVALID) {                                    \
            goto bad_increment;                                                \
        }                                                                      \
        mpv->index += token;                                                   \
        entry >>= MPVVLC_MBAI_FLAGS_SHIFT;                                     \
        mpv->mb_type = entry;                                                  \
        if (mpv->index <= mpv->last_index) {                                   \
            goto valid_increment;                                              \
        }                                                                      \
        /* @hack spelling: INVALID_INCREMENT stored in entry and tested, where \
         * a goto to picture_done at each failure would do */                  \
    bad_increment:                                                             \
        entry = (Uint32)INVALID_INCREMENT;                                     \
        goto end_increment;                                                    \
    valid_increment:                                                           \
        entry = mpv->index - start;                                            \
    end_increment:                                                             \
        if (entry == (Uint32)INVALID_INCREMENT) {                              \
            goto picture_done;                                                 \
        }                                                                      \
    }

/* mpv->cbp holds a coded_block_pattern entry's high byte as its own top
 * byte, from bit CBP_PATTERN_SHIFT */
#define CBP_PATTERN_SHIFT 24

/* read coded_block_pattern into mpv->cbp, or 0 where mpv->mb_type has no
 * MPVVLC_MB_PATTERN. an entry's length is under 16, so ~15UL clears it */
#define READ_PATTERN()                                                         \
    {                                                                          \
        if (mpv->mb_type & MPVVLC_MB_PATTERN) {                                \
            PEEK_FIELD(9, code);                                               \
            packed = mpvvlc_cbp[code];                                         \
            mpv->cbp = (packed & ~15UL)                                        \
                       << (CBP_PATTERN_SHIFT - MPVVLC_CBP_PATTERN_SHIFT);      \
            length = packed & MPVVLC_CBP_LENGTH;                               \
            SKIP_BITS(length);                                                 \
        } else {                                                               \
            mpv->cbp = 0;                                                      \
        }                                                                      \
    }

MPV_CODE(PSGSFD02)
static Sint32 mpvdec_DecPpicMb(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                               Sint32 *consumed)
{
    /* @hack redundant: cursor, nused, bits, lookahead, entry, length, token,
     * start and first declared register, where plain locals would do */
    register Uint8 *cursor;
    register Sint32 nused;
    register Uint32 bits;
    register Uint32 lookahead;
    Uint32 code;
    Uint32 packed;
    register Uint32 entry;
    register Sint32 length;
    register Sint32 token;
    register Sint32 start;
    register Bool first;
    Uint32 end;
    Uint32 span;
    Sint32 used;

    first = TRUE;
    LOAD_READER(mpv);
next_macroblock:
    PEEK_FIELD(START_CODE_ZEROS, entry);
    if (entry == 0) {
        goto picture_done;
    }
    PREDICTED_INCREMENT(mpvvlc_mbai_p_0, mpvvlc_mbai_p_1);
    /* an increment over 1, past the slice's first macroblock, skips the
     * macroblocks between */
    if (!first && entry > 1) {
        mpv->skipped(mpv, entry);
        mpvdec_ResetMv(&mpv->forward);
        mpvdec_ResetDc(mpv);
    }
    if (!(mpv->mb_type & MPVVLC_MB_TYPED)) {
        PEEK_FIELD(5, code);
        packed = mpvvlc_p_mbtype[code];
        mpv->mb_type = packed >> MPVVLC_MBTYPE_FLAGS_SHIFT;
        length = packed & MPVVLC_MBTYPE_LENGTH;
        SKIP_BITS(length);
    }
    if (mpv->mb_type & MPVVLC_MB_QUANT) {
        TAKE_FIELD(QUANTIZER_BITS, mpv->qscale);
    }
    if (mpv->mb_type & MPVVLC_MB_FORWARD) {
        SAVE_READER(mpv);
        entry = mpvdec_MotionSub(mpv, &mpv->forward, &mpv->forward.x,
                                 &mpv->forward.predict_x);
        code = mpvdec_MotionSub(mpv, &mpv->forward, &mpv->forward.y,
                                &mpv->forward.predict_y);
        /* @hack spelling: LOAD_READER written out, with entry |= code before
         * its load of cursor, where LOAD_READER would do */
        bits = mpv->bits;
        lookahead = mpv->next;
        nused = mpv->nused;
        entry |= code;
        cursor = (Uint8 *)mpv->rd;
        if (entry != 0) {
            goto picture_done;
        }
    } else {
        mpvdec_ResetMv(&mpv->forward);
    }
    READ_PATTERN();
    SAVE_READER(mpv);
    if (mpv->mb_type & MPVVLC_MB_INTRA) {
        mpv->intra_blocks(mpv);
        mpv->intra_maker(mpv);
    } else {
        /* a P picture predicts each other macroblock forward, by the zero
         * vector where it codes none */
        if (mpv->cbp != 0) {
            mpv->nonintra_blocks(mpv);
        }
        mpv->makers[MAKER_INDEX(MPVVLC_MB_FORWARD)](mpv);
        mpvdec_ResetDc(mpv);
    }
    LOAD_READER(mpv);
    first = FALSE;
    goto next_macroblock;
picture_done:
    /* @hack type: end a byte count and then an address, as a Uint32, where a
     * pointer to the byte past the last bit would do */
    end = (nused + 7) >> 3;
    cursor -= AHEAD_BYTES;
    end += (Uint32)cursor;
    /* @hack spelling: the bytes read through span, where used would do */
    span = end - (Uint32)*data;
    used = span;
    *consumed += used;
    *size -= used;
    *data = (Uint8 *)end;
    mpvdec_GoNextDelim(data, size, consumed);
    /* @hack redundant: a return of entry in a block no path runs */
    if (0) {
        return entry;
    }
    return 0;
}

/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the bytes its .RES.B reserves */
#pragma asm
        .RES.B      6844
#pragma endasm
#endif
#endif
/* clang-format on */

MPV_CODE(PSGSFD02)
static void mpvdec_ResetMv(struct mpv_motion *motion)
{
    motion->predict_x = 0;
    motion->predict_y = 0;
    motion->x = 0;
    motion->y = 0;
}

MPV_CODE(PSGSFD02)
static void mpvdec_ResetDc(struct mpv_obj *mpv)
{
    mpv->dc_pred.y = DC_ORIGIN;
    mpv->dc_pred.cr = DC_ORIGIN;
    mpv->dc_pred.cb = DC_ORIGIN;
}

/* read one component of a motion vector from mpv's reader into *vector. the
 * component is *predictor plus the delta the stream codes, wrapped into
 * MOTION_BITS + r_size signed bits. *predictor takes it. *vector takes it
 * doubled where motion->full_pel is set. it returns 0, or -1 for bits that are
 * no motion code. those leave mpv's reader, *vector and *predictor as they
 * were */
MPV_CODE(PSGSFD02)
static Sint32 mpvdec_MotionSub(struct mpv_obj *mpv, struct mpv_motion *motion,
                               Sint32 *vector, Sint32 *predictor)
{
    Uint8 *cursor;
    Sint32 nused;
    Uint32 bits;
    Uint32 lookahead;
    Uint32 code;
    Uint32 entry;
    Sint32 delta;
    Sint32 residual_bits;
    Sint32 shift;
    Uint32 residual;
    Sint32 error;

    LOAD_READER(mpv);
    error = 0;
    residual_bits = motion->r_size;
    shift = motion->shift;
    PEEK_FIELD(11, code);
    if ((code >> 7) == 0) {
        entry = mpvvlc_motion_0[code];
    } else {
        entry = mpvvlc_motion_1[code >> 6];
    }
    /* @hack redundant: a sum of 23 reads of entry in a block no path runs */
    if (0) {
        error = entry + entry + entry + entry + entry + entry + entry + entry +
                entry + entry + entry + entry + entry + entry + entry + entry +
                entry + entry + entry + entry + entry + entry + entry;
    }
    delta = (Sint8)entry;
    if (delta == MPVVLC_MOTION_INVALID) {
        error = -1;
    } else {
        entry = (Uint8)(entry >> MPVVLC_MOTION_LENGTH_SHIFT);
        SKIP_BITS(entry);
        if (delta == 0) {
            *vector = *predictor;
        } else {
            if (residual_bits != 0) {
                TAKE_FIELD(residual_bits, residual);
                delta <<= residual_bits;
                residual = motion->f - 1 - residual;
                if (delta > 0) {
                    delta -= residual;
                } else {
                    delta += residual;
                }
            }
            delta += *predictor;
            *vector = (delta << shift) >> shift;
            *predictor = *vector;
        }
        if (motion->full_pel) {
            *vector = (Uint32)*vector << 1;
        }
    }
    SAVE_READER(mpv);
    return error;
}

MPV_CODE(PSGSFD02)
static Sint32 mpvdec_DecBpicMb(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                               Sint32 *consumed)
{
    /* @hack redundant: cursor, nused, bits, lookahead, entry, length, start and
     * first declared register, where plain locals would do */
    register Uint8 *cursor;
    register Sint32 nused;
    register Uint32 bits;
    register Uint32 lookahead;
    Uint32 code;
    Uint32 packed;
    register Uint32 entry;
    register Sint32 length;
    Sint32 token;
    register Sint32 start;
    register Bool first;
    Uint32 end;
    Uint32 span;
    Sint32 used;

    first = TRUE;
    LOAD_READER(mpv);
    /* @hack redundant: a return of data != NULL in a block no path runs */
    if (0) {
        return data != NULL;
    }
next_macroblock:
    PEEK_FIELD(START_CODE_ZEROS, entry);
    if (entry == 0) {
        goto picture_done;
    }
    PREDICTED_INCREMENT(mpvvlc_mbai_b_0, mpvvlc_mbai_b_1);
    /* an increment over 1, past the slice's first macroblock, skips the
     * macroblocks between. a B picture keeps its motion predictions over
     * them */
    if (!first && entry > 1) {
        mpv->skipped(mpv, entry);
        mpvdec_ResetDc(mpv);
    }
    if (!(mpv->mb_type & MPVVLC_MB_TYPED)) {
        PEEK_FIELD(6, code);
        packed = mpvvlc_b_mbtype[code];
        mpv->mb_type = packed >> MPVVLC_MBTYPE_FLAGS_SHIFT;
        length = packed & MPVVLC_MBTYPE_LENGTH;
        SKIP_BITS(length);
    }
    if (mpv->mb_type & MPVVLC_MB_QUANT) {
        TAKE_FIELD(QUANTIZER_BITS, mpv->qscale);
    }
    if (mpv->mb_type & MPVVLC_MB_FORWARD) {
        SAVE_READER(mpv);
        entry = mpvdec_MotionSub(mpv, &mpv->forward, &mpv->forward.x,
                                 &mpv->forward.predict_x);
        code = mpvdec_MotionSub(mpv, &mpv->forward, &mpv->forward.y,
                                &mpv->forward.predict_y);
        LOAD_READER(mpv);
        entry |= code;
        if (entry != 0) {
            goto picture_done;
        }
    }
    if (mpv->mb_type & MPVVLC_MB_BACKWARD) {
        SAVE_READER(mpv);
        entry = mpvdec_MotionSub(mpv, &mpv->backward, &mpv->backward.x,
                                 &mpv->backward.predict_x);
        code = mpvdec_MotionSub(mpv, &mpv->backward, &mpv->backward.y,
                                &mpv->backward.predict_y);
        LOAD_READER(mpv);
        entry |= code;
        if (entry != 0) {
            goto picture_done;
        }
    }
    READ_PATTERN();
    SAVE_READER(mpv);
    if (mpv->mb_type & MPVVLC_MB_INTRA) {
        mpv->intra_blocks(mpv);
        mpv->intra_maker(mpv);
        mpvdec_ResetMv(&mpv->forward);
        mpvdec_ResetMv(&mpv->backward);
    } else {
        if (mpv->cbp != 0) {
            mpv->nonintra_blocks(mpv);
        }
        mpv->makers[0] = mpv->makers[MAKER_INDEX(mpv->mb_type)];
        mpv->makers[0](mpv);
        mpvdec_ResetDc(mpv);
    }
    LOAD_READER(mpv);
    first = FALSE;
    goto next_macroblock;
picture_done:
    /* @hack type: end a byte count and then an address, as a Uint32, where a
     * pointer to the byte past the last bit would do */
    end = (nused + 7) >> 3;
    cursor -= AHEAD_BYTES;
    end += (Uint32)cursor;
    /* @hack spelling: the bytes read through span, where used would do */
    span = end - (Uint32)*data;
    used = span;
    *consumed += used;
    *size -= used;
    *data = (Uint8 *)end;
    mpvdec_GoNextDelim(data, size, consumed);
    /* @hack redundant: a return of entry in a block no path runs */
    if (0) {
        return entry;
    }
    return 0;
}

#pragma section

/* D pictures, which code only their blocks' DC coefficients, aren't decoded.
 * it returns 0, and leaves *data, *size and *consumed as they were */
static Sint32 mpvdec_DecDpicMb(struct mpv_obj *mpv, Uint8 **data, Sint32 *size,
                               Sint32 *consumed)
{
    /* @bug logic: a D picture's first slice, where the data holds its header,
     * hangs MPVDEC_DecPicture. this returns at 0x8c1083f8 with *data still at
     * the slice's start code. MPVDEC_DecPicture then decodes the slice again,
     * at 0x8c108126 */
    return 0;
}
