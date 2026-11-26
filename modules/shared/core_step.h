/* the moving platforms of Cannon's Core, STEP and STEPB in a stage's object
 * list */
#ifndef SHARED_CORE_STEP_H
#define SHARED_CORE_STEP_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST core_step_texlist;
extern NJS_TEXLIST core_step_b_texlist;
/* the platforms' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *core_step_texlists[];
/* the set editor's names for the paths, in enum core_path_kind's order,
 * which no code reads */
extern const char *core_step_path_names[6];

/* the object list's steps for STEP and STEPB. each does nothing where
 * placement_try_leave(task) returns TRUE. task needs a work, and the platform
 * keeps a record of its own in task->motion. its node goes into the world as a
 * moving solid. where task has a force, its two records carry the riders of
 * players 0 and 1.
 *
 * the work's pos places the platform, and ang.y turns it. ang.x's low nibble,
 * mod 6, chooses the path by shared/core_platform_path.h's enum core_path_kind.
 * scl.x is the path's length, or an arc's radius. scl.y + 0.1 is its speed, in
 * units a step. a platform that goes and comes back waits scl.z steps at each
 * end. ang.z gives the heading from pos of a horizontal path, and of an arc's
 * pivot.
 *
 * a rising or sinking platform moves once a player rides it after more than
 * 30 steps without one. at its far end, it goes back on the same terms.
 * it also goes back once a player is near where it started, after those 30
 * steps. ang.x's high byte h sets how near: within (10h + 10) * 2. a
 * horizontal STEP has a child, a solid box under the platform. STEPB draws
 * with DRAW_CONTROL_SHADOW */
void core_step_init(struct task *task);
void core_step_b_init(struct task *task);

#endif
