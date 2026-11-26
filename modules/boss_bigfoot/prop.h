/* the props, ROBJ in the Big Foot stage's object list. kinds 4 and 5 break
 * when their weak point is hurt, and leave a wreck. the other kinds stand
 * still */
#ifndef BOSS_BIGFOOT_PROP_H
#define BOSS_BIGFOOT_PROP_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the props' texture lists for texture_load_shared, ended by null. they cover
 * the kinds, their wrecks and pieces, and the papers */
extern NJS_TEXLIST *prop_texlists[];

/* compile every kind's model at buf, for njDirectDrawObject, then kind 0's
 * wreck and its pieces. a kind an earlier call compiled isn't compiled again.
 * a model that would reach DEMO_AREA of system/areas.h isn't built, and draws
 * without a build. it returns buf past the last build */
Uint8 *prop_build(Uint8 *buf);
/* the object list's step for ROBJ. where placement_try_leave(task) returns
 * TRUE, it does nothing. otherwise it gives task the props' step and
 * destructor, and its kind's draw.
 *
 * task needs a work. its scl.x picks the kind, clamped to 0 through 9. its
 * pos places the prop, and its ang.y turns it. kind 9 takes scl.y as its
 * scale along x, and scl.z along y. the placement keeps the prop's state from
 * one task to the next. a broken prop comes back as its wreck */
void prop_init(struct task *task);

#endif
