/* ASK's grammar. a word has a part of speech, its kind, and a conjugating
 * word a form too. goku_connect is nonzero where an ending of kind right can
 * follow a word of kind left in form. setu_end is nonzero where a phrase can
 * end on a word of kind in form */
#ifndef ASK_GRAMMAR_H
#define ASK_GRAMMAR_H
#include <sg_xpt.h>
int goku_connect(Uint8 left, Uint8 form, Uint8 right);
int setu_end(Uint8 kind, Uint8 form);
#endif
