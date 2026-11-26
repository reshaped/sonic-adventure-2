/* King Boom Boo, the boss of the fight around the sandglass */
#ifndef BOSS_BIGBOGY_BOSS_H
#define BOSS_BIGBOGY_BOSS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

enum bigbogy_mode {
    BIGBOGY_INTRO,
    BIGBOGY_START,
    BIGBOGY_CIRCLE,
    BIGBOGY_HOLDER_DASH,
    BIGBOGY_THROW,
    BIGBOGY_PLAYER_HIT,
    BIGBOGY_BREATH,
    BIGBOGY_BITE,
    BIGBOGY_SUNLIGHT,
    BIGBOGY_RESTORE_DARK,
    BIGBOGY_PURSUIT,
    BIGBOGY_GROUNDED,
    BIGBOGY_DEFEAT
};

/* the boss's task from bigbogy_create until its destructor runs, and null
 * outside that time. bigbogy_reversed, bigbogy_mode and bigbogy_health then
 * return -1 */
struct task *bigbogy_task(void);
Sint8 bigbogy_reversed(void);
int bigbogy_mode(void);
int bigbogy_health(void);
void bigbogy_player_distance(struct task *boss, Float *distance, Angle *turn);
Float bigbogy_horizontal_distance(const NJS_POINT3 *from, const NJS_POINT3 *to,
                                  NJS_VECTOR *offset);
#ifdef NONMATCHING
/* this side returns whether it made the boss, so that bigbogy_stage_step can
 * try again */
Bool bigbogy_create(NJS_POINT3 *position, Angle heading,
                    const NJS_POINT3 *center);
#else
void bigbogy_create(NJS_POINT3 *position, Angle heading,
                    const NJS_POINT3 *center);
#endif

/* the shadow attacks call these four under second names, which symbols.txt
 * places at their addresses */
Angle bigbogy_home_tangent(struct task *boss);
void bigbogy_change_mode(struct task *boss, Sint8 mode);
void bigbogy_reverse(struct task *boss);
void bigbogy_accelerate(struct task *boss, Float acceleration);

#endif
