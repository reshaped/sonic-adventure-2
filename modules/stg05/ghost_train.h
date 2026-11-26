/* the ghost train, GSTRAIN in Pumpkin Hill's object list: an engine and the car
 * it pulls. the train runs along a path, or to and fro along a line. a
 * turntable sends out trains of its own */
#ifndef STG05_GHOST_TRAIN_H
#define STG05_GHOST_TRAIN_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the texture lists for texture_load_shared of the train and of its smoke,
 * ended by null */
extern NJS_TEXLIST *ghost_train_texlists[];

/* a train's modes, in its work's mode. a train moves only while
 * GHOST_TRAIN_RUNNING */
enum ghost_train_mode {
    GHOST_TRAIN_RUNNING,
    GHOST_TRAIN_FADING_OUT, /* it fades out, then turns GHOST_TRAIN_GONE */
    GHOST_TRAIN_FADING_IN, /* it fades in, then runs */
    /* it neither draws nor collides, and hides the piece it carries */
    GHOST_TRAIN_GONE
};
/* a train's submodes, in its work's submode, which the train never changes.
 * under either, the train starts faded out and fades in. under
 * GHOST_TRAIN_FADES_AT_END it also fades out where it turns back at a line's
 * end */
#define GHOST_TRAIN_FADES 1
#define GHOST_TRAIN_FADES_AT_END 2
/* the id a turntable gives the train it sends out */
#define GHOST_TRAIN_TURNTABLE 0xff
/* GHOST_TRAIN_LINE in ang.x puts the train on a line about pos.
 * GHOST_TRAIN_BASE_SPEED in scl.x runs it at its base speed, and
 * GHOST_TRAIN_NO_PIECE in scl.y gives it no emerald piece */
#define GHOST_TRAIN_LINE 0
#define GHOST_TRAIN_BASE_SPEED 0.0F
#define GHOST_TRAIN_NO_PIECE 0.0F
#define GHOST_TRAIN_LINE_SCALE 100.0F

/* the object list's step for GSTRAIN, which a turntable also gives the child it
 * sends out. it gives task the train's step, draw and destructor. it gives task
 * a record in task->motion from syCalloc, which the destructor frees. where
 * syCalloc fails, the call does nothing, and the next step tries again. a train
 * never leaves at its placement's range.
 *
 * task needs a work and TASK_FORCE's records. the low four bits of ang.x, n,
 * pick what the train runs along. n from 1 to 4 picks one of four paths. a
 * greater n picks n modulo 5's path, and a remainder of 0 picks 2's. an n of 0
 * picks a line along z, centered on pos. the line is scl.z + 1 times
 * GHOST_TRAIN_LINE_SCALE long. ang.y turns the path or line about pos. under
 * bit 0x10 of ang.x, the path or line lies in the world's axes instead.
 *
 * the train runs at scl.x + 2 a step, backward below 0. on a line it turns back
 * at each end, and changes scl.x to run at the negated speed. on a path it
 * whistles as it passes scl.z hundredths of the way along the path's segments.
 * the train carries an emerald piece where the low byte of scl.y isn't 0. the
 * byte is 1 plus the piece's id.
 *
 * the low byte of ang.z is the train's id. under submode 0, a train whose id
 * isn't 0 starts GHOST_TRAIN_GONE, and one of id 0 runs. the call then keeps
 * the id's low five bits, which ghost_train_find matches. it lists task for the
 * calls below, which find only listed trains. the list holds ten, and a train
 * started while it is full goes unlisted.
 *
 * the players stand on a copy of the car's collision model in the collision
 * world. the copy collides only while a player lies within its model's radius
 * plus 32 of the car. the car carries player 0 as it moves, but no other
 * player. the engine and the car hurt players, enemies and objects */
void ghost_train_init(struct task *task);
/* give the train task GHOST_TRAIN_FADING_IN or GHOST_TRAIN_FADING_OUT. each
 * returns TRUE */
Bool ghost_train_fade_in(struct task *task);
Bool ghost_train_fade_out(struct task *task);
/* the first listed train whose id is id, or null where there is none */
struct task *ghost_train_find(Sint8 id);
/* the listed train nearest pos that isn't GHOST_TRAIN_GONE, measured to its
 * engine. *engine takes the engine's place. where there is no such train, it
 * returns null and leaves *engine */
struct task *ghost_train_nearest(NJS_POINT3 *pos, NJS_POINT3 *engine);

#endif
