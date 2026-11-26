/* the sandglass, SANDGLASS in a stage's object list: an hourglass with a
 * button on top. a touch on the button or a blow to the body turns it over,
 * and its sand runs. shared/sandglass_registry.h keeps a stage's sandglasses
 * for other objects to find, hold and throw */
#ifndef SHARED_SANDGLASS_H
#define SHARED_SANDGLASS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* a sandglass's modes, in its work's mode. a tumble or a throw lands the
 * sandglass in SANDGLASS_MODE_RUNNING. the run's end stands it in
 * SANDGLASS_MODE_STANDING */
enum sandglass_mode {
    SANDGLASS_MODE_STANDING,
    SANDGLASS_MODE_TUMBLING, /* a strike makes it jump and turn over */
    SANDGLASS_MODE_THROWN, /* it flies on its spd and turns over */
    SANDGLASS_MODE_RUNNING, /* its sand runs */
    /* it stays where sandglass_carry puts it, until sandglass_throw */
    SANDGLASS_MODE_HELD
};
/* a record's next_mode while no mode waits */
#define SANDGLASS_NO_MODE (-1)

/* a sandglass's record, in its task's extra */
struct sandglass {
    Sint32 next_mode; /* the mode its next step takes, or SANDGLASS_NO_MODE */
    Sint32 time; /* the steps its sand has run, 0 while it stands */
    Angle held_turn; /* while it is held, its ang.y less its holder's heading */
    /* the heading along which it turns over, less its ang.y */
    Angle tumble_turn;
    Float floor; /* the height it lands at */
    Float lift; /* how far its body stands above its work's pos */
    /* each end's offset out along the sandglass's axis, from where it rests,
     * and the offset's speed. the bottom end is the top end's model turned
     * over. each end springs back to rest */
    Float top_offset;
    Float bottom_offset;
    Float top_offset_spd;
    Float bottom_offset_spd;
    /* from 0 to 1. a strike lowers it to 0, and the run's end raises it back.
     * the body stands rise * SANDGLASS_RISE higher. the ends' shape motion
     * shows its first frame at 1 and its last at 0 */
    Float rise;
    Float rise_spd; /* what rise gains a step, until rise passes 0 or 1 */
    /* the matrix its draws start from: a translation to its frame's center,
     * then its turns. each step builds it */
    NJS_MATRIX *matrix;
    struct task *button; /* the child whose collision is its button */
    struct task *body; /* the child whose collision is its body */
    /* TRUE for a sandglass sandglass_create made, FALSE for a placed one */
    Bool created;
    Sint32 tick_timer; /* the steps to the next sandglass_tick_sound */
};
/* how far a rise of 1 lifts the body */
#define SANDGLASS_RISE 4.0F

/* task's record */
#define SANDGLASS(task) ((struct sandglass *)(task)->extra.p)
/* the steps task's sand runs for, which its work's scl.x holds */
#define SANDGLASS_RUN_STEPS(task) ((task)->work->scl.x)

/* the sandglass's view of its task's struct task_motion: spd where that has
 * it, and ang_spd inside its unk_18 */
struct sandglass_motion {
    NJS_VECTOR spd;
    Uint8 unk_0c[0x18];
    struct angles ang_spd; /* what its ang gains a step */
};

/* the texture lists for texture_load_shared, ended by null: the frame's, the
 * ends', the sand's and the stream's */
extern NJS_TEXLIST *sandglass_texlists[];

/* the object list's step for SANDGLASS. task needs a work and a motion
 * record. the work's pos is where the sandglass stands and lands. its ang.y
 * turns it, until a landing sets ang.y to 0. the low byte of ang.z is its id,
 * and its sand runs for scl.x steps.
 *
 * the call does nothing where placement_try_leave(task) is TRUE. otherwise it
 * gives task its record and two collision children, the button's and the
 * body's. task takes the sandglass's own step, draw and destructor. the call
 * registers task in sandglasses where the table has room. where the heap can't
 * give the record, its matrix or a child, the call frees what it took. task's
 * step then stays sandglass_init, which tries again at the next step.
 *
 * a standing sandglass turns over at a touch on its button, or at a blow to
 * its body. a blow counts only from a player for whom sandglass_blow_strikes
 * is TRUE. a touch by player 0 bounces player 0 up. a blow turns the
 * sandglass over away from the player who struck. where sandglass_blow_stops
 * is TRUE too, the blow stops player 0, whichever player struck. the
 * sandglass draws only within a distance of 250 from player 0 */
void sandglass_init(struct task *task);
/* make a sandglass at *pos, turned to ang, with id and a run of time steps.
 * it returns the task, or null where the heap can't give the task. it returns
 * null too where the heap can't give what sandglass_init gives. the task then
 * stays with no record, and its step reads through null. the call registers
 * the task in sandglasses where the table has room.
 *
 * the sandglass stays in SANDGLASS_MODE_HELD until sandglass_throw. it lands
 * at a height of -40. it draws translucent while it stands or runs. it draws
 * at any distance, and nothing can strike it */
struct task *sandglass_create(NJS_POINT3 *pos, Angle ang, int id, int time);

/* each binary that links the sandglass defines these four hooks. the
 * sandglass passes each its own task. sandglass_hit_sound runs at each strike.
 * sandglass_tick_sound runs each time tick_timer runs out in
 * SANDGLASS_MODE_RUNNING. sandglass_land_sound runs where a created sandglass
 * lands from a throw. sandglass_shadow_draw runs at each draw while the
 * sandglass stands or runs, and while a placed one is held. a tumbling or
 * thrown sandglass draws its frame's own shadow instead */
void sandglass_hit_sound(struct task *task);
void sandglass_tick_sound(struct task *task);
void sandglass_land_sound(struct task *task);
void sandglass_shadow_draw(struct task *task);

/* each binary that links the sandglass defines these five as well, and its
 * other objects call them too.
 *
 * player is a player's task that the body's collision met.
 * sandglass_blow_strikes is whether that player's blow turns the sandglass
 * over. sandglass_blow_stops, asked only where the blow strikes, is whether
 * the blow also stops player 0 */
Bool sandglass_blow_strikes(struct task *player);
Bool sandglass_blow_stops(struct task *player);
/* the short turn from one angle to another, from -0x8000 to 0x7fff */
Angle sandglass_angle_turn(Angle from, Angle to);
/* turn (0, 0, radius) by ang about y, as njRotateY turns a point. *x and *z
 * take the result's x and z */
void sandglass_polar_xz(Angle ang, Float radius, Float *x, Float *z);
/* draw model in the constant material of alpha, red, green and blue */
void sandglass_model_draw_material(NJS_CNK_MODEL *model, Float alpha, Float red,
                                   Float green, Float blue);

#endif
