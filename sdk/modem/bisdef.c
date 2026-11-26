/* the V.42bis coder's and decoder's state, which bisdef.h declares */
#include <sg_xpt.h>
#include "bisdef.h"

Uint16 tm_cnt_temp_temp;
Sint8 end_of_string, go_compressed, go_transparent, flushing_coder_data;
Sint8 c_transparent;
Uint8 byte_to_encode, c_esc_char, c_bit_ptr;
Sint8 flush_fl, update_cdic;
Uint8 *comp_in_pntr;
Uint16 comp_in_cntr;
Uint8 *comp_in_pntr_bak;
Uint16 comp_in_cntr_bak, tm_byte_temp, P0, P1, P2;
Uint16 current_string_length, nptr, stat_delay, n_codeword_out, n_byte_in;
Uint16 cw_ready, N1, N2, N5, N6, N7, N2_moins1;
Uint16 CC1, CC2, CC3;
Sint16 CC2_moins_8, CC2_moins_16;
Uint16 last_node_created;
Uint8 *c_symbol;
Uint16 *c_parent, *c_dependant, *c_rbrother;

Uint8 *decomp_out_pntr;
Uint16 decomp_out_cntr;
Uint8 *decomp_out_pntr_bak;
Sint8 d_transparent, d_escape_mode, entering_decoder_tr_mode;
Uint8 d_symbol_to_add, d_esc_char, list[V42BIS_ALPHABET], d_bit_ptr;
Sint8 d_check_tree;
Uint16 DC1, DC2, DC3, DC2_mask, n_list, d_last_node_created;
Uint16 d_current_string_length, d_nptr, rr_cnt_temp, rr_word_temp;
Uint16 rr_input_buf, decomp_out_max;
Uint8 *d_symbol;
Uint16 *d_parent, *d_dependant, *d_rbrother;
Sint8 get_v42bis_out;
Uint8 encrypt_cumul_key, decrypt_cumul_key;
Sint8 encryp_cnt, decryp_cnt;
Sint32 scram_q, descram_q;
