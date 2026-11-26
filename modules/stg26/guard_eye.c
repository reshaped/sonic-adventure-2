/* the guard eyes, which a guard's attack calls down around player 0. each
 * aims at player 0, fires a beam that hurts players, and fades */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "stg26/guard_art.h"
#include "stg26/guard_eye.h"
#include "stg26/quarters_util.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

enum eye_mode {
    NO_NEXT_MODE = -1, /* next_mode while no mode waits to start */
    EYE_WAIT,
    EYE_FIRE,
    EYE_FADE,
    EYE_MODES
};
enum eye_shape { EYE_BODY, EYE_BEAM, EYE_SHAPES };
enum beam_phase { BEAM_GROWING, BEAM_SLIDING };

#define EYE_PLACES 12
#define PLACE_ANGLE (FULL_TURN / EYE_PLACES)
#define EYE_LEVEL 2
/* the beam starts at (0, EYE_BEAM_Y, EYE_BEAM_Z) in the eye's own axes */
#define EYE_BEAM_Y 1.7276F
#define EYE_BEAM_Z (-3.2056F)
/* in EYE_FIRE, the eye aims at player 0 until step BEAM_GROW_STEPS. the beam
 * meanwhile grows by BEAM_SPEED_SHARE of their distance a step. it then
 * slides away from the eye at the speed it last grew by. the eye starts to
 * fade at step BEAM_LAST_STEP */
#define BEAM_GROW_STEPS 20
#define BEAM_LAST_STEP 30
#define BEAM_SPEED_SHARE 0.25F
#define BEAM_SOUND 6
#define EYE_CAMERA_CLEARANCE 25.0F
/* the eye's index in guard_eye_positions */
#define EYE_SLOT(task) ((task)->unk_3c.l)

struct eye_state {
    enum eye_mode mode;
    /* the mode eye_step_mode starts next, or NO_NEXT_MODE */
    enum eye_mode next_mode;
    enum beam_phase phase; /* EYE_FIRE's phase */
    Sint32 steps; /* the steps since the mode started */
    Sint32 place; /* its place on the ring, from 0 to EYE_PLACES - 1 */
    Float beam_length;
    /* in EYE_FADE, the eye's spin about y, its scale across and up, and its
     * alpha. spin_speed, scale_across_speed, scale_up_speed and alpha_speed
     * are what a step of the fade adds to each */
    Angle spin;
    Angle spin_speed;
    Float scale_across;
    Float scale_across_speed;
    Float scale_up;
    Float scale_up_speed;
    Float alpha;
    Float alpha_speed;
    Float beam_z; /* the beam's offset along z, which the slide lowers */
    Float beam_speed; /* what a step of the slide takes from beam_z */
};
#define EYE(task) ((struct eye_state *)(task)->extra.p)

static void eye_destroy(struct task *task);
static void eye_step(struct task *task);
static void eye_draw(struct task *task);
static struct eye_state *eye_alloc(struct task *task);
static void eye_free(struct task *task);
static void eye_start_mode(struct task *task, enum eye_mode mode);
static void eye_step_mode(struct task *task);
static void eye_wait_start(struct task *task);
static void eye_wait_step(struct task *task);
static void eye_fire_start(struct task *task);
static void eye_fire_step(struct task *task);
static void eye_fade_start(struct task *task);
static void eye_fade_step(struct task *task);
static void eye_place(struct task *task);

NJS_TEXLIST *guard_eye_texlists[3] = {
#include "stg26/guard_eye/guard_eye_texlists.inc"
};

/* the EYE_PLACES places lie evenly around a ring ring_height above player 0,
 * of radius ring_radius */
static Float ring_height = 20.0F;
static Float ring_radius = 25.0F;
static Sint32 beam_volume = 25;
static Bool models_built = FALSE;
static struct model_build models[EYE_SHAPES] = {
#include "stg26/guard_eye/models.inc"
};
static void (*mode_starts[EYE_MODES])(struct task *) = {
    eye_wait_start,
    eye_fire_start,
    eye_fade_start,
};
static void (*mode_steps[EYE_MODES])(struct task *) = {
    eye_wait_step,
    eye_fire_step,
    eye_fade_step,
};
/* the eye's body, and its beam, which hurts players. a quarter turn about x
 * lays the beam along z, and EYE_FIRE sets its length */
static struct collision_shape shapes[EYE_SHAPES] = {
#include "stg26/guard_eye/shapes.inc"
};
static Angle fade_spin_speed = 0x400;
static Angle fade_spin_accel = 0x100;
static Float fade_across_speed = -0.015F;
static Float fade_across_accel = -0.0025F;
static Float fade_up_speed = 0.04F;
static Float fade_up_accel = 0.01F;
static Float fade_alpha_speed = -0.01F;
static Float fade_alpha_accel = -0.0025F;

/* whether each place was free when guard_eye_find_places last looked. no
 * code reads it */
static Bool place_free[EYE_PLACES];
/* the free places that no eye has taken, the first guard_eye_free_count of
 * the array */
static Sint32 free_places[EYE_PLACES];

/* @hack spelling: next_mode set through a call SHC expands, where an
 * assignment at each call would do */
static void eye_request_mode(struct task *task, enum eye_mode mode)
{
    EYE(task)->next_mode = mode;
}

Uint8 *guard_eye_build(Uint8 *buffer)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buffer, models, EYE_SHAPES) == EYE_SHAPES) {
        models_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buffer;
}

void guard_eye_create(int slot)
{
    struct task *task;
#ifdef NONMATCHING
    /* the index in free_places of the place this side's create takes */
    int place_index;
#else
    int read;
    int write;
#endif

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK | TASK_FORCE, EYE_LEVEL, eye_step,
                            "ObjectEyeExec")) == NULL) {
        return;
    }
#ifdef NONMATCHING
    /* this side repairs the two bugs tagged in the matching side. it destroys
     * the eye where eye_alloc or collision_init fails, and takes the place off
     * the list only once both have succeeded. it moves down only the places
     * past the eye's, and reads no word past free_places */
    if (eye_alloc(task) == NULL) {
        task_destroy(task);
        return;
    }
    place_index = (int)((Float)rand() * guard_eye_free_count * RAND_SCALE);
    EYE(task)->place = free_places[place_index];
    eye_place(task);
    if (!collision_init(task, shapes, EYE_SHAPES, COLLISION_OBJECT)) {
        task_destroy(task);
        return;
    }
    guard_eye_free_count--;
    for (; place_index < guard_eye_free_count; place_index++) {
        free_places[place_index] = free_places[place_index + 1];
    }
    task->draw = eye_draw;
    task->destructor = eye_destroy;
#else
    /* @bug memory: writes the place through null at 0x8c507456 where
     * eye_alloc fails. it returns null where syMalloc does, at 0x8c507758 */
    eye_alloc(task);
    task->draw = eye_draw;
    task->destructor = eye_destroy;
    EYE(task)->place =
        free_places[(int)((Float)rand() * guard_eye_free_count * RAND_SCALE)];
    for (read = 0, write = 0; read < guard_eye_free_count; read++, write++) {
        if (free_places[read] == EYE(task)->place) {
            read++;
        }
        /* @bug memory: reads the word past free_places at 0x8c507478 where
         * all EYE_PLACES places are free and the eye takes the last */
        free_places[write] = free_places[read];
    }
    guard_eye_free_count--;
    eye_place(task);
    collision_init(task, shapes, EYE_SHAPES, COLLISION_OBJECT);
#endif
    eye_start_mode(task, EYE_WAIT);
    EYE_SLOT(task) = slot;
    guard_eye_count++;
}

static void eye_destroy(struct task *task)
{
    eye_free(task);
    guard_eye_count--;
}

static void eye_step(struct task *task)
{
    struct task_work *work = task->work;

    eye_step_mode(task);
    if (EYE(task)->mode != EYE_FADE && GUARDS_STAND_DOWN) {
        eye_request_mode(task, EYE_FADE);
    } else {
        collision_register(task);
    }
    guard_eye_positions[EYE_SLOT(task)] = work->pos;
}

static void eye_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (EYE(task)->mode == EYE_FADE) {
        njRotateY(NULL, EYE(task)->spin);
        njScale(NULL, EYE(task)->scale_across, EYE(task)->scale_up,
                EYE(task)->scale_across);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
        njSetCnkBlendMode(PREMULTIPLIED_BLEND);
        njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
        constant_material_set(EYE(task)->alpha, EYE(task)->alpha,
                              EYE(task)->alpha, EYE(task)->alpha);
        njSetTexture(&guard_eye_texlist);
        njCnkEasyDrawModel(&guard_eye_model);
        njSetCnkBlendMode(ALPHA_BLEND);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    } else {
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        njSetTexture(&guard_eye_texlist);
        if (models_built) {
            njDirectDrawObject(models[EYE_BODY].built);
        } else {
            njCnkEasyDrawModel(&guard_eye_model);
        }
    }
    if (EYE(task)->mode == EYE_FIRE) {
        scale.z = EYE(task)->beam_length;
        /* @hack spelling: the two assignments chained, where two statements
         * would do */
        scale.x = scale.y = 1.0F;
        njTranslate(NULL, 0.0F, EYE_BEAM_Y, EYE(task)->beam_z + EYE_BEAM_Z);
        draw_control_save();
        draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
        njSetCnkBlendMode(ADDITIVE_BLEND);
        njSetTexture(&guard_eye_beam_texlist);
        quarters_model_draw_scaled_v(&guard_eye_beam_model, &scale);
        njSetCnkBlendMode(ALPHA_BLEND);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        draw_control_restore();
    }
    njPopMatrixEx();
}

static struct eye_state *eye_alloc(struct task *task)
{
    task->extra.p = syMalloc(sizeof(struct eye_state));
    if (task->extra.p == NULL) {
        return NULL;
    }
    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    EYE(task)->spin = EYE(task)->spin_speed = 0;
    EYE(task)->scale_across = 1.0F;
    EYE(task)->scale_across_speed = 0.0F;
    EYE(task)->scale_up = 1.0F;
    EYE(task)->scale_up_speed = 0.0F;
    EYE(task)->alpha = 1.0F;
    EYE(task)->alpha_speed = 0.0F;
    EYE(task)->beam_length = 0.0F;
    EYE(task)->beam_z = 0.0F;
    EYE(task)->beam_speed = 0.0F;
    return task->extra.p;
}

static void eye_free(struct task *task)
{
    if (task->extra.p) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void eye_start_mode(struct task *task, enum eye_mode mode)
{
    EYE(task)->mode = mode;
    EYE(task)->next_mode = NO_NEXT_MODE;
    EYE(task)->phase = 0;
    EYE(task)->steps = 0;
    mode_starts[EYE(task)->mode](task);
}

static void eye_step_mode(struct task *task)
{
    /* @hack spelling: eye_start_mode's work repeated here, where a call of it
     * would do */
    if (EYE(task)->next_mode != NO_NEXT_MODE) {
        EYE(task)->mode = EYE(task)->next_mode;
        EYE(task)->next_mode = NO_NEXT_MODE;
        EYE(task)->phase = 0;
        EYE(task)->steps = 0;
        mode_starts[EYE(task)->mode](task);
    }
    mode_steps[EYE(task)->mode](task);
    EYE(task)->steps++;
}

static void eye_wait_start(struct task *task)
{
}

static void eye_wait_step(struct task *task)
{
    eye_request_mode(task, EYE_FIRE);
}

static void eye_fire_start(struct task *task)
{
    sound_play(BEAM_SOUND, task, 0, 0);
    sound_set_volume(task, SOUND_ANY_ID, beam_volume);
}

static void eye_fire_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (EYE(task)->phase) {
    case BEAM_GROWING:
        direction_angles_xy(work->pos.x - player_work[0]->pos.x,
                            work->pos.y - player_work[0]->pos.y,
                            work->pos.z - player_work[0]->pos.z, &work->ang.x,
                            &work->ang.y);
        /* @hack spelling: the two assignments chained, where two statements
         * would do */
        work->collision->shapes[EYE_BEAM].size_y = EYE(task)->beam_length =
            njDistanceP2P(&work->pos, &player_work[0]->pos) * EYE(task)->steps *
            BEAM_SPEED_SHARE;
        if (EYE(task)->steps == BEAM_GROW_STEPS) {
            EYE(task)->phase++;
            EYE(task)->beam_speed =
                njDistanceP2P(&work->pos, &player_work[0]->pos) *
                BEAM_SPEED_SHARE;
        }
        break;
    case BEAM_SLIDING:
        /* @hack spelling: the speed negated and added, where the speed
         * subtracted would do */
        EYE(task)->beam_z += -EYE(task)->beam_speed;
        break;
    }
    if (EYE(task)->steps == BEAM_LAST_STEP) {
        eye_request_mode(task, EYE_FADE);
    }
}

static void eye_fade_start(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    work->collision->shapes[EYE_BEAM].size_y = EYE(task)->beam_length = 0.0F;
    EYE(task)->spin_speed = fade_spin_speed;
    EYE(task)->scale_across_speed = fade_across_speed;
    EYE(task)->scale_up_speed = fade_up_speed;
    EYE(task)->alpha_speed = fade_alpha_speed;
}

static void eye_fade_step(struct task *task)
{
    EYE(task)->spin_speed += fade_spin_accel;
    EYE(task)->scale_across_speed += fade_across_accel;
    EYE(task)->scale_up_speed += fade_up_accel;
    EYE(task)->alpha_speed += fade_alpha_accel;
    EYE(task)->spin += EYE(task)->spin_speed;
    EYE(task)->scale_across += EYE(task)->scale_across_speed;
    EYE(task)->scale_up += EYE(task)->scale_up_speed;
    EYE(task)->alpha += EYE(task)->alpha_speed;
    if (EYE(task)->scale_across < 0.0F) {
        EYE(task)->scale_across = 0.0F;
    }
    if (EYE(task)->alpha < 0.0F) {
        EYE(task)->alpha = 0.0F;
        task_kill(task);
    }
}

void guard_eye_find_places(struct task *guard)
{
    struct ray_query ray;
    NJS_POINT3 pos;
    int i;

    guard_eye_free_count = 0;
    ray.length =
        njSqrt((ring_height * ring_height) + (ring_radius * ring_radius));
    ray.origin = player_work[0]->pos;
    ray.dir.y = ring_height;
    for (i = 0; i < EYE_PLACES; i++) {
        quarters_polar_xz(i * PLACE_ANGLE, ring_radius, &ray.dir.x, &ray.dir.z);
        pos.x = player_work[0]->pos.x + ray.dir.x;
        pos.y = player_work[0]->pos.y + ray.dir.y;
        pos.z = player_work[0]->pos.z + ray.dir.z;
        if (polygon_cast_ray(&ray) ||
            njDistanceP2P(&camera_views[camera_current_index]->pos, &pos) <
                EYE_CAMERA_CLEARANCE) {
            place_free[i] = FALSE;
        } else {
            place_free[i] = TRUE;
            free_places[guard_eye_free_count] = i;
            guard_eye_free_count++;
        }
    }
}

static void eye_place(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR offset;
    /* @hack spelling: angle, a local for the place's turn, where its
     * expression passed to quarters_polar_xz would do */
    Angle angle = EYE(task)->place * PLACE_ANGLE;

    quarters_polar_xz(angle, ring_radius, &offset.x, &offset.z);
    offset.y = ring_height;
    work->pos.x = player_work[0]->pos.x + offset.x;
    work->pos.y = player_work[0]->pos.y + offset.y;
    work->pos.z = player_work[0]->pos.z + offset.z;
    direction_angles_xy(offset.x, offset.y, offset.z, &work->ang.x,
                        &work->ang.y);
    work->ang.z = 0;
}
