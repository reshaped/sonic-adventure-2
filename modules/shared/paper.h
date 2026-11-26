/* paper: pieces of a model that tumble, bounce on their floor and fade. the
 * street props of Mission Street and of the Big Foot stage throw them as they
 * break */
#ifndef SHARED_PAPER_H
#define SHARED_PAPER_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* a piece is a chunk model that flies, falls and bounces on its floor, a
 * height in y. a piece fades away once it comes to rest, or once its life
 * runs out. one task moves and draws every piece.
 *
 * each step, a piece keeps 0.99 of its speed and of its spin. a piece above
 * its floor also gains 0.02 of speed down y, or 0.03 in Security Hall's
 * build. a piece that meets its floor at a falling speed under 5 times that
 * gain comes to rest. a faster piece bounces off at 0.6 of its falling speed,
 * which friction doesn't slow on that step.
 *
 * a new piece's life is 80 steps, and up to 4 more at random. a piece starts
 * opaque, at an alpha of 1. the alpha falls by 1/15 on each step the piece
 * rests on its floor. it falls by as much again on each step past the piece's
 * life. the piece ends once its alpha falls to 0 */

/* a piece's flags, which paper_launch takes */

/* above its floor, the piece leaves a puff of dust on a random tenth of its
 * steps */
#define PAPER_TRAIL 0x1
/* where the piece first reaches its floor, it makes a burst of the textures
 * bomb03 to bomb14 */
#define PAPER_LANDING_BURST 0x2
/* the piece ends where it reaches its floor */
#define PAPER_VANISH 0x4
/* each flag leaves the piece without a random spin about its axis */
#define PAPER_NO_SPIN_X 0x8
#define PAPER_NO_SPIN_Y 0x10
#define PAPER_NO_SPIN_Z 0x20
/* the piece's puffs and burst are scaled to 0.9 of its model's radius, not
 * 0.3 */
#define PAPER_BIG_PARTICLES 0x40
/* the piece passes through its floor. at or under its floor, friction alone
 * slows the piece */
#define PAPER_THROUGH_FLOOR 0x80
/* below full alpha, the piece draws by model_draw_tinted of
 * shared/common_object.h, tinted white at its alpha. where the model's first
 * vertex chunk is NJD_CV_D8 with no flags, NJD_CE must follow it. otherwise
 * the draw never returns */
#define PAPER_FADE_COLORS 0x200
/* above its floor, the piece meets a gust on a random twentieth of its steps.
 * a gust adds up to 0.3 either way to the piece's speed along x and along z.
 * each amount is random */
#define PAPER_GUSTS 0x400

/* a spin about x, y and z, in whole degrees a step */
struct paper_spin {
    Sint32 x, y, z;
};

/* make a piece of model, drawn with texlist and moving at spd. the piece
 * starts at its model's center, placed by njTranslateV(pos), njRotateZ(ang.z),
 * njRotateY(ang.y) and njRotateZ(ang.x) in turn. it draws turned by
 * njRotateZ(ang.z), njRotateY(ang.y) and njRotateX(ang.x) in turn. it keeps
 * model and texlist, and none of the other pointers.
 *
 * the piece spins at *spin, whatever the PAPER_NO_SPIN_ flags say. where spin
 * is null, it spins at random as those flags allow. the call makes no piece
 * where it can't make the task or the heap can't give the piece */
void paper_launch(NJS_POINT3 *pos, struct angles *ang, Float floor,
                  NJS_VECTOR *spd, struct paper_spin *spin,
                  NJS_CNK_MODEL *model, NJS_TEXLIST *texlist, Sint16 flags);

#endif
