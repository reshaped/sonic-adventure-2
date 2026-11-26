/* the bombs the enemies drop and fire */
#ifndef SHARED_ENEMY_BOMB_H
#define SHARED_ENEMY_BOMB_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the bombs' textures, which enemy_bomb_load loads, and the normal bomb's
 * model. they let an object draw a bomb before it drops one */
extern NJS_TEXLIST enemy_bomb_texlist;
extern NJS_CNK_MODEL enemy_bomb_normal_model;

/* a bomb's kind, 0 to BOMB_KINDS - 1. the enemies' readouts print the
 * names */
#define BOMB_KINDS 5
enum bomb_kind {
    /* lies where it lands, and bursts once its count runs out */
    BOMB_NORMAL,
    /* bursts as it lands */
    BOMB_RED_ONE,
    /* a BOMB_NORMAL that grows to four times its size, with a larger blast */
    BOMB_HEAVY,
    /* bursts as it lands, into a column of fire */
    BOMB_FIRE,
    /* a BOMB_NORMAL 19 times in 20, at random, and else a ring */
    BOMB_RING
};

/* how enemy_bomb_create starts a bomb */
enum bomb_start {
    /* toward the nearest player, the faster the farther it is, up to a cap */
    BOMB_THROWN,
    /* from rest */
    BOMB_DROPPED,
    /* under WORK_HELD_BY_PLAYER, which the bomb sets, at a scale that grows
     * from 0 to 1. the bomb falls once the flag clears */
    BOMB_HELD,
    /* bursting at once */
    BOMB_BURSTING
};

/* the bit a bomb sets once a player touches its blast. it sets the bit in its
 * task's unk_3c and in its parent's work flags, and never clears it */
#define BOMB_HIT_PLAYER 0x200

/* load the bombs' textures from E_BOMTEX, unless a task holds them already,
 * and the blasts' as blast_load_texture does. that task runs at task level
 * 0, and releases the textures when it is destroyed. the heap must have room
 * for the task, unchecked */
void enemy_bomb_load(void);
/* make a bomb of kind at pos, started as start says, and return its task.
 * it returns null where the heap has no room for the task. the bomb is
 * parent's child, or without a parent a task at task level 3. the call loads
 * the textures as enemy_bomb_load does.
 *
 * every start but BOMB_THROWN plays a sound at pos. the bomb holds still
 * while game_state.time_stopped isn't 0. it ends with its blast, once parent is
 * killed, or once placement_try_leave finds it out of range.
 *
 * a BOMB_RING that comes up a ring makes no bomb, unless start is BOMB_HELD.
 * the call makes a scattered ring 10 below pos instead, and returns the
 * ring's task. the ring starts heading toward the nearest player, where there
 * is one.
 * the call adds the ring to game_state.ring_total and to snapshot_current's,
 * so snapshot_current must not be null, unchecked.
 *
 * under BOMB_HELD, a BOMB_RING that comes up a ring is a held bomb that draws
 * a ring. it plays no sound, and has no collision record and no enemy_work.
 * WORK_HELD_BY_PLAYER must not clear while it lives, unchecked.
 *
 * the heap must have room for the bomb's collision record and enemy_work,
 * unchecked. parent must have a work where it isn't null, unchecked. under
 * BOMB_THROWN, the slot player_nearest gives for pos must hold a player,
 * unchecked */
struct task *enemy_bomb_create(struct task *parent, enum bomb_kind kind,
                               enum bomb_start start, NJS_POINT3 *pos);

#endif
