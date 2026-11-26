#ifndef STG31_PLATFORMS_H
#define STG31_PLATFORMS_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST *green_hill_platform_texlists[];
extern NJS_TEXLIST *green_hill_floor_texlists[];

extern NJS_TEXLIST green_hill_platform_texlist;
extern NJS_TEXLIST green_hill_crumbling_floor_texlist;
extern NJS_TEXLIST green_hill_floor_pieces_texlist;

/* ASHIBA, a platform facing ang.y. the whole part of scl.x, taken unsigned,
 * picks its kind, mod 4. kinds 0 and 1 swing out from pos along the platform's
 * x and back. the set editor names kind 1 vertical. a swing reaches up to
 * |scl.z| + 10 from pos. where scl.y is above 0, it runs toward +x at
 * scl.y + 0.1 a step. otherwise it runs toward -x at 0.1 - scl.y a step. kind 2
 * stands still. kind 3 falls 60 steps after player 0 first stands on it.
 * FALLASHIBA, a floor facing ang.y, crumbles once player 0 stands on it for 11
 * steps in a row */
void green_hill_platform_init(struct task *task);
void green_hill_crumbling_floor_init(struct task *task);

#endif
