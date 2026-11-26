/* King Boom Boo's fire: the fireball he throws, his breath, and the walls of
 * flame they raise. a flame that reaches player 0 hurts it. it knocks player 0
 * along the heading of the boss's circle at the flame. no flame hurts player 0
 * in BIGBOGY_DEFEAT */
#ifndef BOSS_BIGBOGY_FIRE_H
#define BOSS_BIGBOGY_FIRE_H
#include <ninja.h>
struct task;

/* make a fireball, a root task, that grows above *hand and follows it. hand
 * must stay valid until bigbogy_fireball_throw or bigbogy_fireball_dismiss.
 * the call returns null where an allocation fails. boss must not be null, or
 * the call returns an unset pointer */
struct task *bigbogy_fireball_create(struct task *boss, const NJS_POINT3 *hand);
/* throw ball. h is the heading of the boss's circle at ball. p is the heading
 * from ball to player 0. the ball flies at player 0 where p lies within a third
 * of a turn of h. otherwise it flies at the point of the circle a third of a
 * turn ahead, on the floor. it curves from h onto its target. it goes out where
 * it falls below the floor or reaches player 0. it also goes out within
 * BIGBOGY_BALL_INNER of the center in x and z, and past BIGBOGY_BALL_OUTER.
 * where it goes out, it raises a wall of flame */
void bigbogy_fireball_throw(struct task *ball);
#define BIGBOGY_BALL_INNER 120.0F
#define BIGBOGY_BALL_OUTER 400.0F
/* shrink ball where it is until it goes, raising no flame */
void bigbogy_fireball_dismiss(struct task *ball);
/* make the breath as boss's child, BIGBOGY_BREATH_LEAD ahead of *position
 * along the heading of the boss's circle there. it sweeps drops of fire from
 * side to side ahead of itself. it sends two fronts of waves of flame, which
 * run about the center. it ends once bigbogy_mode() leaves BIGBOGY_BREATH,
 * and its drops end with it. the call returns null where an allocation
 * fails. boss must not be null, or the call returns an unset pointer */
struct task *bigbogy_breath_create(struct task *boss,
                                   const NJS_POINT3 *position);
#define BIGBOGY_BREATH_LEAD 20.0F
/* raise a wall of count flames up from *position, the lowest at a scale of
 * scale. each flame stands above the one below it, and is smaller. a flame goes
 * once it burns out, or at once where it reaches player 0. it takes the flames
 * above it as it goes. the lowest is parent's child, or a root task for a null
 * parent. the call returns it, or null for a count below 1 or where task_create
 * fails */
struct task *bigbogy_fire_wall(struct task *parent, const NJS_POINT3 *position,
                               Sint8 count, Float scale);
/* a fading flame's step, which nothing gives to a task. each step shrinks
 * the flame, and kills its task once the flame is gone. until the submode of
 * the task's parent reaches 2, the step first regrows part of the loss */
void bigbogy_flame_fade(struct task *task);
#endif
