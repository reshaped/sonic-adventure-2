#ifndef STG23_PLATFORMS_H
#define STG23_PLATFORMS_H
#include <ninja.h>

struct task;

/* the lift's and the swing's inits keep the work's placed pos in the task's
 * extra, as ORIGIN. YAW is the high byte of the work's ang.y, in place, and
 * START_PHASE its low byte in 256ths of a turn */
#define ORIGIN(task) ((NJS_POINT3 *)(task)->extra.p)
#define YAW(work) ((work)->ang.y & 0xff00)
#define START_PHASE(work) (((work)->ang.y & 0xff) * 256)

extern NJS_TEXLIST *base_plinth_texlists[];
extern NJS_TEXLIST *base_lift_texlists[];
extern NJS_TEXLIST *base_swing_texlists[];
extern NJS_TEXLIST base_swing_texlist;

/* KOTEIDAI, a plinth of one of five heights, as the whole part of |scl.z|
 * picks, mod 5. its width, depth and collision height scale by scl.x + 1 */
void base_plinth_init(struct task *task);
/* UGOKIDAI, a lift that swings from its pos down to 2 * scl.y below it and
 * back. ang.x sets its speed, and ang.y's low byte its phase. ang.z's low
 * byte picks one of six lifts, mod 6, and ang.y's high byte turns it */
void base_lift_init(struct task *task);

/* KAITEN, a platform that carries a rider. it swings along its facing by
 * scl.z, and up and down by scl.y. ang.x's low byte sets the first swing's
 * speed, and its high byte the second's. ang.y's low byte sets both swings'
 * phase, and its high byte turns the platform */
void base_swing_init(struct task *task);
extern NJS_CNK_MODEL plinth_1m;
extern NJS_CNK_MODEL plinth_6m;
extern NJS_CNK_MODEL plinth_11m;
extern NJS_CNK_MODEL plinth_16m;
extern NJS_CNK_MODEL plinth_21m;
extern NJS_CNK_MODEL lift_3m;
extern NJS_CNK_MODEL lift_5m;
extern NJS_CNK_MODEL lift_7m;
extern NJS_CNK_MODEL lift_dark_3m;
extern NJS_CNK_MODEL lift_dark_5m;
extern NJS_CNK_MODEL lift_dark_7m;
extern NJS_CNK_MODEL lift_shadow;
extern NJS_OBJECT lift_collision_3m;
extern NJS_OBJECT lift_collision_5m;
extern NJS_OBJECT lift_collision_7m;
extern NJS_CNK_MODEL swing_model;
extern NJS_OBJECT swing_collision;

#endif
