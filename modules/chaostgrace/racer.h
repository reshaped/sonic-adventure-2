/* the race's Chao, garden Chao that run by the race's behaviors */
#ifndef CHAOSTGRACE_RACER_H
#define CHAOSTGRACE_RACER_H
#include <ninja.h>
#include "chao/work.h"
struct task;
struct chao_gene;
struct chao_profile;
struct chao_save;
struct garden_race_chao;

/* let a racer meet the stage's floor and edges, and not its course's */
#define RACE_USE_STAGE(chao)                                                   \
    do {                                                                       \
        ((struct chao_work *)(chao)->work)->flags &= ~CHAO_COURSE_CONSTRAIN;   \
        ((struct chao_work *)(chao)->work)->flags |=                           \
            CHAO_RACE_STAGE_COLLISION;                                         \
    } while (0)

/* step a behavior to its next mode, with its timer from 0 */
#define RACE_ADVANCE(behavior)                                                 \
    do {                                                                       \
        (behavior)->mode++;                                                    \
        (behavior)->timer = 0;                                                 \
    } while (0)

/* save's Chao as a racer at position, facing yaw, and return its task. save
 * must hold a Chao. the GCC side returns NULL where garden_spawn_chao does */
struct task *race_enter_saved_chao(struct chao_save *save, NJS_POINT3 *position,
                                   Angle yaw);
/* a racer chao_create makes from gene, profile and appearance, whose mover
 * never comes to rest. the GCC side returns NULL where chao_create does */
struct task *race_create_opponent(const struct chao_gene *gene,
                                  const struct chao_profile *profile,
                                  const struct garden_race_chao *appearance,
                                  NJS_POINT3 *position, Angle yaw);
/* give chao, an entrant, the next place. the first finisher's time becomes
 * race_fastest_time, and entrant 0's race_player_time. entrant 0's finish
 * stops the clock unless the race is RACE_MULTI. from RACE_GOAL on, or with
 * the clock stopped, it does nothing */
void race_record_finish(struct task *chao);
#endif
