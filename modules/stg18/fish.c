#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "object/set.h"
#include "player/player.h"
#include "stg18/fish.h"
#include "stg18/fish/text.inc"
#include "stg18/lagoon_util.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"

/* @hack type: the union, whose axis array lets fish_scatter, fish_advance and
 * fish_swim walk pos and velocity, where NJS_POINT3 and its fields would do */
union fish_vector {
    NJS_POINT3 point;
    Float axis[3];
};

/* a fish of a school, in its task's motion. pos and velocity lie in the
 * school's axes, which its work's pos and ang.y place */
struct fish {
    union fish_vector pos, velocity;
    /* the yaw toward the camera, in the school's axes, as the swim last eased
     * it */
    Angle camera_yaw;
    Sint16 yaw, target_yaw;
    Sint16 pitch, target_pitch;
    /* holds the fish's kind in FISH_KIND_BITS, and the bit FISH_DISTANT */
    Sint16 kind;
    /* the steps the fish counts down while it swims, before it may choose a
     * course again */
    Sint16 wait;
};

enum school_count { FISH_TOTAL, FISH_VISIBLE, FISH_COUNTS };

/* a school's record, in its task's extra. counts[FISH_TOTAL] is how many fish
 * lie in task->motion. the first counts[FISH_VISIBLE] of them swim and draw.
 * radius is the largest of scl's coordinates */
struct fish_school {
    /* @hack type: the counts as an array, where a field for each would do */
    Sint32 counts[FISH_COUNTS];
    Float radius;
};
#define SCHOOL(task) ((struct fish_school *)(task)->extra.p)

/* the bits of ang.z that hold the school's count */
#define FISH_COUNT_BITS 0xff

/* a fish of kind n draws lagoon_fish_model_n under lagoon_fish_textures_n. a
 * kind picked as FISH_RARE is picked again, so a fish is FISH_RARE a ninth of
 * the time */
enum fish_kind { FISH_KIND_1 = 1, FISH_KIND_2, FISH_KIND_3 };
#define FISH_KINDS 3
#define FISH_RARE FISH_KIND_3
#define FISH_KIND_BITS 3

/* a bit of a fish's kind. a choice that neither flees nor wanders sets it
 * where the fish lies far from the camera. far is a squared distance over
 * DISTANT_RANGE2 from the camera's offset from pos. a swimming fish's step
 * clears the bit where that distance is below DISTANT_RANGE2 */
#define FISH_DISTANT 4
#define DISTANT_RANGE2 10000.0F

/* every fish swims and draws while the camera lies within the school's radius
 * plus FULL_RANGE of its pos. beyond, the count that swims falls with the
 * distance, to none at the radius plus SHOWN_RANGE */
#define FULL_RANGE 105.0F
#define SHOWN_RANGE 400.0F
#define LEAVE_SLACK 10.0F

/* each step a swimming fish's speed along y gains RISE, then keeps Y_DRAG of
 * itself. its speeds along x and z keep XZ_DRAG of themselves */
#define RISE 0.00001F
#define Y_DRAG 0.85F
#define XZ_DRAG 0.98F

/* a fish's tick is the school's timer, work->timer, plus how many swimming
 * fish follow it in task->motion. a fish whose wait has counted down to 0
 * chooses a course on a multiple of CHOICE_STEPS. every swimming fish aims on
 * a multiple of AIM_STEPS. an aim sets the fish's targets from its speed, and
 * eases camera_yaw.
 *
 * a fish whose FISH_DISTANT stays set is distant. on a multiple of
 * CHOICE_STEPS, a distant fish only moves and counts its wait down. on its
 * other ticks, the choice's test and the aim's read its tick shifted right by
 * DISTANT_TICK_SHIFT */
#define CHOICE_STEPS 8
#define AIM_STEPS 4
#define DISTANT_TICK_SHIFT 3

/* a choosing fish flees where its squared distance from player 0's offset from
 * pos is below FLEE_RANGE2. it swims straight from that offset at FLEE_SPEED.
 * it then waits up to FLEE_WAIT steps */
#define FLEE_RANGE2 900.0F
#define FLEE_SPEED 0.7F
#define FLEE_WAIT 100.0F

/* a choosing fish that doesn't flee wanders where its squared speed is at
 * most STILL_SPEED2. its speed along each axis gains up to WANDER_SPREAD / 2
 * either way, at random. it then waits up to WANDER_WAIT steps. a new fish
 * waits up to WANDER_WAIT steps too */
#define STILL_SPEED2 0.0001F
#define WANDER_SPREAD 1.4F
#define WANDER_WAIT 200.0F

/* one choice in NUDGE_ODDS nudges the fish. its speeds along x and z gain up
 * to NUDGE_XZ / 2 either way. its speed along y gains up to NUDGE_Y / 2 either
 * way */
#define NUDGE_ODDS 15.0F
#define NUDGE_XZ 0.04F
#define NUDGE_Y 0.02F

/* each step, a swimming fish's pitch and yaw close TURN_EASE of their gaps
 * to their targets. a distant fish's don't on a multiple of CHOICE_STEPS. an
 * aim takes the targets from the fish's speed only where its speed along z
 * isn't 0. an aim closes CAMERA_EASE of camera_yaw's gap to the yaw toward the
 * camera */
#define TURN_EASE 0.12F
#define CAMERA_EASE 0.8F

/* a fish's model is flat, a quad in its y-z plane. two yaws show the quad face
 * on to the camera, a quarter turn either side of camera_yaw. the draw turns
 * the fish by CAMERA_LEAN of its yaw's gap to the nearer of the two */
#define CAMERA_LEAN 0.2F
#define FISH_SCALE 7.5F

#define CAMERA camera_views[camera_current_index]

/* RANDOM_CENTERED() lies from -0.5 up to 0.5, and RANDOM_CENTERED() *
 * UNIT_SPREAD from -1 up to 1 */
#define RANDOM_CENTERED() (RANDOM_UNIT() - 0.5F)
#define UNIT_SPREAD 2.0F

static NJS_TEXNAME lagoon_fish_textures_1_names[1] = {
#include "stg18/fish/lagoon_fish_textures_1_names.inc"
};

static NJS_TEXLIST lagoon_fish_textures_1 = {
#include "stg18/fish/lagoon_fish_textures_1.inc"
};

static Sint16 lagoon_fish_model_1_polygons[25] = {
#include "stg18/fish/lagoon_fish_model_1_polygons.inc"
};

static Sint32 lagoon_fish_model_1_vertices[19] = {
#include "stg18/fish/lagoon_fish_model_1_vertices.inc"
};

static NJS_CNK_MODEL lagoon_fish_model_1_model = {
#include "stg18/fish/lagoon_fish_model_1_model.inc"
};

static NJS_CNK_OBJECT lagoon_fish_model_1[1] = {
#include "stg18/fish/lagoon_fish_model_1.inc"
};

static NJS_TEXNAME lagoon_fish_textures_2_names[1] = {
#include "stg18/fish/lagoon_fish_textures_2_names.inc"
};

static NJS_TEXLIST lagoon_fish_textures_2 = {
#include "stg18/fish/lagoon_fish_textures_2.inc"
};

static Sint16 lagoon_fish_model_2_polygons[25] = {
#include "stg18/fish/lagoon_fish_model_2_polygons.inc"
};

static Sint32 lagoon_fish_model_2_vertices[19] = {
#include "stg18/fish/lagoon_fish_model_2_vertices.inc"
};

static NJS_CNK_MODEL lagoon_fish_model_2_model = {
#include "stg18/fish/lagoon_fish_model_2_model.inc"
};

static NJS_CNK_OBJECT lagoon_fish_model_2[1] = {
#include "stg18/fish/lagoon_fish_model_2.inc"
};

static NJS_TEXNAME lagoon_fish_textures_3_names[1] = {
#include "stg18/fish/lagoon_fish_textures_3_names.inc"
};

static NJS_TEXLIST lagoon_fish_textures_3 = {
#include "stg18/fish/lagoon_fish_textures_3.inc"
};

static Sint16 lagoon_fish_model_3_polygons[25] = {
#include "stg18/fish/lagoon_fish_model_3_polygons.inc"
};

static Sint32 lagoon_fish_model_3_vertices[19] = {
#include "stg18/fish/lagoon_fish_model_3_vertices.inc"
};

static NJS_CNK_MODEL lagoon_fish_model_3_model = {
#include "stg18/fish/lagoon_fish_model_3_model.inc"
};

static NJS_CNK_OBJECT lagoon_fish_model_3[1] = {
#include "stg18/fish/lagoon_fish_model_3.inc"
};

NJS_TEXLIST *lagoon_fish_texlists[] = {
    &lagoon_fish_textures_1,
    &lagoon_fish_textures_2,
    &lagoon_fish_textures_3,
    NULL,
};

static void fish_destroy(struct task *task);
static void fish_swim(struct task *task);
static void fish_draw(struct task *task);

/* the yaw, in the school's axes, that heads a fish toward the camera. pos is
 * the fish's place in those axes, and work the school's work */
static Angle yaw_to_camera(NJS_POINT3 *pos, struct task_work *work)
{
    NJS_POINT3 world;

    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, pos, &world);
    njPopMatrixEx();
    return (Angle)(atan2f(CAMERA->pos.x - world.x, CAMERA->pos.z - world.z) *
                   (Float)ANGLE_PER_RADIAN) -
           work->ang.y;
}

static void fish_scatter(struct task *task)
{
    struct task_work *work = task->work;
    int count = SCHOOL(task)->counts[FISH_TOTAL];
    struct fish *fish = task->motion.p;
    Float width = work->scl.x;
    Float height = work->scl.y;
    Float depth = work->scl.z;
    int i;
    int kind;
    /* @hack spelling: axis, a pointer that walks pos's and velocity's axes,
     * where their fields would do */
    Float *axis;

    for (i = 0; i < count; ++i, ++fish) {
        axis = fish->pos.axis;
        *axis++ = width * RANDOM_CENTERED() * UNIT_SPREAD;
        *axis++ = height * RANDOM_CENTERED() * UNIT_SPREAD;
        *axis = depth * RANDOM_CENTERED() * UNIT_SPREAD;
        axis = fish->velocity.axis;
        *axis++ = 0.0F;
        *axis++ = 0.0F;
        *axis = 0.0F;
        kind = (rand() % FISH_KINDS) + FISH_KIND_1;
        if (kind == FISH_RARE) {
            kind = (rand() % FISH_KINDS) + FISH_KIND_1;
        }
        fish->kind = kind;
        fish->wait = rand() * (WANDER_WAIT * RAND_SCALE);
        fish->pitch = 0;
        fish->target_pitch = 0;
        fish->yaw = 0;
        fish->target_yaw = 0;
        /* @bug uninitialized: camera_yaw keeps what syMalloc left. fish_draw
         * reads it at 0x8c509940 to lean each fish. fish_swim's aims read it
         * at 0x8c5098a0, and ease it from there toward the camera */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it starts camera_yaw at 0 */
        fish->camera_yaw = 0;
#endif
    }
}

void lagoon_fish_init(struct task *task)
{
    struct task_work *work = task->work;
    int count = work->ang.z & FISH_COUNT_BITS;
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    Float radius = work->scl.x;
    Float range;
    if (radius < work->scl.y) {
        radius = work->scl.y;
    }
    if (radius < work->scl.z) {
        radius = work->scl.z;
    }
#else
    /* @hack type: extent, a pointer that reads scl's three fields as an
     * array, where the fields would do */
    Float *extent = &work->scl.x;
    Float radius = *extent++;
    Float range;
    if (radius < *extent++) {
        radius = extent[-1];
    }
    if (radius < *extent) {
        radius = *extent;
    }
#endif
    SCHOOL(task)->radius = radius;
    /* @hack spelling: the school's radius read back after its store, where
     * radius would do */
    range = SCHOOL(task)->radius + SHOWN_RANGE;
    if (!placement_try_leave_by(task, (range * range) + LEAVE_SLACK)) {
        task->draw = fish_draw;
        task->step = fish_swim;
        task->destructor = fish_destroy;
        SCHOOL(task)->counts[FISH_TOTAL] = count;
        SCHOOL(task)->counts[FISH_VISIBLE] = 0;
        task->motion.p = syMalloc(sizeof(struct fish) * count);
#ifdef NONMATCHING
        /* this side's task kills itself where the heap has no room */
        if (!task->motion.p) {
            task->draw = NULL;
            task->destructor = NULL;
            task->step = task_kill;
            return;
        }
#endif
        fish_scatter(task);
        work->timer = 0;
    }
}

static void fish_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    syFree(owner->motion.p);
    owner->motion.p = NULL;
}

#pragma inline(fish_advance)
/* @hack spelling: height before width, where width, height and depth in the
 * order of x, y and z would do */
static void fish_advance(struct fish *fish, Float height, Float width,
                         Float depth)
{
    /* @hack spelling: pos and velocity, pointers that walk the fish's axes
     * forward to read them and back to write them, where their fields would
     * do */
    Float *pos;
    Float *velocity;
    Float x;
    Float y;
    Float z;
    Float vx;
    Float vy;
    Float vz;

    pos = fish->pos.axis;
    velocity = fish->velocity.axis;
    x = *pos++;
    y = *pos++;
    z = *pos;
    vx = *velocity++;
    vy = *velocity++;
    vz = *velocity;
    /* @hack spelling: x = x + vx, where x += vx would do, as y and z do */
    x = x + vx;
    y += vy;
    z += vz;
    if (njAbs(x) >= width) {
        x -= vx;
        vx = -vx;
    }
    if (njAbs(y) >= height) {
        y -= vy;
        vy = -vy;
    }
    if (njAbs(z) >= depth) {
        z -= vz;
        vz = -vz;
    }
    vy = (vy + RISE) * Y_DRAG;
    vz *= XZ_DRAG;
    vx *= XZ_DRAG;
    *velocity = vz;
    *--velocity = vy;
    *--velocity = vx;
    *pos = z;
    *--pos = y;
    *--pos = x;
}

static void fish_swim(struct task *task)
{
    struct task_work *work = task->work;
    struct fish *fish;
    int remaining;
    int tick;
    Sint16 wait;
    /* pitch_delta, yaw_delta and turn each hold the gap between two angles. a
     * Sint16 wraps each gap to the shorter way round */
    Sint16 pitch_delta;
    Sint16 yaw_delta;
    /* @hack spelling: pos and velocity, pointers that walk a fish's axes,
     * where their fields would do */
    Float *pos;
    Float *velocity;
    Float distance;
    Float fade;
    Float radius;
    Float player_offset_x;
    Float player_offset_y;
    Float player_offset_z;
    Float camera_offset_x;
    Float camera_offset_y;
    Float camera_offset_z;
    Float width;
    Float height;
    Float depth;
    Float x;
    Float y;
    Float z;
    Float vx;
    Float vy;
    Float vz;
    /* @hack spelling: origin_z, a local for work->pos.z, where work->pos.z
     * read in place would do */
    Float origin_z;
    Float dx;
    Float dy;
    Float dz;
    Float player_distance2;
    Float speed2;
    Float scale;
    Float forward_speed;
    Sint16 turn;
    int pitch_step;
    int yaw_step;
#ifdef NONMATCHING
#else
    int unused;
#endif

    if (placement_try_leave_by(task, ((SCHOOL(task)->radius + SHOWN_RANGE) *
                                      (SCHOOL(task)->radius + SHOWN_RANGE)) +
                                         LEAVE_SLACK)) {
        return;
    }
    distance = njDistanceP2P(&CAMERA->pos, &work->pos);
    if (distance > SCHOOL(task)->radius + FULL_RANGE) {
        /* @hack spelling: the assignment to radius inside the expression,
         * where the school's radius read in place would do */
        fade = ((radius = SCHOOL(task)->radius) - distance + SHOWN_RANGE) *
               (1.0F / (SHOWN_RANGE - FULL_RANGE));
        if (fade < 0.0F) {
            fade = 0.0F;
        }
        /* @hack spelling: fade reused for the count after the fraction,
         * where the product converted in place would do */
        fade = SCHOOL(task)->counts[FISH_TOTAL] * fade;
        /* @hack type: the count converted through Sint16, where a plain
         * conversion would do: it is at most counts[FISH_TOTAL], 255 or less */
        SCHOOL(task)->counts[FISH_VISIBLE] = (Sint16)fade;
    } else {
        SCHOOL(task)->counts[FISH_VISIBLE] = SCHOOL(task)->counts[FISH_TOTAL];
    }
    if (SCHOOL(task)->counts[FISH_VISIBLE] > 0) {
        remaining = SCHOOL(task)->counts[FISH_VISIBLE];
        /* the offsets of player 0 and the camera from pos are taken in the
         * world's axes. a fish's pos is in the school's axes, which ang.y
         * turns. the swim compares the two as if ang.y were 0 */
        player_offset_x = player_work[0]->pos.x - work->pos.x;
        player_offset_y = player_work[0]->pos.y - work->pos.y;
        origin_z = work->pos.z;
        player_offset_z = player_work[0]->pos.z - origin_z;
        camera_offset_x = CAMERA->pos.x - work->pos.x;
        camera_offset_y = CAMERA->pos.y - work->pos.y;
        camera_offset_z = CAMERA->pos.z - origin_z;
        width = work->scl.x;
        height = work->scl.y;
        depth = work->scl.z;
        fish = task->motion.p;
        for (; remaining--; ++fish) {
            tick = work->timer + remaining;
            wait = fish->wait - 1;
            if (wait <= 0) {
                wait = 0;
            }
            fish_advance(fish, height, width, depth);
#ifdef NONMATCHING
#else
            /* @hack redundant: two tests of camera_offset_z and
             * camera_offset_y that set unused, which nothing reads */
            unused = camera_offset_z ? CAMERA->ang.x : 0;
            unused = camera_offset_y ? CAMERA->ang.x : 0;
            (void)unused;
#endif
            if (fish->kind & FISH_DISTANT) {
                Float cx;
                Float cy;
                Float cz;

                pos = fish->pos.axis;
                cx = camera_offset_x - *pos++;
                cy = camera_offset_y - *pos++;
                cz = camera_offset_z - *pos;
                if ((cx * cx) + (cy * cy) + (cz * cz) < DISTANT_RANGE2) {
                    fish->kind &= ~FISH_DISTANT;
                } else {
                    if (!(tick & (CHOICE_STEPS - 1))) {
                        fish->wait = wait;
                        continue;
                    }
                    /* @hack spelling: tick = tick >> DISTANT_TICK_SHIFT, where
                     * tick >>= DISTANT_TICK_SHIFT would do */
                    tick = tick >> DISTANT_TICK_SHIFT;
                }
            }
            if (!(tick & (CHOICE_STEPS - 1))) {
                pos = fish->pos.axis;
                x = *pos++;
                y = *pos++;
                z = *pos;
                velocity = fish->velocity.axis;
                vx = *velocity++;
                vy = *velocity++;
                vz = *velocity;
                if (!wait) {
                    dx = x - player_offset_x;
                    dy = y - player_offset_y;
                    dz = z - player_offset_z;
                    player_distance2 = (dx * dx) + (dy * dy) + (dz * dz);
                    if (player_distance2 < FLEE_RANGE2) {
                        wait = rand() * (FLEE_WAIT * RAND_SCALE);
                        /* @bug arithmetic: makes a fleeing fish's speed NaN
                         * where the fish lies at player 0's offset. FLEE_SPEED
                         * over their distance of 0, the FDIV at 0x8c5096c2, is
                         * infinite, and 0 times that is NaN */
#ifdef NONMATCHING
                        /* this side repairs the bug tagged above: it sends a
                         * fish at player 0's offset along +z */
                        if (player_distance2 == 0.0F) {
                            dz = 1.0F;
                            player_distance2 = 1.0F;
                        }
#endif
                        scale = FLEE_SPEED / njSqrt(player_distance2);
                        dx *= scale;
                        dy *= scale;
                        dz *= scale;
                        vx = dx;
                        vy = dy;
                        vz = dz;
                    } else {
                        speed2 = (vx * vx) + (vy * vy) + (vz * vz);
                        if (speed2 <= STILL_SPEED2) {
                            wait = rand() * (WANDER_WAIT * RAND_SCALE);
                            vx += RANDOM_CENTERED() * WANDER_SPREAD;
                            vy += RANDOM_CENTERED() * WANDER_SPREAD;
                            vz += RANDOM_CENTERED() * WANDER_SPREAD;
                        } else {
                            Float camera_distance2;

                            x -= camera_offset_x;
                            y -= camera_offset_y;
                            /* @hack spelling: z = z - camera_offset_z, where
                             * z -= camera_offset_z would do, as x and y do */
                            z = z - camera_offset_z;
                            camera_distance2 = (x * x) + (y * y) + (z * z);
                            if (camera_distance2 > DISTANT_RANGE2) {
                                fish->kind |= FISH_DISTANT;
                            }
                        }
                    }
                    if (RANDOM_UNIT() < 1.0F / NUDGE_ODDS) {
                        vx += RANDOM_CENTERED() * NUDGE_XZ;
                        vy += RANDOM_CENTERED() * NUDGE_Y;
                        vz += RANDOM_CENTERED() * NUDGE_XZ;
                    }
                }
                velocity = fish->velocity.axis;
                *velocity = vx;
                velocity++;
                *velocity = vy;
                velocity++;
                *velocity = vz;
            }
            velocity = fish->velocity.axis;
            vx = *velocity++;
            vy = *velocity++;
            vz = *velocity;
            forward_speed = njAbs(vz);
            if (forward_speed > 0.0F && !(tick & (AIM_STEPS - 1))) {
                fish->target_pitch = -(Angle)(atan2f(vy, forward_speed) *
                                              (Float)ANGLE_PER_RADIAN);
                fish->target_yaw =
                    (Angle)(atan2f(vx, vz) * (Float)ANGLE_PER_RADIAN);
            }
            pitch_delta = fish->target_pitch - fish->pitch;
            yaw_delta = fish->target_yaw - fish->yaw;
            /* @hack spelling: the pitch's and the yaw's eases, each a copy
             * into scale and a *= of its own, where (int)(pitch_delta *
             * TURN_EASE) and (int)(yaw_delta * TURN_EASE) would do */
            scale = pitch_delta;
            scale *= TURN_EASE;
            pitch_step = (int)scale;
            scale = yaw_delta;
            scale *= TURN_EASE;
            yaw_step = (int)scale;
            fish->pitch += pitch_step;
#ifdef NONMATCHING
#else
            /* @hack redundant: a block no path reaches, with a test of
             * pitch_step */
            if (0) {
                if (pitch_step) {
                }
            }
#endif
            fish->yaw += yaw_step;
            if (!(tick & (AIM_STEPS - 1))) {
                turn = yaw_to_camera(&fish->pos.point, work) - fish->camera_yaw;
                /* @hack spelling: the ease as a copy into scale and a *= of
                 * its own, where (int)(turn * CAMERA_EASE) would do */
                scale = turn;
                scale *= CAMERA_EASE;
                fish->camera_yaw += (int)scale;
            }
            fish->wait = wait;
        }
    }
#ifdef NONMATCHING
#else
#define KEEP_LOCAL(value)                                                      \
    /* @hack redundant: a test of value, in a block no path reaches */         \
    if (0) {                                                                   \
        if (value) {                                                           \
        }                                                                      \
    }
    KEEP_LOCAL(radius);
    KEEP_LOCAL(remaining);
    KEEP_LOCAL(player_offset_x);
    KEEP_LOCAL(player_offset_y);
    KEEP_LOCAL(camera_offset_x);
#undef KEEP_LOCAL
#endif
    ++work->timer;
}

static void fish_draw(struct task *task)
{
    struct task_work *work = task->work;
    int count = SCHOOL(task)->counts[FISH_VISIBLE];
    struct fish *fish;
    /* the gap from the fish's yaw to camera_yaw, then to the nearer face-on
     * yaw. a Sint16 wraps it to the shorter way round */
    Sint16 delta;
    Float turn;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    fish = task->motion.p;
    njPushMatrixEx();
    while (count--) {
        /* @hack spelling: delta written and read through angle, where delta
         * itself would do */
        Sint16 *angle = &delta;

        *angle = fish->camera_yaw - fish->yaw;
        if (*angle > 0) {
            *angle -= QUARTER_TURN;
        } else {
            *angle += QUARTER_TURN;
        }
        /* @hack spelling: the lean as a copy into turn and a *= of its own,
         * where (Angle)(delta * CAMERA_LEAN) would do */
        turn = *angle;
        turn *= CAMERA_LEAN;
        njTranslateV(NULL, &fish->pos.point);
        njRotateY(NULL, fish->yaw + (Angle)turn);
        njRotateX(NULL, fish->pitch);
        switch (fish->kind & FISH_KIND_BITS) {
        case FISH_KIND_1:
            njSetTexture(&lagoon_fish_textures_1);
            lagoon_model_draw_scaled(lagoon_fish_model_1->model, FISH_SCALE);
            break;
        case FISH_KIND_2:
            njSetTexture(&lagoon_fish_textures_2);
            lagoon_model_draw_scaled(lagoon_fish_model_2->model, FISH_SCALE);
            break;
        case FISH_KIND_3:
            njSetTexture(&lagoon_fish_textures_3);
            lagoon_model_draw_scaled(lagoon_fish_model_3->model, FISH_SCALE);
            break;
        default:
            break;
        }
        matrix_load_parent();
        ++fish;
    }
    njPopMatrixEx();
    njPopMatrixEx();
}
