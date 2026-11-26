/* Sand Ocean's explosion, which stg21/explosion.c builds from
 * boss_golem/explosion.c */
#ifndef STG21_EXPLOSION_H
#define STG21_EXPLOSION_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* as golem_explosion_create does */
struct task *ocean_explosion_create(NJS_POINT3 *pos, Float start, Float end,
                                    Float life);

#endif
