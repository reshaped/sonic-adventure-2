/* the common objects, which the stages' objects share. a body falls, and
 * keeps its task out of the stage's surfaces. stacked tasks fall onto each
 * other. the players stand on solid objects, and pick up holdable ones.
 * colliders and weak points are child tasks that carry a collision */
#ifndef SHARED_COMMON_OBJECT_H
#define SHARED_COMMON_OBJECT_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/trail.h"
#include "system/angle.h"

struct task;

/* the bits of a body's flags that its task sets, to choose how it moves */
/* stop at a ledge, where no floor lies within step_down below the bottom */
#define BODY_LEDGE_STOP 0x1
/* roll as a ball of radius 10 rolls */
#define BODY_ROLLS 0x2
/* on a landing, turn ang.x and ang.z toward the floor's tilt */
#define BODY_TILTS 0x4
/* count a surface as a wall where its normal lies within 60 degrees of the
 * side's axis. without the flag, the normal must also point back at the
 * probe */
#define BODY_ANY_WALL 0x8
/* rest while still, with each of spd.x, spd.y and spd.z within 0.001 of 0. a
 * resting body neither falls nor collides with the stage. a task without a
 * collision, or one that collision_pushed finds pushed, doesn't rest */
#define BODY_RESTS 0x10
/* probe with surface_probe_sides_flat in place of surface_probe_sides */
#define BODY_PROBE_FLAT 0x2000
/* add no gravity. on a floor, body_step changes neither spd nor roll. the
 * drag still slows spd */
#define BODY_NO_GRAVITY 0x8000
/* move by half of spd, and fall under a fifth of the gravity */
#define BODY_SLOW 0x10000

/* the bits of a body's flags that body_init and body_step set */
/* the last collision met a ceiling */
#define BODY_HIT_CEILING 0x20
/* the last collision landed the body on a floor, or stopped it at a ledge */
#define BODY_ON_FLOOR 0x40
/* the last collision met a wall */
#define BODY_HIT_WALL 0x80
/* BODY_LEDGE_STOP acts only while BODY_LEDGE_WAIT is clear. body_init and
 * each stop at a ledge set it, and a landing clears it */
#define BODY_LEDGE_WAIT 0x100
/* body_init sets it. nothing reads it */
#define BODY_UNK_400 0x400
/* surface_probe_sides_flat returned 1 at the body's last BODY_PROBE_FLAT
 * probe. a probe without BODY_PROBE_FLAT leaves the flag as it was */
#define BODY_ON_FLAT 0x4000
/* the body moves while game_state.time_stopped isn't 0. body_step sets the
 * flag while a player holds the task during a time stop. it clears the flag
 * once time_stopped is 0 */
#define BODY_UNFROZEN 0x80000000

#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC's -fpack-struct=4 caps each member's alignment at 4. pack(8) lifts the
 * cap for the matrices, which GCC aligns to 8. they lie at 0 and 64, and the
 * record's 424 bytes are a multiple of 8. so no member moves, and the size
 * stays SHC's */
#pragma pack(push, 8)
#endif
#endif
/* a body: the record body_init gives a task, at task->force. the task may
 * change its sizes and factors */
struct body {
    /* how far rolling has turned the body. each step that moves a
     * BODY_ROLLS body multiplies roll into it */
    NJS_MATRIX rolled;
    /* the turn of one step's roll, worked out on the last step on a floor */
    NJS_MATRIX roll;
    Uint32 flags; /* BODY_ bits */
    /* where the probe lies from the work's pos, before the work's angles
     * turn it */
    NJS_VECTOR probe_offset;
    Float top; /* how far the body's top lies above the probe */
    Float radius; /* how near the probe a wall may come */
    Float bottom; /* how far the body's bottom lies above the probe */
    /* how far below the bottom a floor still counts for BODY_LEDGE_STOP */
    Float step_down;
    /* a wall multiplies the part of spd along its normal by
     * 1 - 2 * wall_bounce. so 0.5 stops that part, and 1 turns it back whole */
    Float wall_bounce;
    Float floor_bounce; /* the share of spd.y that a floor turns back */
    Float friction; /* the share of spd.x and spd.z that a landing keeps */
    Float unk_ac;
    Angle wall_turn; /* how far a wall turns ang.y */
    /* the work's pos after the body's last collision with the stage */
    NJS_POINT3 last_pos;
    /* the normal of the floor the last collision landed on, else (0, 1, 0) */
    NJS_VECTOR floor_normal;
    /* the stage's nearest surface on each of six sides of the probe. the probe
     * lies at the work's pos plus probe_turned */
    struct surface_probe probe;
    NJS_VECTOR probe_turned; /* probe_offset, turned by the work's angles */
    /* the work's flags as the last body_step left them. that call also set
     * WORK_HELD_BY_PLAYER here where the task had no collision, or
     * collision_pushed returned TRUE */
    Sint16 work_flags;
};
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side ends the pack(8) pushed above struct body, and checks that the
 * struct keeps SHC's 424 bytes */
#pragma pack(pop)
_Static_assert(sizeof(struct body) == 424, "struct body changed its size");
#endif
#endif
/* the body body_init gave task */
#define BODY(task) ((struct body *)(task)->force.p)

/* give task a body at task->force, from the Shinobi heap. task->force is null
 * where the heap has no room. task needs a work and a motion record.
 * body_init first gives the work FALL_KIND_NORMAL by fall_set_kind.
 *
 * the body's probe_offset is (0, 5, 0), its top 15 and its bottom -5. its
 * radius is 10, and its step_down 50. floor_bounce and friction are 0.7,
 * wall_bounce 0.8, unk_ac 0.1 and wall_turn 0x400 */
void body_init(struct task *task);
/* free task's body, and set task->force to null */
void body_destroy(struct task *task);
/* move task by its motion record's spd for one step, and keep it out of the
 * stage's surfaces. body_step then registers its collision. task must have a
 * body.
 *
 * while game_state.time_stopped isn't 0, only a BODY_UNFROZEN body moves. a
 * body that doesn't rest collides with the stage whether it moves or not.
 *
 * a body in the air gains gravity in spd.y, unless it rests. a body on a floor
 * has spd.y turned upward. a drag slows spd, and spd.y stays under a top speed.
 * a still BODY_RESTS body whose work_flags hold WORK_HELD_BY_PLAYER takes a
 * small spd.y down, so that it collides again.
 *
 * while a player holds task, body_step neither moves it nor registers its
 * collision. it sets spd to 0, clears BODY_ON_FLOOR, and sets rolled and roll
 * to the unit matrix. ang.x and ang.z keep 0.9 of their low 16 bits, as
 * Sint16s */
void body_step(struct task *task);

/* the most tasks stack_tasks holds */
#define STACK_TASKS 64
/* the height of one stacked task */
#define STACK_HEIGHT 20.0F
/* the stacked tasks: STACK_TASKS entries, each a task or null where free.
 * stack_add allocates the list, and stack_remove frees it once every entry
 * is free. the pointer is null while no list is allocated */
extern struct task **stack_tasks;
/* put task in the first free entry of stack_tasks. a task with a placement
 * moves down STACK_HEIGHT for each drop its placement's state counts. returns
 * 1, or 0 where no entry is free or the heap can't give the list */
int stack_add(struct task *task);
/* free each entry that holds task */
void stack_remove(struct task *task);
/* drop each stacked task whose work's pos lies more than 11 above pos. the task
 * must also lie less than 8 from pos across x and z. a stacked task keeps in
 * its work's scl.y how far it has left to fall. a drop adds STACK_HEIGHT to
 * that. it adds 1 to the state of the task's placement too, where it has one.
 * pos isn't written. returns 0 */
int stack_drop(NJS_POINT3 *pos);

/* the bits of a model object's or a solid object's mode */
/* turn about x and z too. without it the object turns about y alone. a model
 * object's draw turns about z, x and then y. a solid object's turns about z,
 * y and then x */
#define OBJECT_TURNED 0x1
/* draw with DRAW_CONTROL_SHADOW of system/util.h set, which lets the modifier
 * volumes change the model. the draw clears those bits after */
#define OBJECT_AS_LAND 0x2
/* call placement_try_leave as the object sets itself up, and at the start of
 * each step. where it returns TRUE, do no more */
#define OBJECT_MAY_LEAVE 0x4
/* in a solid object's mode: a step that clears WORLD_ADDED takes the object's
 * copy out of the world. no step puts it back */
#define OBJECT_NEAR_ONLY 0x80

/* make task a model object, which draws model with texlist at the work's pos
 * unless either is null. mode holds OBJECT_ flags, and its low byte becomes the
 * work's mode. where shapes isn't null, the task gets a collision of count
 * shapes, of kind COLLISION_OBJECT. the collision is registered while the
 * work's timer is 0.
 *
 * the object keeps model in task->motion and texlist in task->force, and
 * doesn't free them. returns 0, having set nothing up, where OBJECT_MAY_LEAVE
 * had the task leave, else 1 */
int model_object_init(struct task *task, NJS_TEXLIST *texlist,
                      NJS_CNK_MODEL *model, struct collision_shape *shapes,
                      int count, Sint16 mode);
/* make a child of parent a model object, as model_object_init does without
 * OBJECT_MAY_LEAVE. the child's work is as task_create_child left it.
 * returns the child, or null where task_create_child fails */
struct task *model_object_create(struct task *parent, NJS_TEXLIST *texlist,
                                 NJS_CNK_MODEL *model,
                                 struct collision_shape *shapes, int count,
                                 Sint16 mode);

/* make task a solid object, which the players stand on. it draws model with
 * texlist unless either is null. it takes mode and shapes as model_object_init
 * does. returns 0 where OBJECT_MAY_LEAVE had the task leave or the heap can't
 * give its record, else 1.
 *
 * the record at task->extra holds a copy of object's node, placed and turned
 * as the work is. the copy has no child or sibling, and object must have a
 * model. the copy goes into the collision world unless the work's mode held
 * OBJECT_NEAR_ONLY before the call. solid_object_init_flags adds world_flags
 * to the flags it goes in under.
 *
 * each step sets WORLD_ADDED in the work's flags while the timer is 0 and a
 * player is near. it clears the flag otherwise. a player is near within 15 more
 * than the radius of object's model from that model's center */
int solid_object_init(struct task *task, NJS_TEXLIST *texlist,
                      NJS_CNK_MODEL *model, struct collision_shape *shapes,
                      int count, NJS_OBJECT *object, Sint16 mode);
int solid_object_init_flags(struct task *task, NJS_TEXLIST *texlist,
                            NJS_CNK_MODEL *model,
                            struct collision_shape *shapes, int count,
                            NJS_OBJECT *object, Sint16 mode,
                            Uint32 world_flags);
/* make a child of parent a solid object, without OBJECT_MAY_LEAVE, at parent's
 * pos, angles and scl. solid_object_create makes it as solid_object_init does,
 * and solid_object_create_flags as solid_object_init_flags does.
 * solid_object_create_at makes it as solid_object_create_flags does, at pos and
 * ang. each returns the child, or null where task_create_child fails. a child
 * whose record the heap can't give is returned too, and its step does
 * nothing */
struct task *solid_object_create(struct task *parent, NJS_TEXLIST *texlist,
                                 NJS_CNK_MODEL *model,
                                 struct collision_shape *shapes, int count,
                                 NJS_OBJECT *object, Sint16 mode);
struct task *solid_object_create_flags(struct task *parent,
                                       NJS_TEXLIST *texlist,
                                       NJS_CNK_MODEL *model,
                                       struct collision_shape *shapes,
                                       int count, NJS_OBJECT *object,
                                       Sint16 mode, Uint32 world_flags);
struct task *solid_object_create_at(struct task *parent, NJS_POINT3 *pos,
                                    struct angles *ang, NJS_TEXLIST *texlist,
                                    NJS_CNK_MODEL *model,
                                    struct collision_shape *shapes, int count,
                                    NJS_OBJECT *object, Sint16 mode,
                                    Uint32 world_flags);

/* make a lock-on box: a child of parent that stops a mech's lock-on, as a
 * WORK_ID_STOP_LOCKON_BOX. it stands 30 from parent's pos along -z, turned as
 * njRotateEx(ang, 0) turns, and its angles are 0. its work's scl, the box's
 * half sizes, holds size turned the same way, each part made positive. it
 * carries an enemy_work and one sphere of radius 1, of kind COLLISION_ITEM.
 * the sphere is registered while the child's work's mode is 0. returns the
 * child, or null where task_create_child fails */
struct task *lockon_box_create(struct task *parent, Angle *ang,
                               NJS_VECTOR *size);

/* the color object_tree_tint and model_draw_tinted take for no tint */
#define COLOR_NONE 0xffffffff
/* the color with which object_tree_tint and model_draw_tinted halve each
 * byte of a vertex's color, alpha too */
#define COLOR_HALF 0x80808080
/* the color with which object_tree_tint and model_draw_tinted clear each
 * byte of a vertex's color, alpha too */
#define COLOR_CLEAR 0
/* the color with which object_tree_tint and model_draw_tinted clear a
 * vertex's alpha. each other byte loses at most 1 */
#define COLOR_CLEAR_ALPHA 0x00ffffff
/* tint in place the first vertex chunk of each model down a chain of nodes, as
 * vlist_shade shades. the chain runs from object to its child, that child's
 * child and so on. a model whose list doesn't open with an NJD_CV_D8 chunk
 * with no flags is passed over.
 *
 * object and each visited node's model must not be null. the call never returns
 * where a visited node has both a child and a sibling. it never returns either
 * where NJD_CE doesn't follow a chunk it tints */
void object_tree_tint(NJS_CNK_OBJECT *object, Uint32 color);
/* draw model with njCnkEasyDrawModel, its first vertex chunk tinted as
 * object_tree_tint tints. the tinted copy lies at WORK_AREA, over what was
 * there. where color is COLOR_NONE or the chunk isn't NJD_CV_D8 with no flags,
 * model draws as it is. otherwise the call never returns where NJD_CE doesn't
 * follow the chunk */
void model_draw_tinted(NJS_CNK_MODEL *model, Uint32 color);
/* model_draw_colored draws model with njCnkEasyDrawModel, and
 * model_build_colored builds it as model_build does. each takes a copy at
 * WORK_AREA, over what was there. the copy's vertices hold color in place of
 * their normals. model's vertex list must be one chunk, then NJD_CE, or each
 * call never returns. each vertex of the chunk must hold a point and a normal.
 * model_build_colored returns what model_build does */
void model_draw_colored(NJS_CNK_MODEL *model, Uint32 color);
NJS_DIRECT_HEAD *model_build_colored(Uint8 **buffer, NJS_CNK_MODEL *model,
                                     Sint32 color);

/* return the first of player slots 0 and 1 whose record's held is task. return
 * -1 where neither is. a slot without a record is passed over */
int player_holding(struct task *task);
/* TRUE where the squared distance from the current camera to pos is range2
 * or less, else FALSE */
Bool camera_near(NJS_POINT3 *pos, Float range2);

/* return a value from 0 to 1 that repeats with game_state.steps. the period is
 * hold + rest + rise + fall steps. the value rises from 0 over rise steps, and
 * holds 1 for hold. it then falls over fall, and holds 0 for rest */
Float pulse_level(int hold, int rest, int rise, int fall);
/* return a value from 0 to 1 that repeats with game_state.steps. the period is
 * period steps. the value falls from 1 over the period's first fade steps, and
 * is 0 through step rest. it then rises over fade steps, and is 1 to the
 * period's end */
Float blink_level(int period, int rest, int fade);
/* return blink_level(period, rest, fade) as an opaque gray: 0xff808080 at 0,
 * white at 1 */
Uint32 blink_light_gray(int period, int rest, int fade);

/* make a weak point: a child of parent at pos that tells parent when it is
 * hurt. the child has an enemy_work of enemy/enemy.h, and its collision is
 * registered while its work's submode is 0. once WORK_HURT is set in its work's
 * flags, it sets bit index of parent's work's submode. it then leaves a puff of
 * dust and a flash, and is killed. the submode is a Sint8, so index must be
 * below 8.
 *
 * weak_point_create gives the child its collision on its first step. the
 * collision is one sphere of radius 1, of kind COLLISION_ITEM, and not a
 * COLLISION_TARGET. weak_point_create_shaped gives it count shapes of kind
 * COLLISION_ITEM at once. it gives none where shapes is null or count is 0.
 *
 * weak_point_create_paired makes the child as weak_point_create does, but the
 * hurt sets bit index * 2. index must then be below 4. where the enemy_work's
 * shot_by isn't 0, the hurt also sets bit index * 2 + 1. it clears that bit
 * where shot_by is 0. enemy_work_alloc sets shot_by to -1, so a hurt by no
 * player's shot sets the bit.
 *
 * each returns the child, or null where task_create_child fails */
struct task *weak_point_create(struct task *parent, int index, NJS_POINT3 *pos);
struct task *weak_point_create_shaped(struct task *parent, int index,
                                      NJS_POINT3 *pos,
                                      struct collision_shape *shapes,
                                      int count);
struct task *weak_point_create_paired(struct task *parent, int index,
                                      NJS_POINT3 *pos);

/* set ang to the angles of the rotation that matrix holds. matrix equals
 * njRotateZ by ang->z, then njRotateX by ang->x, then njRotateY by ang->y,
 * on a unit matrix. matrix must hold a rotation and nothing else */
void matrix_get_angles(NJS_MATRIX *matrix, struct angles *ang);

/* a trail: a ring of count points, of which used are kept. the kept points
 * lie from index head on, oldest first, and wrap at count. step is
 * game_state.steps at the last trail_draw outside the pause screen */
struct trail {
    NJS_POINT3 *points;
    Sint32 count;
    Sint32 head;
    Sint32 used;
    Uint32 step;
};
/* return an empty trail of count points from the Shinobi heap, or null where it
 * has no room. style isn't read */
struct trail *trail_alloc(struct trail_style *style, int count);
/* draw trail as a strip that ends at pos, then keep pos as its newest point.
 * style sets the strip's width and color along its length. a null style draws
 * nothing, and empties trail where it isn't null. with a style, trail must not
 * be null. GCC's build draws nothing for a null trail. pos is in the current
 * matrix's space, and the kept points are in the view's. the strip's vertices
 * lie at WORK_AREA, over what was there.
 *
 * pos is kept once for each step game_state.steps has counted since trail's
 * step, up to 4 times. under TRAIL_ODD_STEPS it is kept for the odd steps
 * alone. none is kept while stage/stage.h's stage_get_mode returns STAGE_PAUSE.
 * a draw that finds no new step while game_state.time_stopped isn't 0
 * empties trail */
void trail_draw(struct trail_style *style, struct trail *trail,
                NJS_POINT3 *pos);
/* free trail and its points. a null trail is left alone. style isn't read */
void trail_free(struct trail_style *style, struct trail *trail);
/* forget trail's oldest point and return 1, or return 0 where it has none.
 * trail must not be null. GCC's build returns 0 for a null trail */
int trail_drop(struct trail *trail);

/* model_build, object_build and object_tree_build each compile a build for
 * njDirectDrawObject at *buffer. each moves *buffer past the build, and returns
 * where the build starts. where the build would reach DEMO_AREA of
 * system/areas.h, each builds nothing and returns null.
 *
 * model_build builds model under a node of its own. object_build builds
 * object's node alone: its model, with no place, turn or scale, and no child or
 * sibling. object_tree_build builds object as it is */
NJS_DIRECT_HEAD *model_build(Uint8 **buffer, NJS_CNK_MODEL *model);
NJS_DIRECT_HEAD *object_build(Uint8 **buffer, NJS_CNK_OBJECT *object);
NJS_DIRECT_HEAD *object_tree_build(Uint8 **buffer, NJS_CNK_OBJECT *object);
/* a model that model_builds_compile builds once */
struct model_build {
    Sint32 kind; /* a MODEL_BUILD_ kind */
    /* the texture list that must be loaded first, or null for none */
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    NJS_DIRECT_HEAD *built; /* the build, or null until then */
};
/* a build's kinds. MODEL_BUILD_ALONE builds as object_build does, and
 * MODEL_BUILD_TREE as object_tree_build does. an _AS_LAND kind sets
 * DRAW_CONTROL_SHADOW, builds as the kind before it, and clears those bits. a
 * kind outside the four builds as MODEL_BUILD_ALONE does */
#define MODEL_BUILD_ALONE 0
#define MODEL_BUILD_ALONE_AS_LAND 1
#define MODEL_BUILD_TREE 2
#define MODEL_BUILD_TREE_AS_LAND 3
/* build at *buffer each of count builds whose built is null. a build with a
 * texture list waits until the list is loaded. the call makes that list current
 * first. returns how many it built */
int model_builds_compile(Uint8 **buffer, struct model_build *builds, int count);

/* sphere_in_front_no_depth, sphere_in_front and sphere_in_front_within each
 * test a ball of radius about center, placed by the current matrix. each
 * returns FALSE where the ball lies wholly behind the camera, else TRUE.
 * sphere_in_front also sets *depth to how far center lies ahead.
 * sphere_in_front_within is FALSE too where center lies more than reach
 * ahead */
Bool sphere_in_front_no_depth(NJS_POINT3 *center, Float radius);
Bool sphere_in_front(NJS_POINT3 *center, Float radius, Float *depth);
Bool sphere_in_front_within(NJS_POINT3 *center, Float radius, Float reach);

/* cover the 640 by 480 pixel screen with color. an alpha of 255 draws opaque,
 * an alpha of 0 nothing, and any other translucent. a fill that draws leaves
 * the fog on, whether or not it was before. it leaves Ninja's blending as
 * source alpha over inverse source alpha */
void screen_fill(NJS_COLOR *color);

/* the bits of a holdable object's mode, beside OBJECT_MAY_LEAVE */
/* the object's inner cylinder pushes too, as its outer one does */
#define HOLDABLE_PUSHES 0x8
#define HOLDABLE_ROLLS 0x10 /* the object's body has BODY_ROLLS */
#define HOLDABLE_BOUNCY 0x20 /* the body's floor_bounce is 0.6, not 0.3 */
/* leave the object's pos out of the pressers of shared/switch.h, so that it
 * pushes no switch's cap */
#define HOLDABLE_NO_PRESSER 0x40
/* make task an object a player picks up and holds. it is a BODY_RESTS body that
 * draws model with texlist unless either is null. task needs a work and a
 * motion record. the motion record keeps hold_turn, how far a holding player's
 * draw turns the nodes that hold the object. where OBJECT_MAY_LEAVE has the
 * task leave, the call does nothing more.
 *
 * the object keeps model in task->extra and texlist in task->unk_3c. while no
 * player holds it, it draws shadow_object under itself. a child at its pos
 * carries its attack, a COLLISION_ATTACK collision. the attack is off while a
 * player holds the object.
 *
 * an object with a placement may release a small animal the first time a player
 * holds it. animal_release of object/animal.h releases it 5 above the object's
 * pos, of kind (n - 1) % 3. n is the work's scl.x as an int. no animal is
 * released where n is 0 or less */
void holdable_init(struct task *task, NJS_TEXLIST *texlist,
                   NJS_CNK_MODEL *model, Sint32 hold_turn, int mode);

/* set out to where point shows on the screen, with point in the current
 * matrix's space. out's x and y are in pixels, and its z is 1 over point's
 * depth. returns 0, with out as it was, where point lies behind the eye,
 * else 1 */
int screen_project(NJS_POINT3 *point, NJS_POINT3 *out);

/* play the sound of kind at pos, at SOUND_LOUDEST of sound/sound.h. the kinds
 * play these ids, and any other kind plays nothing:
 *
 *   0, 7     0x1011
 *   1, 2, 4  0x8015
 *   3        0x1022
 *   5        0x8015, where task_create gives kind 5's task
 *   6        0x1010
 *   8        0x1004
 *   9        0x100c
 *   10       one of 0x1010, 0x1011, 0x1004, 0x100c and 0x8015, by rand
 *
 * kind 5's task, ComObjDmySe, plays nothing. it kills itself on its 12th to
 * 40th step */
void object_sound_play(int kind, NJS_POINT3 *pos);

#endif
