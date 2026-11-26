/* REDLA and REDLB, Lost Colony's red lamps. each draws without fog, and its
 * lens blinks */
#ifndef STG27_RED_LAMPS_H
#define STG27_RED_LAMPS_H
#include <ninja.h>

struct task;

/* the name of the one texture both lamps' lists hold */
extern const char red_lamp_texture_name[16];
extern NJS_TEXLIST red_lamp_a_texlist;
extern NJS_TEXLIST red_lamp_b_texlist;
extern NJS_CNK_OBJECT red_lamp_b_object[1];
/* the lamps' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *colony_red_lamp_texlists[];

/* task needs a work, whose pos places the lamp. REDLA turns by ang.y, and
 * REDLB by the whole of ang */
void red_lamp_a_init(struct task *task);
void red_lamp_b_init(struct task *task);

#endif
