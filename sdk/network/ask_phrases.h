#ifndef ASK_PHRASES_H
#define ASK_PHRASES_H
#include "ask_cells.h"
#include <sg_xpt.h>

struct ask_fixed_word {
    Uint8 kind, form, reading;
    /* the entries back to the shorter ending whose reading begins this one's,
     * or 0 for none */
    Sint8 back;
};
extern Uint8 pluswd[243];
extern struct ask_fixed_word pword[383];
#define ASK_ENDING_BUCKETS 95
extern Sint16 pwordmap[];

/* what conversion found at one position of the reading. it holds the ending and
 * the dictionary words that start there, and the phrases built from them. it
 * holds the candidates too, and the one chosen */
struct ask_position {
    struct ask_fixed_word *ending;
    struct ask_match *words;
    struct ask_phrase *phrases;
    union ask_candidate *candidates;
    Sint32 candidate_index;
    char fixed, length, first_length, word_length, ending_length, selection;
};
extern struct ask_position hchar[80];
/* hchar's positions, one for each byte of the reading */
#define ASK_POSITIONS 80
/* a position's ending is ASK_ENDING_UNSEARCHED until find_endings searches
 * it, then NULL where none begins there. its words are NULL until searched,
 * then ASK_NO_WORDS where none begins there. its phrases are NULL until
 * searched. ASK_NO_PHRASES stands for none, which no search stores */
#define ASK_ENDING_UNSEARCHED ((struct ask_fixed_word *)-1)
#define ASK_NO_WORDS ((struct ask_match *)-1)
#define ASK_NO_PHRASES ((struct ask_phrase *)-1)
extern int kd_mode, henkan_mode, ikkatsu_mode;
/* the modes henkan converts in, as henkan_mode holds them */
enum {
    ASK_FREE_LENGTHS, /* bs_prefer picks each phrase's length */
    /* the first phrase takes the length henkan is given, and bs_prefer
     * scores its phrases of that length */
    ASK_SCORED_LENGTH,
    /* the first phrase takes the length unscored, and bs_prefix favors the
     * next position's phrases that are endings alone */
    ASK_PLAIN_LENGTH,
    /* the reading may go on, so henkan stops where more of it could change
     * a phrase */
    ASK_PARTIAL_READING
};

void init_hchar(void);
void free_hchar(int first, int end, int keep);
int bunsetu_len(int first);
int bs_prefer(int first, int end, int length);
void bs_prefix(int first);
Uint16 kh_priority(struct ask_phrase *phrase, Uint16 rank);
#endif
