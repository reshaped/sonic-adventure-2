/* each direction's dictionary is a tree of strings, a node each. a node's
 * string is its parent's and then its symbol. node N6 + b holds the byte b
 * alone. nodes N5 to N2 - 1 hold the longer strings. each node links to its
 * parent, its first child and its next sibling. the parent, dependant and
 * rbrother tables hold those three links. N1, N2 and N5 to N7 are V.42bis's
 * parameters, which raz sets */
#include <sg_xpt.h>

#include "bisdef.h"
#include "command.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "v42.h"
#include "v42bis.h"
#include "v42def.h"

/* a call codes nothing unless more than TM_ROOM bytes of TM are free */
#define TM_ROOM 300
#define ESCAPE_STEP 51
/* the codeword width, CC2 and DC2, starts at FIRST_WIDTH bits.
 * FIRST_THRESHOLD, where CC3 and DC3 start, is the first codeword too wide
 * for it */
#define FIRST_WIDTH 9
#define FIRST_THRESHOLD (1 << FIRST_WIDTH)
/* the most bytes the decoder puts in one of the host's buffers */
#define DECODER_FILL_MAX (COM_BUFFER_SIZE - 2)
/* stat_delay in compressed and in transparent mode. the coder checks its mode
 * at a string's end once n_byte_in passes stat_delay */
#define COMPRESSED_STAT_DELAY 256
#define TRANSPARENT_STAT_DELAY 64
/* the coder changes mode once the other would save more than MODE_MARGIN
 * bits */
#define MODE_MARGIN 16
/* encryp_cnt and decryp_cnt start at KEY_START. encrypt and decrypt step
 * their count before each read of key_string. each one's first read then
 * takes key_string[KEY_START + 1] */
#define KEY_START 3
/* the steps of the cipher's warm-up */
#define WARMUP_STEPS 20
/* scram_q holds the bytes scrambler put out, the last in its low byte.
 * scrambler XORs each byte with scram_q shifted right by FIRST_TAP, and by
 * FIRST_TAP + TAP_GAP. descrambler takes the same bits of descram_q, the bytes
 * it took in, so it undoes scrambler */
#define FIRST_TAP 6
#define TAP_GAP 3

/* the commands of transparent mode, each the byte after the escape
 * character */
enum transparent_command { ENTER_COMPRESSED, LITERAL_ESCAPE, RESET_DICTIONARY };
/* the control codewords of compressed mode, and their count */
enum compressed_command {
    ENTER_TRANSPARENT,
    FLUSH_CODE,
    EXTEND_CODE,
    CONTROL_CODEWORDS
};

union modem_compression_storage common_buffer;

static void v42bis_coder(void);
static void output_transparent_word(void);
static void flush_v42bis_coder(void);
static void check_mode(void);
static void search_dictionary(void);
static void add_node_coder(void);
static char find_c_left_brother(Uint16 code, Uint16 *previous);
static void v42bis_decoder(void);
static void decoder_transparent_mode(void);
static void decoder_search_dictionary_tm(void);
static void decoder_compressed_mode(void);
static void data_codeword_rx_cm(void);
static char check_decoder_tree(void);
static void add_node_decoder(void);
static char find_d_left_brother(Uint16 code, Uint16 *previous);
static void write_codeword_to_tm(Uint16 code);
static void check_codeword_size(Uint16 code);
static void write_to_tm(Uint8 byte);
static void coder_octet_alignment(void);
static char read_rrbyte_to_codeword(void);
static void write_to_dte(Uint8 byte);
static void init_coder(void);
static void init_decoder(void);
static void init_encrypt(void);
static Uint8 encrypt(Uint8 byte);
static Uint8 decrypt(Uint8 byte);
static Uint8 scrambler(Uint8 byte);
static Uint8 descrambler(Uint8 byte);

/* @hack type: encode a char, where a Bool would do */
void v42bis(char encode)
{
    rr_cnt_shadow = RR_CNT;
    if (encode == 0) {
        if (p0 & P0_DECODER) {
            if ((BMR(23) & BMR23_LINE_BREAK) != 0 &&
                RxBreakPointer[0] == RR_READ) {
                vxd_dte_tx_brk(get_EC_brk_length(), TRUE);
            } else if (vxd_dte_set_tx_data(&decomp_out_pntr_bak) >=
                       COM_BUFFER_SIZE) {
                v42bis_decoder();
            }
        }
    } else {
        /* @hack spelling: the test of TM's room inside the test of p0, where
         * && would do */
        if (p0 & P0_CODER) {
            if (tm_cnt_shadow < tm_lnt - TM_ROOM) {
                tm_cnt_temp_temp = tm_cnt_shadow - tm_cnt_temp;
                if (tm_cnt_temp_temp < tx_max_packet_size) {
                    v42bis_coder();
                    /* flush the string in progress where vxd_dte_get_rx_data
                     * returns 0, or where a break from the host waits. a break
                     * of OIS1_BREAK_IN_SEQUENCE goes in sequence with the
                     * data. it waits until the coder has coded each byte it
                     * took from the host. the call's pointer and count are
                     * scratch: v42bis sets tm_cnt_temp_temp afresh before it
                     * tests it again */
                    if (flush_fl != 0 &&
                        (vxd_dte_get_rx_data(&comp_in_pntr,
                                             &tm_cnt_temp_temp) == 0 ||
                         ((BMR(25) & BMR25_BREAK_PENDING) != 0 &&
                          ((OIS(1) & OIS1_BREAK_TYPE) !=
                               OIS1_BREAK_IN_SEQUENCE ||
                           comp_in_cntr_bak == 0)))) {
                        flush_fl = 0;
                        flush_v42bis_coder();
                    }
                }
            }
        }
    }
    get_v42bis_out = 0;
    TM_CNT = tm_cnt_shadow;
}

static void v42bis_coder(void)
{
    Uint8 escape;

    if (!comp_in_cntr_bak) {
        if (BMR(25) & BMR25_BREAK_PENDING) {
            return;
        }
        vxd_dte_get_rx_data(&comp_in_pntr_bak, &comp_in_cntr);
        comp_in_cntr_bak = comp_in_cntr;
        if (comp_in_cntr_bak == 0) {
            return;
        }
    }
    while (tm_cnt_temp_temp < tx_max_packet_size) {
        if (get_v42bis_out) {
            get_v42bis_out = 0;
            v42_empty_rx();
        }
        if (comp_in_cntr_bak == 0) {
            if (comp_in_cntr != 0) {
                vxd_dte_rx_data_done();
            }
            if (BMR(25) & BMR25_BREAK_PENDING) {
                return;
            }
            vxd_dte_get_rx_data(&comp_in_pntr_bak, &comp_in_cntr);
            comp_in_cntr_bak = comp_in_cntr;
            if (comp_in_cntr_bak == 0) {
                return;
            }
        }
        byte_to_encode = *comp_in_pntr_bak++;
        comp_in_cntr_bak--;
        n_byte_in++;
        flush_fl = 1;
        search_dictionary();
        if (!flushing_coder_data) {
            if (end_of_string && c_transparent == 0) {
                if (cw_ready >= CC3) {
                    check_codeword_size(cw_ready);
                }
                write_codeword_to_tm(cw_ready);
            }
        } else {
            flushing_coder_data = 0;
            if (end_of_string == 0) {
                nptr = N6 + byte_to_encode;
                current_string_length = 1;
                last_node_created = 0;
            }
        }
        if (end_of_string) {
            end_of_string = 0;
            n_codeword_out += CC2;
            if (n_byte_in > stat_delay) {
                check_mode();
            }
            if (update_cdic) {
                add_node_coder();
                nptr = N6 + byte_to_encode;
                current_string_length = 1;
            }
        }
        if (c_transparent) {
            output_transparent_word();
        }
        /* @hack spelling: c_esc_char read once into escape, where the test and
         * the step would each read it */
        escape = c_esc_char;
        if (byte_to_encode == escape) {
            c_esc_char = escape + ESCAPE_STEP;
        }
    }
    if (comp_in_cntr_bak == 0) {
        vxd_dte_rx_data_done();
        comp_in_cntr = 0;
    }
}

static void output_transparent_word(void)
{
    write_to_tm(byte_to_encode);
    if (byte_to_encode == c_esc_char) {
        write_to_tm(LITERAL_ESCAPE);
    }
}

static void flush_v42bis_coder(void)
{
    if (!c_transparent) {
        check_codeword_size(nptr);
        write_codeword_to_tm(nptr);
        write_codeword_to_tm(FLUSH_CODE);
        coder_octet_alignment();
        flushing_coder_data = 1;
    }
}

void reset_v42bis_coder(void)
{
    if (!c_transparent) {
        if (flush_fl) {
            flush_fl = 0;
            flush_v42bis_coder();
        }
        write_codeword_to_tm(ENTER_TRANSPARENT);
        coder_octet_alignment();
    }
    write_to_tm(c_esc_char);
    write_to_tm(RESET_DICTIONARY);
    init_coder();
}

/* choose the mode that sends fewer bits since the last check. n_byte_in bytes
 * take 8 bits each as they are, and n_codeword_out bits as codewords. with
 * encryption set, the coder enters compressed mode and stays there */
static void check_mode(void)
{
    n_of_bytes_in += n_byte_in;
    if (n_of_bytes_in >= RATIO_INPUT) {
        last_nb_out = n_of_bytes_out;
        n_of_bytes_in = 0;
        n_of_bytes_out = 0;
    }
    if (encryption) {
        if (c_transparent) {
            c_transparent = 0;
            stat_delay = COMPRESSED_STAT_DELAY;
            write_to_tm(c_esc_char);
            write_to_tm(ENTER_COMPRESSED);
            n_byte_in = 0;
            n_codeword_out = 0;
        }
        /* @bug logic: in compressed mode under encryption, the return leaves
         * n_byte_in as it was. n_byte_in then stays past stat_delay, so a
         * check comes at every string's end. each adds the bytes since the
         * switch to n_of_bytes_in again. a count in last_nb_out then covers
         * fewer than RATIO_INPUT bytes in, so cmd_GET_STAT's
         * compression_ratio comes out too high. check_mode is at 0x0caffbec,
         * and n_byte_in at 0x0cb78d7a */
        return;
    }
    if (c_transparent) {
        if (n_byte_in * 8 > n_codeword_out + MODE_MARGIN) {
            c_transparent = 0;
            stat_delay = COMPRESSED_STAT_DELAY;
            write_to_tm(c_esc_char);
            write_to_tm(ENTER_COMPRESSED);
        }
    } else {
#ifdef NONMATCHING
        /* this side tests the condition the empty arm below negates, the plain
         * form the hack tag gives */
        if (n_byte_in * 8 < n_codeword_out - MODE_MARGIN) {
#else
        /* @hack spelling: an empty arm under the test, where the negated test
         * over the else's statements would do */
        if (n_byte_in * 8 >= n_codeword_out - MODE_MARGIN) {
        } else {
#endif
            stat_delay = TRANSPARENT_STAT_DELAY;
            c_transparent = 1;
            write_codeword_to_tm(ENTER_TRANSPARENT);
            coder_octet_alignment();
        }
    }
    n_byte_in = 0;
    n_codeword_out = 0;
}

static void search_dictionary(void)
{
    Uint16 prefix;
    /* @hack redundant: register on code, symbols, next and byte */
    register unsigned int code;
    register Uint8 *symbols;
    register Uint16 *next;
    register unsigned int byte;

    update_cdic = 0;
    /* @hack spelling: prefix set inside the subscript, where a statement of
     * its own would do */
    code = c_dependant[prefix = nptr];
    /* @hack spelling: gotos into the arm of the test after the loop, where a
     * loop that stops at 0 and one test after it would do */
    if (code == 0) {
        if (current_string_length == 0) {
            nptr = N6 + byte_to_encode;
            current_string_length = 1;
            return;
        }
        goto no_match;
    } else {
        symbols = c_symbol;
        next = c_rbrother;
        byte = byte_to_encode;
        while (symbols[code] != byte) {
            code = next[code];
            if (code == 0) {
                goto no_match;
            }
        }
    }
    if (code == 0) {
    no_match:
        cw_ready = prefix;
        end_of_string = 1;
        update_cdic = 1;
        return;
    }
    /* the peer's decoder adds last_node_created only once the next codeword
     * comes, so no codeword may name it yet. the string ends before it */
    if (code == last_node_created) {
        cw_ready = prefix;
        nptr = N6 + byte_to_encode;
        current_string_length = 1;
        end_of_string = 1;
        last_node_created = 0;
        /* @bug logic: n_codeword_out takes CC2 here and again in
         * v42bis_coder, for the one codeword that ends the string. check_mode
         * then counts CC2 more bits than the string's codeword takes.
         * search_dictionary is at 0x0caffc90, and v42bis_coder at
         * 0x0caff930 */
        n_codeword_out += CC2;
    } else {
        current_string_length++;
        nptr = code;
    }
}

/* add the string at nptr and then byte_to_encode at CC1, unless nptr's string
 * is N7 bytes already. CC1 then moves on to the next node without children,
 * wrapping from N2 - 1 to N5. that node drops out of its parent's list of
 * children, ready for the next add */
static void add_node_coder(void)
{
    /* @hack redundant: register on code, children and last */
    register unsigned int code;
    Uint16 previous;
    register Uint16 *children;
    /* @hack spelling: N2_moins1 and N5 read once into last and first, where
     * the tests would read them, as add_node_decoder's do */
    register unsigned int last;
    unsigned int first;

    if (current_string_length < N7) {
        code = CC1;
        children = c_dependant;
        c_rbrother[code] = children[nptr];
        children[nptr] = code;
        c_symbol[code] = byte_to_encode;
        c_parent[code] = nptr;
        children[code] = 0;
        last_node_created = code;
        last = N2_moins1;
        first = N5;
        code++;
        if (code > last) {
            code = first;
        }
        while (children[code]) {
            code++;
            if (code > last) {
                code = first;
            }
        }
        CC1 = code;
        if (find_c_left_brother(CC1, &previous)) {
            c_rbrother[previous] = c_rbrother[CC1];
        } else {
            c_dependant[c_parent[CC1]] = c_rbrother[CC1];
        }
    } else {
        last_node_created = 0;
    }
}

/* define function, which finds the sibling before code in its parent's list
 * of children. it sets *previous to that sibling and returns 1, or returns 0
 * where none comes before code */
#define FIND_PREVIOUS_SIBLING(function, next_links, children, parents)         \
    /* @hack type: a char result, where a Bool would do */                     \
    static char function(Uint16 code, Uint16 *previous)                        \
    {                                                                          \
        /* @hack redundant: register on wanted and next */                     \
        register Uint16 wanted;                                                \
        register Uint16 *next;                                                 \
        next = (next_links);                                                   \
        wanted = code;                                                         \
        code = (children)[(parents)[wanted]];                                  \
        while (code != 0) {                                                    \
            if (wanted == next[code]) {                                        \
                *previous = code;                                              \
                return 1;                                                      \
            }                                                                  \
            code = next[code];                                                 \
        }                                                                      \
        return 0;                                                              \
    }

FIND_PREVIOUS_SIBLING(find_c_left_brother, c_rbrother, c_dependant, c_parent)

static void v42bis_decoder(void)
{
    rr_cnt_temp = rr_cnt_shadow;
    while ((!(BMR(23) & BMR23_LINE_BREAK) || RxBreakPointer[0] != RR_READ) &&
           rr_cnt_shadow != 0) {
        if (get_v42bis_out) {
            get_v42bis_out = 0;
            v42_empty_rx();
        }
        if (decomp_out_cntr > decomp_out_max) {
            vxd_dte_tx_data_done(decomp_out_cntr);
            decomp_out_cntr = 0;
            if (rr_cnt_temp - rr_cnt_shadow > tx_max_packet_size) {
                return;
            }
            if (vxd_dte_set_tx_data(&decomp_out_pntr_bak) < COM_BUFFER_SIZE) {
                return;
            }
        }
        if (d_transparent) {
            decoder_transparent_mode();
        } else {
            decoder_compressed_mode();
        }
    }
    if (decomp_out_cntr) {
        vxd_dte_tx_data_done(decomp_out_cntr);
        decomp_out_cntr = 0;
    }
}

static void decoder_transparent_mode(void)
{
    if (encryption != 0) {
        cw_ready = decrypt(*RR_READ++);
    } else {
        cw_ready = *RR_READ++;
    }
    RR_CNT--;
    rr_cnt_shadow--;
    if (RR_READ > RR_TOPP) {
        RR_READ = RR_BUFFER;
    }
    /* @hack spelling: a goto to the output after the return, where an else arm
     * holding it would do */
    if (!d_escape_mode) {
        if (cw_ready == d_esc_char) {
            d_escape_mode = 1;
        } else {
            goto output;
        }
    } else {
        d_escape_mode = 0;
        if (cw_ready == ENTER_COMPRESSED) {
            d_bit_ptr = 0;
            rr_word_temp = 0;
            d_check_tree = 1;
            d_transparent = 0;
        } else if (cw_ready == LITERAL_ESCAPE) {
            cw_ready = d_esc_char;
            write_to_dte(d_esc_char);
            decoder_search_dictionary_tm();
            entering_decoder_tr_mode = 0;
            d_esc_char += ESCAPE_STEP;
        } else if (cw_ready == RESET_DICTIONARY) {
            init_decoder();
        }
    }
    return;
output:
    write_to_dte(cw_ready);
    decoder_search_dictionary_tm();
    entering_decoder_tr_mode = 0;
}

static void decoder_search_dictionary_tm(void)
{
    Uint16 code;

    /* @hack spelling: a goto into the loop, and d_dependant[d_nptr] read
     * twice, where a loop that stops at 0 and one test after it would do */
    if (d_dependant[d_nptr] == 0) {
        if (d_current_string_length == 0) {
            d_nptr = cw_ready + N6;
            d_current_string_length = 1;
            return;
        }
        goto add;
    }
    code = d_dependant[d_nptr];
    while (d_symbol[code] != cw_ready) {
        code = d_rbrother[code];
        if (code == 0) {
        add:
            d_symbol_to_add = cw_ready;
            add_node_decoder();
            d_current_string_length = 1;
            d_nptr = cw_ready + N6;
            return;
        }
    }
    /* the string ends before the node made last, as the coder's does. it also
     * ends at the first byte after compressed mode, as the coder's did there */
    if (code == d_last_node_created || entering_decoder_tr_mode) {
        d_nptr = cw_ready + N6;
        d_current_string_length = 1;
        d_last_node_created = 0;
    } else {
        d_current_string_length++;
        d_nptr = code;
    }
}

#define BAD_CODEWORD()                                                         \
    {                                                                          \
        rr_cnt_shadow = 0;                                                     \
        cmd_resp_to_host(ECDC_RESPONSE_BAD_CODEWORD);                          \
        StatisticInfo.link_end = LINK_END_BAD_CODEWORD;                        \
    }

static void decoder_compressed_mode(void)
{
    if (!read_rrbyte_to_codeword()) {
        return;
    }
    if (cw_ready > EXTEND_CODE) {
        data_codeword_rx_cm();
    } else if (cw_ready == ENTER_TRANSPARENT) {
        d_bit_ptr = 0;
        rr_word_temp = 0;
        d_transparent = 1;
        entering_decoder_tr_mode = 1;
    } else if (cw_ready == FLUSH_CODE) {
        d_bit_ptr = 0;
        rr_word_temp = 0;
        d_check_tree = 1;
    } else if (cw_ready == EXTEND_CODE) {
#ifdef NONMATCHING
        /* the GCC build refuses to widen codewords past N1 bits, which no
         * codeword of the dictionary needs */
        if (DC2 >= N1) {
            BAD_CODEWORD()
            return;
        }
#endif
        DC2++;
        /* DC2_mask takes a codeword's DC2 bits. DC2 is at least 10 here, so
         * the first arm never holds */
        if (DC2 == 9) {
            DC2_mask = 0x1ff;
        } else if (DC2 == 10) {
            DC2_mask = 0x3ff;
        } else if (DC2 == 11) {
            DC2_mask = 0x7ff;
        } else if (DC2 == 12) {
            DC2_mask = 0xfff;
        } else if (DC2 == 13) {
            DC2_mask = 0x1fff;
        } else if (DC2 == 14) {
            DC2_mask = 0x3fff;
        }
    }
}

/* send cw_ready's string to the host. the previous string, at d_nptr, and
 * then this one's first byte make the string to add. the call adds it where
 * d_nptr isn't 0 and check_decoder_tree finds it new. list takes cw_ready's
 * string from its last byte to its first, so it goes out read backward. a
 * codeword of DC1, the next node the decoder adds, names no string yet */
static void data_codeword_rx_cm(void)
{
    Uint16 code;
#ifdef NONMATCHING
    Uint8 *cursor = list;
    /* this side repairs the bug tagged below: it refuses a codeword whose
     * string the tables, list, N7 or the buffer can't hold */
    code = cw_ready;
    if (code >= N2 || code == DC1) {
        BAD_CODEWORD()
        return;
    }
    *cursor++ = d_symbol[code];
    while (code >= N5) {
        code = d_parent[code];
        if (code >= N2 || cursor == list + sizeof(list)) {
            BAD_CODEWORD()
            return;
        }
        *cursor++ = d_symbol[code];
    }
    n_list = (Uint16)(cursor - list);
    if (n_list > N7 || n_list > COM_BUFFER_SIZE - decomp_out_cntr) {
        BAD_CODEWORD()
        return;
    }
    d_symbol_to_add = cursor[-1];
    decomp_out_cntr += n_list;
    while (cursor != list) {
        cursor--;
        if (*cursor == d_esc_char) {
            d_esc_char = d_esc_char + ESCAPE_STEP;
        }
        *decomp_out_pntr_bak++ = *cursor;
    }
#else
    /* @hack type: cursor an integer address, where a Uint8 pointer would do,
     * stepped back before each read rather than after */
    Uint32 cursor;
    cursor = (Uint32)list;
    code = cw_ready;
    /* @bug memory: a codeword of N2 or more reads the tables past the
     * dictionary. the parents read there can lead anywhere, and the walk can
     * run past list's end. the decoder refuses only DC1. data_codeword_rx_cm
     * is at 0x0cb0027e */
    *(Uint8 *)cursor++ = d_symbol[code];
    if (code == DC1) {
        BAD_CODEWORD()
    } else {
        while (code >= N5) {
            /* @hack redundant: a test of &N5 that never runs */
            if (0) {
                /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
                if (&N5) {
                }
            }
            code = d_parent[code];
            *(Uint8 *)cursor++ = d_symbol[code];
        }
        /* @hack type: the count cut to a byte, where the plain difference
         * would do while a string is shorter than 256 bytes */
        n_list = (Uint8)(cursor - (Uint32)list);
        cursor--;
        d_symbol_to_add = *(Uint8 *)cursor;
        decomp_out_cntr += n_list;
        while (cursor >= (Uint32)list) {
            if (*(Uint8 *)cursor == d_esc_char) {
                /* @hack spelling: d_esc_char = d_esc_char + ESCAPE_STEP, where
                 * += would do */
                d_esc_char = d_esc_char + ESCAPE_STEP;
            }
            *decomp_out_pntr_bak++ = *(Uint8 *)cursor--;
            /* @hack redundant: a test of n_list that never runs */
            if (0) {
                if (n_list) {
                }
            }
        }
#endif
    /* @hack spelling: the test of d_nptr inside the test of
     * check_decoder_tree's result, where && would do */
    if (check_decoder_tree()) {
        if (d_nptr) {
            add_node_decoder();
        }
    }
    d_current_string_length = n_list;
    d_nptr = cw_ready;
#ifdef NONMATCHING
#else
    }
#endif
}

/* whether to add the string at d_nptr and then d_symbol_to_add. the call
 * returns 0 where its search finds the string, and clears
 * d_last_node_created. it returns 1 where the search doesn't. the search walks
 * d_nptr's children. for a peer whose XID names MMA, remote_is_mma, only the
 * first test after d_check_tree is set walks them. each later one looks only
 * at d_last_node_created */
/* @hack type: a char result, where a Bool would do */
static char check_decoder_tree(void)
{
    /* @hack redundant: register on code, byte, symbols and next */
    register Uint16 code;
    register Uint16 byte;
    register Uint8 *symbols;
    register Uint16 *next;

    if (!d_check_tree && remote_is_mma) {
        /* @hack spelling: the test of d_symbol_to_add inside the test of
         * d_nptr, where && would do */
        if (d_nptr == d_parent[d_last_node_created]) {
            if (d_symbol_to_add == d_symbol[d_last_node_created]) {
                d_last_node_created = 0;
                return 0;
            }
        }
    } else {
        d_check_tree = 0;
        code = d_dependant[d_nptr];
        byte = d_symbol_to_add;
        next = d_rbrother;
        symbols = d_symbol;
        while (code) {
            if (symbols[code] == byte) {
                d_last_node_created = 0;
                return 0;
            }
            code = next[code];
        }
    }
    return 1;
}

/* add_node_coder for the decoder: the string at d_nptr and then
 * d_symbol_to_add, at DC1 */
static void add_node_decoder(void)
{
    unsigned int code;
    Uint16 previous;
    Uint16 *children;

    if (d_current_string_length < N7) {
        /* @hack spelling: the two assignments joined by a comma, where a
         * statement each would do */
        children = d_dependant, code = DC1;
        d_rbrother[code] = children[d_nptr];
        children[d_nptr] = code;
        d_symbol[code] = d_symbol_to_add;
        d_parent[code] = d_nptr;
        children[code] = 0;
        d_last_node_created = code;
        code++;
        if (code > N2_moins1) {
            code = N5;
        }
        while (children[code]) {
            code++;
            if (code > N2_moins1) {
                code = N5;
            }
        }
        DC1 = code;
        if (find_d_left_brother(DC1, &previous)) {
            d_rbrother[previous] = d_rbrother[DC1];
        } else {
            d_dependant[d_parent[DC1]] = d_rbrother[DC1];
        }
    } else {
        d_last_node_created = 0;
    }
}

FIND_PREVIOUS_SIBLING(find_d_left_brother, d_rbrother, d_dependant, d_parent)

#define STORE_CODE_OCTET()                                                     \
    if (encryption) {                                                          \
        *(*write_pointer)++ = encrypt(word);                                   \
    } else {                                                                   \
        *(*write_pointer)++ = word;                                            \
    }
#define COUNT_CODE_OCTET()                                                     \
    (*queued)++;                                                               \
    tm_cnt_temp_temp++;                                                        \
    n_of_bytes_out++;                                                          \
    if (*write_pointer > TM_TOPP) {                                            \
        *write_pointer = TM_BUFFER;                                            \
    }

/* codewords pack from the least significant bit. a codeword's low bits fill
 * tm_byte_temp, the octet in progress, from bit c_bit_ptr up. CC2 is at least
 * FIRST_WIDTH, so a codeword always fills that octet. codewords stay below N2,
 * at most V42BIS_MAX_NODES, so CC2 stays at most 10. a codeword then fills at
 * most one octet more. each octet it fills goes to TM, and its last bits wait
 * in tm_byte_temp */
static void write_codeword_to_tm(Uint16 code)
{
    /* @hack spelling: TM_WRITE and tm_cnt_shadow through write_pointer and
     * queued, where the names themselves would do */
    Uint16 *queued;
    Uint8 **write_pointer;
    /* @hack redundant: register on word */
    register Uint16 word;

    word = tm_byte_temp;
    /* @hack spelling: the two assignments joined by a comma, where a
     * statement each would do */
    word = word | (code << c_bit_ptr), write_pointer = &TM_WRITE;
    STORE_CODE_OCTET()
    queued = &tm_cnt_shadow;
    COUNT_CODE_OCTET()
    word = code >> (8 - c_bit_ptr);
    c_bit_ptr += CC2_moins_8;
    if (c_bit_ptr >= 8) {
        STORE_CODE_OCTET()
        COUNT_CODE_OCTET()
        word = word >> 8;
        c_bit_ptr -= 8;
    }
    tm_byte_temp = word;
}

static void check_codeword_size(Uint16 code)
{
    while (code >= CC3) {
        write_codeword_to_tm(EXTEND_CODE);
        CC2++;
        CC2_moins_8++;
        CC2_moins_16++;
        CC3 += CC3;
    }
}

static void write_to_tm(Uint8 byte)
{
    if (encryption) {
        *TM_WRITE++ = encrypt(byte);
    } else {
        *TM_WRITE++ = byte;
    }
    tm_cnt_shadow++;
    tm_cnt_temp_temp++;
    n_of_bytes_out++;
    if (TM_WRITE > TM_TOPP) {
        TM_WRITE = TM_BUFFER;
    }
}

static void coder_octet_alignment(void)
{
    if (c_bit_ptr != 0) {
        c_bit_ptr = 0;
        write_to_tm(tm_byte_temp);
    }
}

/* read RR into rr_word_temp until it holds a codeword of DC2 bits. the call
 * then returns 1 with the codeword in cw_ready, or 0 where RR runs out
 * first */
/* @hack type: a char result, where a Bool would do */
static char read_rrbyte_to_codeword(void)
{
    while (rr_cnt_shadow > 0) {
        if (encryption) {
            rr_input_buf = decrypt(*RR_READ++);
        } else {
            rr_input_buf = *RR_READ++;
        }
        if (RR_READ > RR_TOPP) {
            RR_READ = RR_BUFFER;
        }
        rr_cnt_shadow--;
        RR_CNT--;
        rr_word_temp = (rr_input_buf << d_bit_ptr) | rr_word_temp;
        d_bit_ptr += 8;
        if (d_bit_ptr >= DC2) {
            cw_ready = rr_word_temp & DC2_mask;
            d_bit_ptr -= DC2;
            rr_word_temp = rr_input_buf >> (8 - d_bit_ptr);
            return 1;
        }
    }
    return 0;
}

static void write_to_dte(Uint8 byte)
{
    *decomp_out_pntr_bak++ = byte;
    decomp_out_cntr++;
}

void raz(Uint16 directions, Uint16 dictionary_size, Uint16 string_limit)
{
    Sint16 capacity;
#ifdef NONMATCHING
    /* the GCC build clamps each argument to the bounds raz requires */
    if (dictionary_size < FIRST_THRESHOLD) {
        dictionary_size = FIRST_THRESHOLD;
    } else if (dictionary_size > V42BIS_MAX_NODES) {
        dictionary_size = V42BIS_MAX_NODES;
    }
    if (string_limit > DECODER_FILL_MAX) {
        string_limit = DECODER_FILL_MAX;
    }
#endif
    N2 = dictionary_size;
    N2_moins1 = dictionary_size - 1;
    capacity = FIRST_THRESHOLD;
    N1 = FIRST_WIDTH;
    /* N2 is at most V42BIS_MAX_NODES, well inside capacity's Sint16 */
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (; N2 > capacity; capacity *= 2) {
        N1++;
    }
    N5 = V42BIS_FIRST_STRING;
    N6 = CONTROL_CODEWORDS;
    N7 = string_limit;
    /* the decoder sends a buffer once it holds more than decomp_out_max
     * bytes. no string is longer than N7, so none takes the buffer past
     * DECODER_FILL_MAX */
    decomp_out_max = DECODER_FILL_MAX - string_limit;
    init_coder();
    init_decoder();
    if (encryption) {
        init_encrypt();
    }
}

/* the bytes of each table of struct v42bis_dictionary_storage, symbols and
 * the six link tables after it */
#define TABLE_BYTES (2 * V42BIS_MAX_NODES)

static void init_coder(void)
{
    Sint16 i;
    CC1 = N5;
    CC2 = FIRST_WIDTH;
    CC2_moins_8 = FIRST_WIDTH - 8;
    CC2_moins_16 = FIRST_WIDTH - 16;
    CC3 = FIRST_THRESHOLD;
    current_string_length = 0;
    c_bit_ptr = 0;
    nptr = 0;
    cw_ready = 0;
    tm_byte_temp = 0;
    last_node_created = 0;
    n_byte_in = 0;
    last_nb_out = RATIO_INPUT;
    n_of_bytes_in = 0;
    n_of_bytes_out = 0;
    n_codeword_out = 0;
    c_esc_char = 0;
    c_transparent = 1;
    end_of_string = 0;
    go_compressed = 0;
    go_transparent = 0;
    flush_fl = 0;
    flushing_coder_data = 0;
    if (encryption) {
        stat_delay = 0;
    } else {
        stat_delay = TRANSPARENT_STAT_DELAY;
    }
    comp_in_cntr = 0;
    comp_in_cntr_bak = 0;
    c_symbol = common_buffer.dictionary.symbols;
    d_symbol = c_symbol + V42BIS_MAX_NODES;
#ifdef NONMATCHING
    /* this side reaches the link tables by their own names, the plain form the
     * hack tag below gives */
    c_parent = common_buffer.dictionary.encode_parent;
    c_dependant = common_buffer.dictionary.encode_child;
    c_rbrother = common_buffer.dictionary.encode_sibling;
    d_parent = common_buffer.dictionary.decode_parent;
    d_dependant = common_buffer.dictionary.decode_child;
    d_rbrother = common_buffer.dictionary.decode_sibling;
#else
    /* @hack type: the link tables as offsets from c_symbol, where
     * common_buffer.dictionary's fields would do */
    c_parent = (Uint16 *)(c_symbol + TABLE_BYTES);
    c_dependant = (Uint16 *)(c_symbol + (2 * TABLE_BYTES));
    c_rbrother = (Uint16 *)(c_symbol + (3 * TABLE_BYTES));
    d_parent = (Uint16 *)(c_symbol + (4 * TABLE_BYTES));
    d_dependant = (Uint16 *)(c_symbol + (5 * TABLE_BYTES));
    d_rbrother = (Uint16 *)(c_symbol + (6 * TABLE_BYTES));
#endif
    /* N2_moins1 is below V42BIS_MAX_NODES, well inside i's Sint16 */
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i <= N2_moins1; i++) {
        c_symbol[i] = 0;
        c_parent[i] = 0;
        c_dependant[i] = 0;
        c_rbrother[i] = 0;
    }
    /* N5 is V42BIS_FIRST_STRING, well inside i's Sint16 */
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = N6; i < N5; i++) {
        c_symbol[i] = i - N6;
    }
}

static void init_decoder(void)
{
    Sint16 i;
    DC1 = N5;
    DC2 = FIRST_WIDTH;
    DC3 = FIRST_THRESHOLD;
    DC2_mask = FIRST_THRESHOLD - 1;
    d_current_string_length = 0;
    d_nptr = 0;
    d_bit_ptr = 0;
    d_last_node_created = 0;
    rr_word_temp = 0;
    d_esc_char = 0;
    d_escape_mode = 0;
    d_transparent = 1;
    entering_decoder_tr_mode = 0;
    d_check_tree = 1;
    rr_input_buf = 0;
    decomp_out_cntr = 0;
    /* N2_moins1 is below V42BIS_MAX_NODES, well inside i's Sint16 */
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i <= N2_moins1; i++) {
        d_symbol[i] = 0;
        d_parent[i] = 0;
        d_dependant[i] = 0;
        d_rbrother[i] = 0;
    }
    /* N5 is V42BIS_FIRST_STRING, well inside i's Sint16 */
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = N6; i < N5; i++) {
        d_symbol[i] = i - N6;
    }
}

#ifdef NONMATCHING
/* this side keeps the image's warm-up sums: it reads ENCRYPT_KEY[0] by its
 * name where init_encrypt's warm-up reads past key_string */
#define WARMUP_OCTET(index)                                                    \
    ((index) == KEY_BYTES ? ENCRYPT_KEY[0] : key_string[index])
#else
#define WARMUP_OCTET(index) (key_string[index])
#endif

static void init_encrypt(void)
{
    Sint8 i;
    Sint8 index;
    scram_q = 0;
    descram_q = 0;
    encryp_cnt = KEY_START;
    decryp_cnt = KEY_START;
    encrypt_cumul_key = 0;
    decrypt_cumul_key = 0;
    index = 0;
    for (i = 1; i <= WARMUP_STEPS; i++) {
        /* @bug memory: the eighth and seventeenth steps read
         * key_string[KEY_BYTES], one past its end. the test wraps index only
         * on the step after it reaches KEY_BYTES. those steps take
         * ENCRYPT_KEY[0] at 0x0cb78d26, the byte after key_string's last at
         * 0x0cb78d25 */
        if (index++ >= KEY_BYTES) {
            index = 0;
        }
        descrambler(scrambler(WARMUP_OCTET(index)));
        encrypt_cumul_key += WARMUP_OCTET(index);
        decrypt_cumul_key += WARMUP_OCTET(index);
    }
}

static Uint8 encrypt(Uint8 byte)
{
    if (++encryp_cnt >= KEY_BYTES) {
        encryp_cnt = 0;
    }
    encrypt_cumul_key += key_string[encryp_cnt];
    byte = scrambler(byte) ^ encrypt_cumul_key;
    return byte;
}

static Uint8 decrypt(Uint8 byte)
{
    if (++decryp_cnt >= KEY_BYTES) {
        decryp_cnt = 0;
    }
    decrypt_cumul_key += key_string[decryp_cnt];
    byte ^= decrypt_cumul_key;
    return descrambler(byte);
}

static Uint8 scrambler(Uint8 byte)
{
    Uint16 taps = scram_q >> FIRST_TAP;
    unsigned int delayed;
    delayed = taps >> TAP_GAP;
    byte ^= delayed ^ taps;
    scram_q = ((Uint32)scram_q << 8) | byte;
    return byte;
}

static Uint8 descrambler(Uint8 byte)
{
    Uint16 taps = descram_q >> FIRST_TAP;
    unsigned int delayed;
    delayed = taps >> TAP_GAP;
    descram_q = ((Uint32)descram_q << 8) | byte;
    return taps ^ delayed ^ byte;
}
