/* the warp hole a flute stand opens, KDWARPHOLE in a stage's object list */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "draw/color.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/knudai.h"
#include "shared/knudai_warp_hole.h"
#include "shared/ring_sparkle.h"
#include "shared/warp_hole.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#include "shared/knudai_warp_hole/text.inc"

#define KNUDAI_ID(work, mask) (((work)->ang.x & (mask)) % KNUDAI_IDS)

/* the warp hole's modes, in its work's mode */
enum knudai_warp_hole_mode { MODE_SHUT, MODE_OPENING, MODE_OPEN };
/* the alpha each step of MODE_OPENING adds, up to 1. the radius grows with
 * it, up to open_radius */
#define OPEN_STEP 0.0125F
/* the placement's state once the warp hole starts to open. a task whose
 * placement's state isn't 0 starts open */
#define STATE_OPENED 1

#define CATCH_RANGE 1.5F /* in units of open_radius */
/* the work's tick holds off a catch. each step that finds a player near
 * either end sets it to CATCH_COOLDOWN. each step without a carry takes 1
 * from it, and an end catches only while it is 0 */
#define CATCH_COOLDOWN 60
#define ANIM_CARRIED 15
#define NO_PLAYER (-1)

/* a carry's phases, in the work's submode. CARRY_PULL draws the player into
 * the end that caught it, and shrinks the player. CARRY_GROW grows it back
 * to a scale of 1 at the other end. no end catches while a player is carried,
 * so the two ends share the submode */
enum knudai_warp_hole_carry { CARRY_PULL, CARRY_GROW };
/* each pull step keeps PULL_KEEP of the player's offset from the end. the
 * pull ends once the offset is below ARRIVE_RANGE */
#define PULL_KEEP 0.96F
#define ARRIVE_RANGE 0.5F
/* the work's timer holds the carried player's spin about x and z. a pull spins
 * the player a turn every SPIN_STEPS steps. a grow takes the player's scale to
 * 1 over GROW_STEPS steps. it spins the player slower as the scale nears 1 */
#define SPIN_STEPS 120
#define GROW_STEPS 120
#define GROW_STEP (1.0F / GROW_STEPS)

#define OPEN_SOUND 0x101f
#define PULL_SOUND 0x1017
#define GROW_SOUND 0x1018
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 0
#define SOUND_DURATION 30 /* in sound updates */

/* the points of each end's swirl, a hole of shared/warp_hole.h. the in end's
 * points swirl in to its core, and the out end's out from it */
#define SWIRL_POINTS 254

/* the draw lays a steady quad and a pulse quad over each end. the pulse
 * quad's alpha dips by up to PULSE_DEPTH of the steady quad's */
#define QUAD_TEXTURE 0
#define PULSE_RATE 256 /* in Angle units a step */
#define PULSE_DEPTH 0.3F
/* a quad is white, with its alpha scaled to 0 to ALPHA_MAX in the color's top
 * byte */
#define ALPHA_MAX 0xff
#define WHITE_RGB 0xffffff

/* the warp hole's record, in task->motion */
struct knudai_warp_hole {
    Float alpha; /* from 0, shut, to 1, open */
    Float radius; /* of both ends, from 0 to open_radius */
    /* the in and out ends' swirls, or null where an end has none. a swirl's
     * destructor sets its field to null */
    struct task *in_swirl;
    struct task *out_swirl;
    /* the slots of the players caught at the in and out ends, or NO_PLAYER */
    Sint16 in_player;
    Sint16 out_player;
};

#define HOLE(task) ((struct knudai_warp_hole *)(task)->motion.p)

static void knudai_warp_hole_destroy(struct task *task);
static void knudai_warp_hole_step(struct task *task);
static void knudai_warp_hole_draw(struct task *task);

#define TEXTURES 1
static NJS_TEXNAME knudai_warp_hole_texname[TEXTURES] = {
#include "shared/knudai_warp_hole/knudai_warp_hole_texname.inc"
};
static NJS_TEXLIST knudai_warp_hole_texlist = {
#include "shared/knudai_warp_hole/knudai_warp_hole_texlist.inc"
};
NJS_TEXLIST *knudai_warp_hole_texlists[4] = {
#include "shared/knudai_warp_hole/knudai_warp_hole_texlists.inc"
};
/* the quads' spins, in Angle units a step. an end's two quads spin opposite
 * ways. each quad spins opposite to the same quad at the other end */
static Angle steady_spin = 768;
static Angle pulse_spin = 592;
static Float open_radius = 10.0F; /* an open end's radius */
/* warp_hole_create's scale for a swirl, for each unit of radius */
static Float swirl_scale = 4.0F;
/* the open size: warp_hole_create's sprite size for a swirl at open_radius */
static Float swirl_size = 0.4F;

void knudai_warp_hole_init(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: radius, a pointer to open_radius, where open_radius
     * read in place would do */
    Float *radius;
    /* @hack spelling: alpha, a local for 1.0F, where the constant in place
     * would do */
    Float alpha;

    if (task->placement != NULL && placement_get_state(task)) {
        work->mode = MODE_OPEN;
    }
    if (work->mode != MODE_OPEN && placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct knudai_warp_hole));
    if (task->motion.p == NULL) {
        return;
    }
    radius = &open_radius;
    alpha = 1.0F;
    if (work->mode == MODE_OPEN) {
        HOLE(task)->radius = *radius;
        HOLE(task)->alpha = alpha;
        /* @hack redundant: a call of knudai_get_pos that no path reaches,
         * with radius as its pos */
        if (0) {
            knudai_get_pos(0, (NJS_POINT3 *)radius);
        }
    }
    /* @bug logic: a warp hole that starts open never draws its quads. the
     * block above gives it the open alpha, which in MODE_OPEN only the draw
     * reads, but only the step that starts the opening sets the draw. the
     * store of the alpha and the store of null to the draw are at 0x54 and
     * 0x5e of the object's code */
    task->draw = NULL;
    task->step = knudai_warp_hole_step;
    task->destructor = knudai_warp_hole_destroy;
    /* @hack redundant: the stores of null to in_swirl and out_swirl, which
     * syCalloc has zeroed */
    HOLE(task)->in_swirl = NULL;
    HOLE(task)->out_swirl = NULL;
    work->tick = 0;
    HOLE(task)->in_player = NO_PLAYER;
    HOLE(task)->out_player = NO_PLAYER;
}

static void knudai_warp_hole_destroy(struct task *task)
{
    if (HOLE(task)->in_player >= 0 &&
        player_work[HOLE(task)->in_player] != NULL) {
        player_work[HOLE(task)->in_player]->scl.x = 1.0F;
        player_work[HOLE(task)->in_player]->scl.y = 1.0F;
        player_work[HOLE(task)->in_player]->scl.z = 1.0F;
    }
    if (HOLE(task)->out_player >= 0 &&
        player_work[HOLE(task)->out_player] != NULL) {
        player_work[HOLE(task)->out_player]->scl.x = 1.0F;
        player_work[HOLE(task)->out_player]->scl.y = 1.0F;
        player_work[HOLE(task)->out_player]->scl.z = 1.0F;
    }
    if (HOLE(task)->in_swirl != NULL) {
        task_destroy(HOLE(task)->in_swirl);
    }
    if (HOLE(task)->out_swirl != NULL) {
        task_destroy(HOLE(task)->out_swirl);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void knudai_warp_hole_step(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;
    Float distance;
    int near;

    if (work->mode == MODE_SHUT && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    /* @hack redundant: a default ahead of the cases, which does nothing */
    default:
        break;
    case MODE_SHUT: {
        NJS_POINT3 knudai_pos;

        /* @hack redundant: a test of knudai_get_pos, which returns 1 for
         * every id knudai_played returns 1 for. nothing reads the pos it
         * writes */
        if (knudai_played(KNUDAI_ID(work, KNUDAI_ID_MASK)) == 1 &&
            knudai_get_pos(KNUDAI_ID(work, KNUDAI_ID_MASK), &knudai_pos)) {
            task->draw = knudai_warp_hole_draw;
            HOLE(task)->alpha = 0.0F;
            HOLE(task)->radius = 0.0F;
            work->mode = MODE_OPENING;
            if (task->placement != NULL) {
                placement_set_state(task, STATE_OPENED);
            }
        }
        break;
    }
    case MODE_OPENING:
        HOLE(task)->alpha += OPEN_STEP;
        HOLE(task)->radius += open_radius * OPEN_STEP;
        if (HOLE(task)->alpha > 1.0F) {
            HOLE(task)->alpha = 1.0F;
            HOLE(task)->radius = open_radius;
            work->mode = MODE_OPEN;
        }
        sound_play_at_for(OPEN_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                          SOUND_DURATION, &work->pos);
        /* @bug logic: a swirl made while the warp hole opens draws at the
         * open size, swirl_size, until the next step scales it by radius /
         * open_radius. warp_hole_create takes swirl_size unscaled at 0x2f6
         * and 0x342 of the object's code */
        if (HOLE(task)->in_swirl != NULL) {
            WARP_HOLE_SCALE(HOLE(task)->in_swirl) =
                HOLE(task)->radius * swirl_scale;
            WARP_HOLE_SIZE(HOLE(task)->in_swirl) =
                swirl_size * (HOLE(task)->radius / open_radius);
        } else {
            warp_hole_create(&work->pos, HOLE(task)->radius * swirl_scale,
                             SWIRL_POINTS, FALSE, swirl_size,
                             &HOLE(task)->in_swirl);
        }
        if (HOLE(task)->out_swirl != NULL) {
            WARP_HOLE_SCALE(HOLE(task)->out_swirl) =
                HOLE(task)->radius * swirl_scale;
            WARP_HOLE_SIZE(HOLE(task)->out_swirl) =
                swirl_size * (HOLE(task)->radius / open_radius);
        } else {
            warp_hole_create(&work->scl, HOLE(task)->radius * swirl_scale,
                             SWIRL_POINTS, TRUE, swirl_size,
                             &HOLE(task)->out_swirl);
        }
        break;
    case MODE_OPEN:
        if (HOLE(task)->in_swirl != NULL) {
            WARP_HOLE_SCALE(HOLE(task)->in_swirl) =
                HOLE(task)->radius * swirl_scale;
            WARP_HOLE_SIZE(HOLE(task)->in_swirl) =
                swirl_size * (HOLE(task)->radius / open_radius);
        } else {
            warp_hole_create(&work->pos, HOLE(task)->radius * swirl_scale,
                             SWIRL_POINTS, FALSE, swirl_size,
                             &HOLE(task)->in_swirl);
        }
        if (HOLE(task)->out_swirl != NULL) {
            WARP_HOLE_SCALE(HOLE(task)->out_swirl) =
                HOLE(task)->radius * swirl_scale;
            WARP_HOLE_SIZE(HOLE(task)->out_swirl) =
                swirl_size * (HOLE(task)->radius / open_radius);
        } else {
            warp_hole_create(&work->scl, HOLE(task)->radius * swirl_scale,
                             SWIRL_POINTS, TRUE, swirl_size,
                             &HOLE(task)->out_swirl);
        }
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((near = player_near(&work->pos, open_radius * CATCH_RANGE))) {
            if (!work->tick && HOLE(task)->in_player < 0) {
                HOLE(task)->in_player = near - 1;
                work->timer = 0;
                player_request(HOLE(task)->in_player, PLAYER_REQUEST_ANIM,
                               ANIM_CARRIED);
                work->submode = CARRY_PULL;
            }
            work->tick = CATCH_COOLDOWN;
        }
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((near = player_near(&work->scl, open_radius * CATCH_RANGE))) {
            if (!work->tick && HOLE(task)->out_player < 0) {
                HOLE(task)->out_player = near - 1;
                work->timer = 0;
                /* @hack spelling: the submode set ahead of the request,
                 * where after it, as at the in end, would do */
                work->submode = CARRY_PULL;
                player_request(HOLE(task)->out_player, PLAYER_REQUEST_ANIM,
                               ANIM_CARRIED);
            }
            work->tick = CATCH_COOLDOWN;
        }
        /* @bug memory: reads a carried player's work through null once the
         * player's slot empties. the destructor tests that work for null at
         * 0xa4 of the object's code. the step reads the in end's player's
         * work untested at 0x4b4 of the object's code, and the out end's at
         * 0x7ec of the object's code */
#ifdef NONMATCHING
        /* this side repairs the tag above: either end lets go of a carried
         * player whose slot has emptied */
        if (HOLE(task)->in_player >= 0 &&
            player_work[HOLE(task)->in_player] == NULL) {
            HOLE(task)->in_player = NO_PLAYER;
        }
        if (HOLE(task)->out_player >= 0 &&
            player_work[HOLE(task)->out_player] == NULL) {
            HOLE(task)->out_player = NO_PLAYER;
        }
#endif
        if (HOLE(task)->in_player >= 0) {
            if (game_state.cleared == 1 ||
                player_work[HOLE(task)->in_player]->mode == PLAYER_MODE_HURT ||
                (player_records[HOLE(task)->in_player]->flags &
                 PLAYER_MISSED)) {
                player_work[HOLE(task)->in_player]->scl.x = 1.0F;
                player_work[HOLE(task)->in_player]->scl.y = 1.0F;
                player_work[HOLE(task)->in_player]->scl.z = 1.0F;
                player_request(HOLE(task)->in_player, PLAYER_REQUEST_RELEASE,
                               0);
                HOLE(task)->in_player = NO_PLAYER;
            } else if (work->submode != CARRY_PULL) {
                /* @hack redundant: offset, a zero vector added to scl, where
                 * scl alone would do */
                NJS_VECTOR offset;

                offset.x = 0.0F;
                offset.y = 0.0F;
                offset.z = 0.0F;
                scale = player_work[HOLE(task)->in_player]->scl.x;
                if (scale < 1.0F) {
                    work->timer +=
                        (int)((1.0F - scale) * ((Float)FULL_TURN / SPIN_STEPS));
                    scale += GROW_STEP;
                    if (scale > 1.0F) {
                        scale = 1.0F;
                    }
                    player_work[HOLE(task)->in_player]->ang.y =
                        camera_views[camera_current_index]->ang.y;
                    player_work[HOLE(task)->in_player]->ang.x = work->timer;
                    player_work[HOLE(task)->in_player]->ang.z = work->timer;
                    player_set_pos(
                        HOLE(task)->in_player, work->scl.x + offset.x,
                        work->scl.y + offset.y, work->scl.z + offset.z);
                    player_work[HOLE(task)->in_player]->scl.x = scale;
                    player_work[HOLE(task)->in_player]->scl.y = scale;
                    player_work[HOLE(task)->in_player]->scl.z = scale;
                    sound_play_at_for(GROW_SOUND, work, SOUND_PRIORITY,
                                      SOUND_VOLUME, SOUND_DURATION, &work->scl);
                } else {
                    player_set_pos(
                        HOLE(task)->in_player, work->scl.x + offset.x,
                        work->scl.y + offset.y, work->scl.z + offset.z);
                    player_work[HOLE(task)->in_player]->scl.x = scale;
                    player_work[HOLE(task)->in_player]->scl.y = scale;
                    player_work[HOLE(task)->in_player]->scl.z = scale;
                    player_request(HOLE(task)->in_player,
                                   PLAYER_REQUEST_RELEASE, 0);
                    HOLE(task)->in_player = NO_PLAYER;
                }
            } else {
                NJS_VECTOR offset;

                work->timer += FULL_TURN / SPIN_STEPS;
                /* @hack spelling: the offset as -pos plus the player's pos,
                 * where the player's pos less pos would do */
                offset.x =
                    -work->pos.x + player_work[HOLE(task)->in_player]->pos.x;
                offset.y =
                    -work->pos.y + player_work[HOLE(task)->in_player]->pos.y;
                offset.z =
                    -work->pos.z + player_work[HOLE(task)->in_player]->pos.z;
                /* @hack spelling: the offset scaled in statements of its own,
                 * where the product in each assignment would do */
                offset.x *= PULL_KEEP;
                offset.y *= PULL_KEEP;
                offset.z *= PULL_KEEP;
                distance = njScalor(&offset);
                player_work[HOLE(task)->in_player]->ang.y =
                    camera_views[camera_current_index]->ang.y;
                player_work[HOLE(task)->in_player]->ang.x = work->timer;
                player_work[HOLE(task)->in_player]->ang.z = work->timer;
                if (distance < ARRIVE_RANGE) {
                    work->submode = CARRY_GROW;
                    work->timer = 0;
                } else {
                    if (distance < open_radius) {
                        /* @hack spelling: distance reused for the player's
                         * scale, where a local of its own would do */
                        distance /= open_radius;
                        player_work[HOLE(task)->in_player]->scl.x = distance;
                        player_work[HOLE(task)->in_player]->scl.y = distance;
                        player_work[HOLE(task)->in_player]->scl.z = distance;
                    }
                    /* @hack spelling: offset.x ahead of pos.x, where pos.x
                     * first, as in y and z, would do */
                    player_set_pos(
                        HOLE(task)->in_player, offset.x + work->pos.x,
                        work->pos.y + offset.y, work->pos.z + offset.z);
                    sound_play_at_for(PULL_SOUND, work, SOUND_PRIORITY,
                                      SOUND_VOLUME, SOUND_DURATION, &work->pos);
                }
            }
        }
        if (HOLE(task)->out_player >= 0) {
            if (game_state.cleared == 1 ||
                player_work[HOLE(task)->out_player]->mode == PLAYER_MODE_HURT ||
                (player_records[HOLE(task)->out_player]->flags &
                 PLAYER_MISSED)) {
                player_work[HOLE(task)->out_player]->scl.x = 1.0F;
                player_work[HOLE(task)->out_player]->scl.y = 1.0F;
                player_work[HOLE(task)->out_player]->scl.z = 1.0F;
                player_request(HOLE(task)->out_player, PLAYER_REQUEST_RELEASE,
                               0);
                HOLE(task)->out_player = NO_PLAYER;
            } else if (work->submode != CARRY_PULL) {
                /* @hack redundant: offset, a zero vector added to pos, where
                 * pos alone would do */
                NJS_VECTOR offset;

                offset.x = 0.0F;
                offset.y = 0.0F;
                offset.z = 0.0F;
                scale = player_work[HOLE(task)->out_player]->scl.x;
                if (scale < 1.0F) {
                    work->timer +=
                        (int)((1.0F - scale) * ((Float)FULL_TURN / SPIN_STEPS));
                    scale += GROW_STEP;
                    if (scale > 1.0F) {
                        scale = 1.0F;
                    }
                    player_work[HOLE(task)->out_player]->ang.y =
                        camera_views[camera_current_index]->ang.y;
                    player_work[HOLE(task)->out_player]->ang.x = work->timer;
                    player_work[HOLE(task)->out_player]->ang.z = work->timer;
                    player_set_pos(
                        HOLE(task)->out_player, work->pos.x + offset.x,
                        work->pos.y + offset.y, work->pos.z + offset.z);
                    player_work[HOLE(task)->out_player]->scl.x = scale;
                    player_work[HOLE(task)->out_player]->scl.y = scale;
                    player_work[HOLE(task)->out_player]->scl.z = scale;
                    sound_play_at_for(GROW_SOUND, work, SOUND_PRIORITY,
                                      SOUND_VOLUME, SOUND_DURATION, &work->pos);
                } else {
                    player_set_pos(
                        HOLE(task)->out_player, work->pos.x + offset.x,
                        work->pos.y + offset.y, work->pos.z + offset.z);
                    player_work[HOLE(task)->out_player]->scl.x = scale;
                    player_work[HOLE(task)->out_player]->scl.y = scale;
                    player_work[HOLE(task)->out_player]->scl.z = scale;
                    player_request(HOLE(task)->out_player,
                                   PLAYER_REQUEST_RELEASE, 0);
                    HOLE(task)->out_player = NO_PLAYER;
                }
            } else {
                NJS_VECTOR offset;

                work->timer += FULL_TURN / SPIN_STEPS;
                /* @hack spelling: the offset as -scl plus the player's pos,
                 * where the player's pos less scl would do */
                offset.x =
                    -work->scl.x + player_work[HOLE(task)->out_player]->pos.x;
                offset.y =
                    -work->scl.y + player_work[HOLE(task)->out_player]->pos.y;
                offset.z =
                    -work->scl.z + player_work[HOLE(task)->out_player]->pos.z;
                /* @hack spelling: the offset scaled in statements of its own,
                 * where the product in each assignment would do */
                offset.x *= PULL_KEEP;
                offset.y *= PULL_KEEP;
                offset.z *= PULL_KEEP;
                distance = njScalor(&offset);
                player_work[HOLE(task)->out_player]->ang.y =
                    camera_views[camera_current_index]->ang.y;
                player_work[HOLE(task)->out_player]->ang.x = work->timer;
                player_work[HOLE(task)->out_player]->ang.z = work->timer;
                /* unlike the in end's arrival, this one leaves the timer at the
                 * pull's angle. the grow then spins on from there */
                if (distance < ARRIVE_RANGE) {
                    work->submode = CARRY_GROW;
                } else {
                    if (distance < open_radius) {
                        /* @hack spelling: distance reused for the player's
                         * scale, where a local of its own would do */
                        distance /= open_radius;
                        player_work[HOLE(task)->out_player]->scl.x = distance;
                        player_work[HOLE(task)->out_player]->scl.y = distance;
                        player_work[HOLE(task)->out_player]->scl.z = distance;
                    }
                    /* @hack spelling: offset.x ahead of scl.x, where scl.x
                     * first, as in y and z, would do */
                    player_set_pos(
                        HOLE(task)->out_player, offset.x + work->scl.x,
                        work->scl.y + offset.y, work->scl.z + offset.z);
                    sound_play_at_for(PULL_SOUND, work, SOUND_PRIORITY,
                                      SOUND_VOLUME, SOUND_DURATION, &work->scl);
                }
            }
        }
        if (HOLE(task)->in_player < 0 && HOLE(task)->out_player < 0 &&
            work->tick) {
            work->tick--;
        }
        break;
    }
}

static void knudai_warp_hole_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 alpha;
    Uint32 pulse_alpha;
    Uint32 color;
    Uint32 pulse_color;

    /* @hack redundant: a block no path reaches, kept for its five strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)KNUDAI_WARP_HOLE_DEBUG_ID;
        (void)KNUDAI_WARP_HOLE_DEBUG_YAW;
        (void)KNUDAI_WARP_HOLE_DEBUG_EXIT_X;
        (void)KNUDAI_WARP_HOLE_DEBUG_EXIT_Y;
        (void)KNUDAI_WARP_HOLE_DEBUG_EXIT_Z;
    }
    alpha = (Uint32)(HOLE(task)->alpha * ALPHA_MAX);
    if (alpha > ALPHA_MAX) {
        alpha = ALPHA_MAX;
    }
    color = (alpha << ALPHA_SHIFT) | WHITE_RGB;
    pulse_alpha =
        (Uint32)(HOLE(task)->alpha *
                 (1.0F -
                  (njAbs(njSin(game_state.steps * PULSE_RATE)) * PULSE_DEPTH)) *
                 ALPHA_MAX);
    if (pulse_alpha > ALPHA_MAX) {
        pulse_alpha = ALPHA_MAX;
    }
    pulse_color = (pulse_alpha << ALPHA_SHIFT) | WHITE_RGB;
    njFogDisable();
    njSetTexture(&knudai_warp_hole_texlist);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    particle_draw_quad(&work->pos, QUAD_TEXTURE, QUARTER_TURN,
                       game_state.steps * steady_spin, HOLE(task)->radius,
                       HOLE(task)->radius, color, PARTICLE_BLEND_ADD, 0.0F,
                       0.0F);
    particle_draw_quad(&work->pos, QUAD_TEXTURE, QUARTER_TURN,
                       -(game_state.steps * pulse_spin), HOLE(task)->radius,
                       HOLE(task)->radius, pulse_color,
                       PARTICLE_BLEND_ADD | PARTICLE_FLIP_U, 0.0F, 0.0F);
    particle_draw_quad(&work->scl, QUAD_TEXTURE, QUARTER_TURN,
                       -(game_state.steps * steady_spin), HOLE(task)->radius,
                       HOLE(task)->radius, color, PARTICLE_BLEND_ADD, 0.0F,
                       0.0F);
    particle_draw_quad(&work->scl, QUAD_TEXTURE, QUARTER_TURN,
                       game_state.steps * pulse_spin, HOLE(task)->radius,
                       HOLE(task)->radius, pulse_color,
                       PARTICLE_BLEND_ADD | PARTICLE_FLIP_U, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}
