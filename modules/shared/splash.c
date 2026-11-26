/* splash: particles of water, and the tasks that ring the players' feet with
 * them in shallow water */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/world.h"
#include "effect/particle.h"
#include "player/player.h"
#include "shared/splash.h"
#include "shared/splash/text.inc"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a splash starts turned up to TURN degrees either way. it turns up to SPIN
 * degrees a step either way */
#define TURN 40.0F
#define SPIN 0.25F

/* the players who can have a splash task: 0 and 1 */
#define SPLASH_PLAYERS 2
/* the task level of a splash task made without a parent */
#define SPLASH_TASK_LEVEL 1

/* a splash task's work keeps its player's slot in tick. its timer counts the
 * steps since its last ring. its flags keep the player's work's flags of the
 * step before */
#define SLOT(work) ((work)->tick)
/* the player's animation frame of the step before */
#define LAST_FRAME(work) ((work)->scl.x)
/* the player's vertical speed of the step before, without its sign */
#define LAST_FALL_SPEED(work) ((work)->scl.y)

/* the color of a ring's splashes, by stage */
#define AQUATIC_MINE_COLOR 0xff404458
#define GREEN_FOREST_COLOR 0xff182928
#define OTHER_COLOR 0xff8080a0
/* splash_violet_create's color */
#define VIOLET_COLOR 0xffa090ff

/* a ring's splash moves out from the center at RING_OUT times its scale. it
 * rises at RING_RISE times its scale, on top of the ring's spd */
#define RING_OUT 0.07F
#define RING_RISE 0.03F

/* a player splashes only in water less than SHALLOW deep. the player's
 * ground record shows the water: its above_flags hold a bit of WORLD_FLAT.
 * the depth is the record's above_y less its below_y */
#define SHALLOW 3.0F
/* the sound of a ring of splashes */
#define SPLASH_SOUND 0x7000
/* a ring's splashes take SPEED_SHARE of the player's speed in x and z */
#define SPEED_SHARE 0.3F
/* a landing ring needs more than LANDING_WAIT steps since the last ring, or
 * since the task began. a running ring and a cycle ring need more than
 * RING_WAIT */
#define LANDING_WAIT 12
#define RING_WAIT 4
/* the splashes in a landing ring, in a running ring and in a cycle ring */
#define LANDING_SPLASHES 12
#define RUNNING_SPLASHES 5
#define CYCLE_SPLASHES 6
/* a running ring needs the player at RUNNING_SPEED or more along its own x.
 * it comes as the animation starts over. it also comes as the frame climbs
 * from below RUNNING_FRAME to RUNNING_FRAME or past it. the animation starts
 * over where the frame is 0 or more and below the last step's */
#define RUNNING_SPEED 0.1F
#define RUNNING_FRAME 15.0F
/* a running ring's splashes rise at splash_rise plus a random kick. the kick
 * is KICK times a number from KICK_BASE to KICK_BASE + 1 */
#define KICK 0.002F
#define KICK_BASE 9.0F
/* a cycle ring comes as the animation of a player in PLAYER_MODE_BRAKE or
 * PLAYER_MODE_UNK_1B starts over. its splashes rise at CYCLE_RISE, at the
 * size CYCLE_SCALE */
#define CYCLE_RISE 0.1F
#define CYCLE_SCALE 2.1F

static int splash_step(struct particle_group *group, struct particle *particle);
static void splash_task_destroy(struct task *task);
static void splash_task_step(struct task *task);

static NJS_TEXNAME splash_texnames[SPLASH_FRAMES] = {
#include "shared/splash/splash_texnames.inc"
};
NJS_TEXLIST splash_texlist = {
#include "shared/splash/splash_texlist.inc"
};

/* clang-format off */
static struct particle_group splash_group = {
    PARTICLE_BLEND_ADD, &splash_texlist, 0, SPLASH_FRAMES,
    0.2F, 0.93F, -0.025F, -0.04F, splash_step, 75000.0F, NULL, 0, NULL, NULL,
};
static struct particle_group splash_tinted_group = {
    PARTICLE_BLEND_ADD, &splash_texlist, 0, SPLASH_FRAMES,
    0.25F, 0.99F, -0.015F, 0.02F, splash_step, 75000.0F, NULL, 0, NULL, NULL,
};
static struct particle_group splash_violet_group = {
    PARTICLE_BLEND_ADD, &splash_texlist, 0, SPLASH_FRAMES,
    0.5F, 0.998F, -0.03F, 0.01F, splash_step, 75000.0F, NULL, 0, NULL, NULL,
};
/* clang-format on */
/* the color of every ring's splashes, which splash_task_create sets */
static Uint32 splash_color = AQUATIC_MINE_COLOR;
/* each player's splash task, or null */
static struct task *splash_tasks[SPLASH_PLAYERS] = {NULL, NULL};
/* the size of the splashes of a landing ring and of a running ring */
static Float splash_scale = 2.1F;
/* a landing ring's splashes rise at splash_rise times the speed the player
 * fell at, and at most splash_rise_max */
static Float splash_rise = 0.15F;
static Float splash_rise_max = 0.25F;
/* a ring's radius is splash_radius. a landing ring's is larger by
 * splash_radius_per_speed times the speed the player fell at */
static Float splash_radius = 1.4F;
static Float splash_radius_per_speed = 0.3F;

struct particle *splash_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale)
{
    struct particle *splash;

    splash = particle_create(&splash_group);
    if (splash != NULL) {
        splash->pos = *pos;
        splash->spd = *spd;
        splash->scale = scale;
        splash->ang = RANDOM_ANGLE(TURN);
        splash->ang_spd = RANDOM_ANGLE(SPIN);
        /* @hack redundant: the frame set to 0, which particle_create gave it
         * already from the group's first_frame */
        splash->frame = 0.0F;
    }
    return splash;
}

struct particle *splash_violet_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                      Float scale)
{
    struct particle *splash;

    splash = particle_create(&splash_violet_group);
    if (splash != NULL) {
        splash->pos = *pos;
        splash->spd = *spd;
        splash->scale = scale;
        splash->ang = RANDOM_ANGLE(TURN);
        splash->ang_spd = RANDOM_ANGLE(SPIN);
        splash->frame = (Float)rand() * (2.0F * RAND_SCALE);
        splash->color = VIOLET_COLOR;
    }
    return splash;
}

/* make a ring's splash in splash_color. it grows, and lasts 32 steps */
static struct particle *splash_tinted_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                             Float scale)
{
    struct particle *splash;

    splash = particle_create(&splash_tinted_group);
    if (splash != NULL) {
        splash->pos = *pos;
        splash->spd = *spd;
        splash->scale = scale;
        splash->ang = RANDOM_ANGLE(TURN);
        splash->ang_spd = RANDOM_ANGLE(SPIN);
        /* @hack redundant: the frame set to 0, which particle_create gave it
         * already from the group's first_frame */
        splash->frame = 0.0F;
        splash->color = splash_color;
    }
    return splash;
}

/* particle_move, but a frame that reaches the group's frames stays there.
 * particle_move sets it back to frames - 1 */
static int splash_step(struct particle_group *group, struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        return 0;
    }
    return 1;
}

/* make count splashes of size scale on a level ring of radius about pos.
 * each moves at spd, plus the speeds RING_OUT and RING_RISE give it. count
 * must be from 1 to 255 */
static void splash_ring(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                        Float scale, int count)
{
    NJS_VECTOR out = {0.0F, RING_RISE, RING_OUT};
    NJS_POINT3 offset = {0.0F, 0.0F, 0.0F};
    NJS_POINT3 point;
    NJS_VECTOR splash_spd;
    Angle gap;
    Angle ang;
    /* @hack type: i, a Uint8, where an int would do for a count below 256 */
    Uint8 i;

    gap = FULL_TURN / count;
    ang = 0;
    offset.z = radius;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    out.y *= scale;
    out.z *= scale;
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i < count; i++) {
        ang += (Float)gap * (Float)rand() * (0.5F * RAND_SCALE);
        /* @bug logic: the turns add up twice, so the splashes don't lie a gap
         * apart round the ring. ang already sums the gaps, and each njRotateY
         * adds ang to the matrix's turn, which the loop never sets back (the
         * JSR to njRotateY at 0x324 of the object's code) */
        njRotateY(NULL, ang);
        njCalcPoint(NULL, &offset, &point);
        njCalcVector(NULL, &out, &splash_spd);
        point.x += pos->x;
        point.y += pos->y;
        point.z += pos->z;
        splash_spd.x += spd->x;
        splash_spd.y += spd->y;
        splash_spd.z += spd->z;
        splash_tinted_create(&point, &splash_spd, scale);
        ang += gap;
    }
    njPopMatrixEx();
}

struct task *splash_task_create(int player, struct task *parent)
{
    struct task *task;
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work;

    if (player < SPLASH_PLAYERS && splash_tasks[player] == NULL) {
        switch (stage_current_id()) {
        case STAGE_AQUATIC_MINE:
            splash_color = AQUATIC_MINE_COLOR;
            break;
        case STAGE_GREEN_FOREST:
            splash_color = GREEN_FOREST_COLOR;
            break;
        default:
            splash_color = OTHER_COLOR;
            break;
        }
        if (parent != NULL) {
            task = task_create_child(TASK_WORK, splash_task_step, parent);
        } else {
            task = task_create(TASK_WORK, SPLASH_TASK_LEVEL, splash_task_step,
                               "SplashExec");
        }
        if (task != NULL) {
            work = task->work;
            task->destructor = splash_task_destroy;
            SLOT(work) = player;
            splash_tasks[player] = task;
            return task;
        }
    }
    return NULL;
}

/* empty the player's entry of splash_tasks where it holds task, so the
 * player can have another */
static void splash_task_destroy(struct task *task)
{
    if (splash_tasks[SLOT(task->work)] == task) {
        splash_tasks[SLOT(task->work)] = NULL;
    }
}

static void splash_task_step(struct task *task)
{
    struct task_work *work = task->work;
    int player = SLOT(work);
    struct task_work *player_task_work = player_work[player];
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float frame;
    Float radius;

    if (player_task_work == NULL) {
        task_destroy(task);
        return;
    }
    frame = player_records[player]->anim.frame;
    if ((player_records[player]->ground.above_flags & WORLD_FLAT) &&
        player_records[player]->ground.above_y -
                player_records[player]->ground.below_y <
            SHALLOW) {
        if (((work->flags ^ player_task_work->flags) & player_task_work->flags &
             PLAYER_WORK_ON_GROUND) &&
            work->timer > LANDING_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= SPEED_SHARE;
            spd.y = splash_rise * LAST_FALL_SPEED(work);
            spd.z *= SPEED_SHARE;
            radius = splash_radius +
                     (splash_radius_per_speed * LAST_FALL_SPEED(work));
            if (spd.y > splash_rise_max) {
                spd.y = splash_rise_max;
                /* @bug logic: a landing ring's radius never gains the rise's
                 * excess over splash_rise_max. the excess is taken after the
                 * rise is cut to splash_rise_max, so it is 0 (the FSUB at
                 * 0x5f6 of the object's code) */
                radius += spd.y - splash_rise_max;
            }
            pos = player_task_work->pos;
            pos.y = player_records[player]->ground.above_y;
            splash_ring(&pos, &spd, radius, splash_scale, LANDING_SPLASHES);
            work->timer = 0;
            sound_play_at(SPLASH_SOUND, NULL, 0, 0, &pos);
        }
        if ((player_task_work->flags & PLAYER_WORK_ON_GROUND) &&
            (player_task_work->mode == PLAYER_MODE_RUN ||
             player_task_work->mode == PLAYER_MODE_UNK_19 ||
             player_task_work->mode == PLAYER_MODE_UNK_0E ||
             player_task_work->mode == PLAYER_MODE_UNK_20) &&
            player_records[player]->spd.x >= RUNNING_SPEED &&
            ((frame >= 0.0F && LAST_FRAME(work) > frame) ||
             (frame >= RUNNING_FRAME && LAST_FRAME(work) < RUNNING_FRAME &&
              frame > LAST_FRAME(work))) &&
            work->timer > RING_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= SPEED_SHARE;
            spd.y = splash_rise +
                    ((((Float)rand() * RAND_SCALE) + KICK_BASE) * KICK);
            spd.z *= SPEED_SHARE;
            /* @hack spelling: radius set to splash_radius, where
             * splash_radius passed in place would do */
            radius = splash_radius;
            pos = player_task_work->pos;
            pos.y = player_records[player]->ground.above_y;
            splash_ring(&pos, &spd, radius, splash_scale, RUNNING_SPLASHES);
            work->timer = 0;
            sound_play_at(SPLASH_SOUND, NULL, 0, 0, &pos);
        } else if ((player_task_work->flags & PLAYER_WORK_ON_GROUND) &&
                   (player_task_work->mode == PLAYER_MODE_BRAKE ||
                    player_task_work->mode == PLAYER_MODE_UNK_1B) &&
                   frame >= 0.0F && LAST_FRAME(work) > frame &&
                   work->timer > RING_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= SPEED_SHARE;
            spd.y = CYCLE_RISE;
            spd.z *= SPEED_SHARE;
            /* @hack spelling: radius set to splash_radius, where
             * splash_radius passed in place would do */
            radius = splash_radius;
            pos = player_task_work->pos;
            pos.y = player_records[player]->ground.above_y;
            splash_ring(&pos, &spd, radius, CYCLE_SCALE, CYCLE_SPLASHES);
            work->timer = 0;
            sound_play_at(SPLASH_SOUND, NULL, 0, 0, &pos);
        }
    }
    work->flags = player_task_work->flags;
    LAST_FALL_SPEED(work) = njAbs(player_motion[player]->spd.y);
    work->timer++;
    LAST_FRAME(work) = frame;
}
