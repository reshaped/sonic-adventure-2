/* the Egg Golem's explosion in BOSS_GOLEM.PRS, which grows, fades and throws
 * black smoke */
#ifndef BOSS_GOLEM_EXPLOSION_H
#define BOSS_GOLEM_EXPLOSION_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* create an explosion at *pos and return its task. its outer shell grows from
 * size start toward end. the inner one stays 0.95 of the outer's size.
 * each step keeps 0.95 of the growth. past a life of 1, the outer shell then
 * stops short of end. the explosion fades out over life steps, then kills its
 * task. life must not be 0.
 *
 * it returns null where the heap can't give the task or its record. a task
 * left without its record still steps, and writes through null */
struct task *golem_explosion_create(NJS_POINT3 *pos, Float start, Float end,
                                    Float life);

#endif
