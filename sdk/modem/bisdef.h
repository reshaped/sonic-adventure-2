/* V.42bis compression's state, the coder's and then the decoder's. each has a
 * dictionary and a bit cursor of its own. N1, N2 and N5 to N7 are the V.42bis
 * parameters of those names. CC1 to CC3 are the coder's C1 to C3, and DC1 to
 * DC3 the decoder's */
#ifndef MODEM_BISDEF_H
#define MODEM_BISDEF_H
#include <sg_xpt.h>

#define V42BIS_MAX_NODES 1024
#define V42BIS_ALPHABET 256
#define V42BIS_FIRST_STRING 259
/* each node takes two symbol bytes and six 16-bit links, 14 bytes */
#define V42BIS_STORAGE_BYTES (14 * V42BIS_MAX_NODES)

struct v42bis_dictionary_storage {
    Uint8 symbols[2 * V42BIS_MAX_NODES];
    Uint16 encode_parent[V42BIS_MAX_NODES], encode_child[V42BIS_MAX_NODES];
    Uint16 encode_sibling[V42BIS_MAX_NODES], decode_parent[V42BIS_MAX_NODES];
    Uint16 decode_child[V42BIS_MAX_NODES], decode_sibling[V42BIS_MAX_NODES];
};
/* the dictionaries' storage. before compression starts, error-control
 * negotiation borrows it as fallback. fallback then holds the bytes received
 * while the link may still fall back to no error control. its count gives how
 * many lie in data */
union modem_compression_storage {
    struct v42bis_dictionary_storage dictionary;
    struct {
        Uint16 count;
        Uint8 data[V42BIS_STORAGE_BYTES - 2];
    } fallback;
    Uint8 bytes[V42BIS_STORAGE_BYTES];
};
extern union modem_compression_storage common_buffer;

extern Uint16 tm_cnt_temp_temp;
extern Sint8 end_of_string, go_compressed, go_transparent, flushing_coder_data;
extern Sint8 c_transparent;
extern Uint8 byte_to_encode, c_esc_char, c_bit_ptr;
extern Sint8 flush_fl, update_cdic;
extern Uint8 *comp_in_pntr;
extern Uint16 comp_in_cntr;
extern Uint8 *comp_in_pntr_bak;
extern Uint16 comp_in_cntr_bak, tm_byte_temp, P0, P1, P2;
extern Uint16 current_string_length, nptr, stat_delay, n_codeword_out,
    n_byte_in;
extern Uint16 cw_ready, N1, N2, N5, N6, N7, N2_moins1;
extern Uint16 CC1, CC2, CC3;
extern Sint16 CC2_moins_8, CC2_moins_16;
extern Uint16 last_node_created;
extern Uint8 *c_symbol;
extern Uint16 *c_parent, *c_dependant, *c_rbrother;

extern Uint8 *decomp_out_pntr;
extern Uint16 decomp_out_cntr;
extern Uint8 *decomp_out_pntr_bak;
extern Sint8 d_transparent, d_escape_mode, entering_decoder_tr_mode;
extern Uint8 d_symbol_to_add, d_esc_char, list[V42BIS_ALPHABET], d_bit_ptr;
extern Sint8 d_check_tree;
extern Uint16 DC1, DC2, DC3, DC2_mask, n_list, d_last_node_created;
extern Uint16 d_current_string_length, d_nptr, rr_cnt_temp, rr_word_temp;
extern Uint16 rr_input_buf, decomp_out_max;
extern Uint8 *d_symbol;
extern Uint16 *d_parent, *d_dependant, *d_rbrother;

extern Sint8 get_v42bis_out;
extern Uint8 encrypt_cumul_key, decrypt_cumul_key;
extern Sint8 encryp_cnt, decryp_cnt;
extern Sint32 scram_q, descram_q;
#endif
