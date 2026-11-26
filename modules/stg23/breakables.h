#ifndef STG23_BREAKABLES_H
#define STG23_BREAKABLES_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST *base_wall_texlists[];
extern NJS_TEXLIST *base_block_texlists[];

/* WALL and BLOCK break in three attacks, cracking at the first two. each
 * keeps its damage in its placement's state. after a hit, the wall takes no
 * other for 10 steps, the block for 30. the whole part of |scl.z| picks one
 * of the wall's two looks */
void base_wall_init(struct task *task);
void base_block_init(struct task *task);

#endif
