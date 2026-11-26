/* the ai, an enemy that a stage's object list names "E AI". it stands or
 * flies, and fires bullets, lasers or capturing bullets at a player in
 * range */
#ifndef SHARED_ENEMY_AI_H
#define SHARED_ENEMY_AI_H

struct task;

/* the kinds, named for their titles in the image. the AI_GUN_ kinds fire
 * bullets, the AI_LASER_ kinds lasers, and the AI_CAPT_ kinds capturing
 * bullets. the fixed kinds are AI_GUN_FIXED, AI_LASER_FIXED and
 * AI_CAPT_FIXED, which stand, and the moving kinds fly. an EX kind waits
 * unseen, then drops and stands as the fixed kind of its name. AI_KINDS
 * counts the kinds */
enum ai_kind {
    AI_GUN_FIXED,
    AI_GUN_MOVING,
    AI_LASER_FIXED,
    AI_LASER_MOVING,
    AI_CAPT_FIXED,
    AI_SHIELDER, /* fires lasers, and guards itself while it waits */
    AI_GUN_FIXED_EX,
    AI_LASER_FIXED_EX,
    AI_CAPT_FIXED_EX,
    AI_ONLY_STAND, /* stands, and never fires */
    AI_KINDS
};

/* the object list's step for "E AI". its first call sets the ai up, and each
 * later call runs it. task needs a work and a placement, as object/set.h's
 * set manager gives them. the work holds the ai's entry in the set file:
 *
 * - scl.x: the kind, by its whole part. one that names no kind gives
 *   AI_GUN_FIXED;
 * - scl.y: the shots' speed. a kind that fires must not have 0, unchecked:
 *   each shot divides by its speed;
 * - scl.z: the range less 60, by its whole part. its fraction sets the point
 *   that a fixed kind and the shielder measure from, as below;
 * - ang.x: in its low byte, signed, a gun or capturing kind's shots in a
 *   burst. a laser kind's or the shielder's byte is its laser's length,
 *   which must lie below 0x80, unchecked;
 * - ang.y: the heading, with the group in its low byte, as
 *   enemy_work_alloc_grouped of enemy/enemy.h takes it. AI_ONLY_STAND takes
 *   its group by enemy_work_alloc_grouped_no_hunt;
 * - ang.z: in its low half, signed, the interval between bursts or lasers,
 *   in steps. a gun or capturing kind takes 60 for one below 60.
 *
 * the range lies across x and z. a moving kind measures it from where it was
 * set, and an EX kind from itself until it stands. a fixed kind and the
 * shielder measure it from a point ahead of where they were set. the point
 * lies along their heading, 1000 times scl.z's fraction away, taken without
 * its sign.
 *
 * the ai fires at the nearest player once one has stayed in range for more
 * than the interval. it fires again an interval after each burst or laser.
 * a fixed kind crouches until a player comes in range. its first wait is
 * then more than 60 steps from its rise, and an EX kind's more than 60 steps
 * from its landing. a gun kind with 0 shots or fewer never fires. a capturing
 * kind's burst is one capturing bullet, or bullets at a player already in
 * PLAYER_MODE_CAUGHT. those bullets are as many as the shots, and one at
 * least. a catch by the ai's capturing bullet starts its wait over.
 *
 * one hit beats an ai. a magic hand's hit, as enemy/enemy.h's
 * ENEMY_IN_MAGIC_HAND says, also makes it a ball. otherwise the shielder's
 * guard turns a hit away while a walker's lock holds it and nothing touches
 * it. the guard also turns away a touch by a player who isn't attacking, and
 * throws that player back.
 * a deflecting shielder and a hidden EX kind register no collision record, so
 * nothing hits them. the next call kills task by placement_kill. a beaten
 * AI_ONLY_STAND also sets its placement's unk_20. while unk_20 is set, each
 * call kills task at once by task_kill. the heap must have room for the ai's
 * enemy_work, its collision record and the tasks that keep its textures,
 * unchecked.
 *
 * while game_state.time_stopped is set, an ai neither moves nor turns, and
 * its timer holds. its motion holds too, unless it is a deflecting
 * shielder's. a hit then beats the ai, whatever its guard */
void ai_step(struct task *task);

#endif
