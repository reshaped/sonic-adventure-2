/* the glow lights: a glow that streaks as the camera moves. Mission Street's,
 * two-player Mission Street's and the Big Foot fight's object lists call them
 * GLIGHT and GLIGHT2 */
#ifndef SHARED_GLOW_LIGHT_H
#define SHARED_GLOW_LIGHT_H
#include <ninja.h>

struct task;

/* the glow's two textures: the glow, then its streak */
extern NJS_TEXLIST glow_texlist;
/* the glow's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *glow_texlists[];

/* the object list's step for GLIGHT. unless placement_try_leave(task) returns
 * TRUE, it gives task a record of two streaks in task->motion. it also gives
 * task the light's own step, draw and destructor. where the heap can't give the
 * record, this stays task's step and tries again on the next. where the heap
 * can't give a streak, that streak is null. the draw reads through it whenever
 * the glow streaks on that streak's screen.
 *
 * task needs a work, whose pos places the glow. the glow is white. it keeps a
 * streak for each of two players' screens. it streaks only while it lies less
 * than 350 ahead of the camera, and while game_state.mini_event_running is 0 */
void glow_light_init(struct task *task);
/* the object list's step for GLIGHT2: a glow as GLIGHT's, in the color of its
 * work's scl. scl.x, scl.y and scl.z give its red, green and blue. each runs
 * from 0 to 1, and is held to that range. its quad's second edge lies a
 * quarter turn from its first, where GLIGHT's lies 0x5000 from it. the light's
 * own step writes the packed colors over the work's ang.x and ang.y */
void glow_light_colored_init(struct task *task);

#endif
