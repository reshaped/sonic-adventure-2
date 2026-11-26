/* debris: the pieces a broken model flies apart into */
#ifndef SHARED_DEBRIS_H
#define SHARED_DEBRIS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* a piece is a chunk model that flies, falls and bounces on its floor. the
 * call that makes a piece gives its floor, a height in y unless
 * DEBRIS_GRAVITY says otherwise. a piece shrinks away once it comes to rest,
 * or as its life runs out. one task moves and draws every piece.
 *
 * a call makes no piece where it can't make that task. it leaves out each
 * piece the heap can't give */

/* each step, a piece keeps debris_friction of its speed and of its spin. a
 * piece above its floor also gains debris_gravity of speed toward the floor.
 * a piece that meets its floor at a falling speed under 5 * debris_gravity
 * comes to rest. a faster piece bounces off at 0.6 of its falling speed,
 * which friction doesn't slow on that step.
 *
 * a new piece's life is debris_life steps, and up to debris_life_spread more
 * at random. the piece shrinks away over the last 10 steps of its life */
extern Float debris_friction;
extern Float debris_gravity;
extern Sint32 debris_life;
extern Float debris_life_spread;

/* a piece's flags, which the calls below take */

/* above its floor, the piece leaves a puff of dust on a random tenth of its
 * steps */
#define DEBRIS_TRAIL 0x10
/* where the piece first reaches its floor, it makes a burst of the textures
 * bomb03 to bomb14 */
#define DEBRIS_LANDING_BURST 0x20
/* the piece ends where it reaches its floor */
#define DEBRIS_VANISH 0x40
/* each flag leaves the piece without a random spin about its axis */
#define DEBRIS_NO_SPIN_X 0x80
#define DEBRIS_NO_SPIN_Y 0x100
#define DEBRIS_NO_SPIN_Z 0x200
/* the piece's puffs and burst are scaled to 0.9 of its model's radius, not
 * 0.3 */
#define DEBRIS_BIG_PARTICLES 0x400
/* the piece passes through its floor. at or under its floor, friction alone
 * slows the piece */
#define DEBRIS_THROUGH_FLOOR 0x800
/* the calls that take pos and angle neither move the model by pos nor turn
 * it by angle. the pieces still fly away from pos. a piece thrown by
 * DEBRIS_THROW_SQUARE is still turned by angle about its center */
#define DEBRIS_UNPLACED 0x1000
/* below full scale, the piece draws by model_draw_tinted of
 * shared/common_object.h, tinted white with its scale for alpha. where the
 * model's first vertex chunk is NJD_CV_D8 with no flags, NJD_CE must follow
 * it. otherwise the draw never returns */
#define DEBRIS_FADE_COLORS 0x2000
/* the piece falls along the debris_axis that the low three bits of its flags
 * hold. the debris_throw calls set those bits from world_gravity, and the
 * other calls take them as flags gives them. floor is a coordinate on the
 * axis the piece falls along. the other calls' callers must negate floor for a
 * fall toward +y. the debris_throw calls negate it themselves.
 *
 * three axes don't fall as they are named, as debris.c's debris_axes says. a
 * DEBRIS_AXIS_POS_X piece falls toward -x, and a thrown one meets its floor
 * where x is -floor. a DEBRIS_AXIS_NEG_Z piece falls toward -y, and a
 * DEBRIS_AXIS_POS_Z piece toward +y */
#define DEBRIS_GRAVITY 0x4000
/* the piece draws its model by Ninja's simple drawer, not its easy one. a
 * piece that draws its build, or fades by DEBRIS_FADE_COLORS, ignores the
 * flag */
#define DEBRIS_SIMPLE 0x10000
/* the piece's speed is pulled toward the speed that the field of
 * shared/field.h gives at the piece. each step closes a fifth of the gap,
 * where the field gives a speed. a DEBRIS_GRAVITY piece ignores the field */
#define DEBRIS_FIELD 0x20000
/* gravity never pulls the piece, even above its floor. the piece passes
 * through its floor as a DEBRIS_THROUGH_FLOOR piece does */
#define DEBRIS_WEIGHTLESS 0x40000
/* the piece draws in draw/light.h's LIGHT_DEBRIS. light_after_draw's light is
 * applied after the piece */
#define DEBRIS_LIGHT 0x80000

/* the six directions along x, y and z. the flags of a DEBRIS_GRAVITY piece
 * hold one */
enum debris_axis {
    DEBRIS_AXIS_NEG_X,
    DEBRIS_AXIS_POS_X,
    DEBRIS_AXIS_NEG_Y,
    DEBRIS_AXIS_POS_Y,
    DEBRIS_AXIS_NEG_Z,
    DEBRIS_AXIS_POS_Z,
    DEBRIS_AXES
};

/* debris_throw's modes. each turns a piece at random about x and y. it turns
 * the point the piece starts from by other random angles. DEBRIS_THROW_SQUARE
 * turns both by whole quarter turns. it also tilts each of the point's two
 * angles by up to 30 degrees either way. DEBRIS_THROW_ANY turns both any way */
#define DEBRIS_THROW_SQUARE 1
#define DEBRIS_THROW_ANY 2

/* a part of a model that debris_break breaks. pos places the part in the
 * model. ang turns the part by njRotateZ, njRotateY and njRotateX, in that
 * order */
struct debris_part {
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model;
    NJS_POINT3 pos;
    struct angles ang;
};

/* a debris_part and its build: its model as debris_build compiled it, or
 * null */
struct debris_built_part {
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_DIRECT_HEAD *build;
};

/* a spin about x, y and z, in whole degrees a step */
struct debris_spin {
    Sint32 x, y, z;
};

/* the number of pieces the last step began with, or 0 where no task moves
 * them */
int debris_count(void);
/* compile the model of each of count parts at *buffer, for
 * njDirectDrawObject. each part's texture list is made current first. each
 * part's build is set to where its model's build starts. a part's build is
 * null where the build would reach DEMO_AREA of system/areas.h. *buffer ends
 * past the last build. returns how many parts were built */
int debris_build(Uint8 **buffer, struct debris_built_part *parts, int count);

/* make a piece of model and throw it at speed, straight away from pos. the
 * piece starts from its model's center, moved 1 along y. mode turns that
 * point at random. scale then scales it, angle turns it about y, and pos
 * moves it. mode is a DEBRIS_THROW_ value: any other counts as
 * DEBRIS_THROW_ANY.
 *
 * debris_throw_flags and debris_throw_built_flags take flags, and the other
 * two pass 0. debris_throw_built and debris_throw_built_flags take build.
 * where build isn't null, the piece draws it in model's place at full scale */
void debris_throw(NJS_POINT3 *pos, int mode, NJS_VECTOR *scale, Angle angle,
                  Float speed, Float floor, NJS_CNK_MODEL *model,
                  NJS_TEXLIST *texlist);
void debris_throw_flags(NJS_POINT3 *pos, int mode, NJS_VECTOR *scale,
                        Angle angle, Float speed, Float floor,
                        NJS_CNK_MODEL *model, NJS_TEXLIST *texlist,
                        Uint32 flags);
void debris_throw_built(NJS_POINT3 *pos, int mode, NJS_VECTOR *scale,
                        Angle angle, Float speed, Float floor,
                        NJS_CNK_MODEL *model, NJS_DIRECT_HEAD *build,
                        NJS_TEXLIST *texlist);
void debris_throw_built_flags(NJS_POINT3 *pos, int mode, NJS_VECTOR *scale,
                              Angle angle, Float speed, Float floor,
                              NJS_CNK_MODEL *model, NJS_DIRECT_HEAD *build,
                              NJS_TEXLIST *texlist, Uint32 flags);

/* make a piece of model moving at spd. the piece starts at its model's
 * center, turned by ang and then moved by pos. ang turns the center by
 * njRotateZ, njRotateY and njRotateX, in that order. the piece starts turned
 * by ang as well.
 *
 * debris_launch_spin's piece spins at *spin, whatever the DEBRIS_NO_SPIN_
 * flags say. where spin is null, the piece spins at random as the others'
 * pieces do. debris_launch_built_flags takes build. where build isn't null,
 * the piece draws it in model's place at full scale */
void debris_launch_flags(NJS_POINT3 *pos, struct angles *ang, Float floor,
                         NJS_VECTOR *spd, NJS_CNK_MODEL *model,
                         NJS_TEXLIST *texlist, Uint32 flags);
void debris_launch_built_flags(NJS_POINT3 *pos, struct angles *ang, Float floor,
                               NJS_VECTOR *spd, NJS_CNK_MODEL *model,
                               NJS_DIRECT_HEAD *build, NJS_TEXLIST *texlist,
                               Uint32 flags);
void debris_launch_spin(NJS_POINT3 *pos, struct angles *ang, Float floor,
                        NJS_VECTOR *spd, struct debris_spin *spin,
                        NJS_CNK_MODEL *model, NJS_TEXLIST *texlist,
                        Uint32 flags);

/* break a model of count parts into a piece for each. the model is turned by
 * angle about y, then moved by pos.
 *
 * each piece flies straight away from pos at speed, plus up to spread more at
 * random. a DEBRIS_UNPLACED piece flies away at speed alone. it gains up to
 * spread either way along each of x, y and z at random. spd is then added to
 * each piece's speed. debris_break passes 0 for flags.
 *
 * r is a part's model's radius. debris_break_sized multiplies the range of
 * each piece's random spin by 4 * (1 - r / radius). a part whose r is radius or
 * more doesn't spin. debris_break_built spins its pieces as debris_break_sized
 * does. below a radius of 0.1, it spins them as debris_break does */
void debris_break(NJS_POINT3 *pos, Angle angle, struct debris_part *parts,
                  int count, Float speed, Float spread, Float floor,
                  NJS_VECTOR *spd);
void debris_break_flags(NJS_POINT3 *pos, Angle angle, struct debris_part *parts,
                        int count, Float speed, Float spread, Float floor,
                        NJS_VECTOR *spd, Uint32 flags);
void debris_break_sized(NJS_POINT3 *pos, Angle angle, struct debris_part *parts,
                        int count, Float speed, Float spread, Float floor,
                        NJS_VECTOR *spd, Uint32 flags, Float radius);
void debris_break_built(NJS_POINT3 *pos, Angle angle,
                        struct debris_built_part *parts, int count, Float speed,
                        Float spread, Float floor, NJS_VECTOR *spd,
                        Uint32 flags, Float radius);

#endif
