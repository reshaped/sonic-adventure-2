/* the tasks of a stage's paths: a path's draw, and the steps that put
 * players on a path */
#ifndef STAGE_PATH_TASK_H
#define STAGE_PATH_TASK_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"
#include "system/game_state.h"

struct path;
struct task;

/* the record a path's task keeps in its extra */
struct path_task {
    Sint8 mode; /* 0 until the step sets the record up */
    /* the players the step holds on the path, bit n for slot n */
    Sint8 held;
    Sint16 frames; /* the frames path_draw has drawn the path in */
    /* by slot, the runs of the step since it let that player go, counted up
     * to its wait */
    Sint16 wait[GAME_PLAYERS];
    struct angles ang; /* what path_ledge_step turns a player to */
    /* the high and low corners of a box about the path's points, which the
     * step's set-up grows by its margin */
    NJS_POINT3 high;
    NJS_POINT3 low;
    NJS_VECTOR up; /* a rail's way up */
    struct path *path;
};

/* create a task named "exec" with step at level 0, and a zeroed struct
 * path_task in its extra. it returns the task, or null where task_create or
 * syCalloc fails */
struct task *path_task_create(void (*step)(struct task *task));

/* draw task's path as a triangle at each point but the last. it draws only
 * while the first point lies within 8000 of camera_task_work's pos */
void path_draw(struct task *task);

/* the steps of a path's task. each takes a player near the path onto it, by one
 * of player.h's path calls. it holds the player until the player's work loses
 * PLAYER_WORK_ON_PATH. after it lets a player go, it waits its own count of
 * runs to take that player again. every step's path must be PATH_LAYOUT_SLOPES,
 * unchecked, as must path_draw's.
 *
 * a player is near inside a box about the path's points, grown by the step's
 * margin. it must also lie within the step's reach of a point but the last. a
 * rail and a current measure the reach from their path instead */

/* path_join_step takes a player near a point by player_join_path. it takes one
 * only while player_read_stick is TRUE for the player, or its stick_lock isn't
 * 0. path_hang_step takes a player near a point by player_hang_path.
 * path_ledge_step would take one by player_ledge_path, but takes none. no path
 * of the game's images names path_hang_step or path_ledge_step */
void path_join_step(struct task *task);
void path_hang_step(struct task *task);
void path_ledge_step(struct task *task);
/* the set-ups of a rail, which hand the task on to the rail's step.
 * path_rail_init's rail has its up along +y, and path_rail_inverted_init's
 * along -y. the rail takes a player near it from beneath, by
 * player_rail_path_beneath, where world_gravity pulls along its up. it takes
 * one from above by player_rail_path otherwise. neither way takes a player
 * whose work's submode is PLAYER_REQUEST_RESET or PLAYER_REQUEST_MISS. from
 * above, the rail neither takes nor lets go a rising player whose stick_lock
 * is 0 or less. a player rises while the spd.y of its struct player is above
 * 0 */
void path_rail_init(struct task *task);
void path_rail_inverted_init(struct task *task);
/* take a player as path_join_step does, but by player_run_path, and none
 * whose work's submode is PLAYER_REQUEST_RESET or PLAYER_REQUEST_MISS */
void path_run_step(struct task *task);
/* a current along the path, which takes a player in water near the path by
 * player_current_path. it takes none:
 * - while game_state.time_stopped isn't 0
 * - whose work's submode is PLAYER_REQUEST_RESET or PLAYER_REQUEST_MISS
 * - already on a current's path
 * - in PLAYER_MODE_JUMP whose jump_steps is below 5
 * - that is a Sonic in PLAYER_MODE_SONIC_BOUNCE or
 *   PLAYER_MODE_SONIC_BOUNCE_UP
 *
 * it also lets a player go once the player's struct player names another
 * path.
 *
 * while players_on_path finds a player on the path, the current pushes each
 * player it holds in water, along the path and toward it:
 * - on STAGE_BOSS_LAST1 the push sets the player's speed
 * - on STAGE_RELAY_SONIC the current sets the player's acc along the path
 *   alone
 * - on any other stage the current adds the push to the push field of the
 *   player's struct player, and holds its stick_lock at 2 or more
 * but where the player's nearest place reaches an end of the path, the
 * current lets it go, and takes it off the path, instead */
void path_current_step(struct task *task);

#endif
