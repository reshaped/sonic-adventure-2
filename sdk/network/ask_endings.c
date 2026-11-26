#include "ask_phrases.h"
#include <sg_xpt.h>

/* the endings' readings. a reading is a length byte, which counts its kana,
 * then each kana but the first. the first is the pwordmap bucket that holds
 * the ending */
#pragma section ATA
Uint8 pluswd[243] = {
#include "network/ask_endings/pluswd.inc"
};
/* the endings, the words and suffixes that may follow a dictionary word,
 * grouped by their reading's first kana */
struct ask_fixed_word pword[383] = {
#include "network/ask_endings/pword.inc"
};
/* the endings from pword[pwordmap[c]] up to pword[pwordmap[c + 1]] start with
 * the kana coded 0xa0 + c */
Sint16 pwordmap[ASK_ENDING_BUCKETS + 1] = {
#include "network/ask_endings/pwordmap.inc"
};
#pragma section
