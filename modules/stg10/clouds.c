/* the clouds the missile rises through: puffs and sheets of cloud in a band
 * about camera 0's height */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "draw/color.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg10/clouds.h"
#include "stg10/clouds/text.inc"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the clouds' one mode, in the work's mode. nothing sets another */
enum clouds_mode { MODE_RUN };

/* task_create's level for the clouds' task */
#define CLOUDS_LEVEL 1
#define PUFFS 60
#define LAYERS 3
/* the clouds' center is the work's pos. the band's bottom lies
 * band_half_height below it, and never below y -BAND_BOTTOM_DEPTH. the center
 * takes camera 0's height, raised where that height would put the bottom
 * lower */
#define BAND_BOTTOM_DEPTH 320.0F
/* a puff or a layer fades over FADE_SPAN at each end of the band */
#define FADE_SPAN 200.0F
/* cloud_puff_place puts a puff at_top up to TOP_DEPTH below the top it's
 * given. the step gives a top up to RESPAWN_DEPTH below the band's */
#define TOP_DEPTH 5.0F
#define RESPAWN_DEPTH 20.0F
/* the work's timer fades the clouds in: it climbs by FADE_IN_STEP a step,
 * up to FADE_IN_END */
#define FADE_IN_STEP 3
#define FADE_IN_END 255
/* the layers lie LAYER_SPACING times band_half_height apart, the first that
 * far above the band's bottom */
#define LAYER_SPACING 0.5F
/* each layer's turn, spin and scrolls start at random, spread over these
 * spans about 0 */
#define LAYER_TURN_SPAN (10.0F * ANGLE_PER_DEGREE)
#define LAYER_SPIN_SPAN 2.0F
#define LAYER_SCROLL_SPAN 512.0F
/* a puff inside its clearance is pushed out to it, along its line from the
 * clouds' center. a puff whose squared distance from the center is below
 * CENTER_DIST2_MIN has no such line. it goes back to the top instead */
#define CENTER_DIST2_MIN 0.01F
/* a puff that leaves the band at one end comes back WRAP_INSET inside the
 * other */
#define WRAP_INSET 1.0F
/* rand() times RAND_TURN spans a whole turn, which a puff's heading takes */
#define RAND_TURN 2.0F
/* a puff draws only where its alpha, out of 255, is above PUFF_ALPHA_MIN */
#define PUFF_ALPHA_MIN 5
/* particle_draw_quad's edge_angle for every puff, as the particle template
 * has it */
#define PUFF_EDGE_ANGLE QUARTER_TURN
/* the puffs take clouds_puff_texlist's textures in turn */
#define PUFF_TEXTURES 3
/* the turn every puff's quad draws at */
#define PUFF_TURN 0x1800
/* a layer's tint and a puff's draw color: WHITE, with an alpha shifted into
 * the top byte */
#define WHITE 0x00ffffff

/* a puff of cloud, which draws as a quad. pos is from the clouds' center */
struct cloud_puff {
    NJS_POINT3 pos;
    Uint32 color;
    Float half_width;
    Float half_height;
    Float alpha; /* from 0 to 1, by the puff's place in the band */
};

/* a sheet of cloud, which draws as clouds_layer_model */
struct cloud_layer {
    NJS_POINT3 pos; /* from the clouds' center */
    Angle ang;
    /* what each step adds to ang. LAYER_SPIN_SPAN gives it a value from -1 up
     * to 1, which truncates to 0. only a rand() of 0 makes it -1 */
    Angle spin;
    Uint32 color;
    Sint16 scroll_u;
    Sint16 scroll_v;
};

/* the clouds' record, in task->motion */
struct clouds {
    struct cloud_puff *puffs;
    Sint32 puff_count;
    struct cloud_layer *layers;
    Sint32 layer_count;
    struct task **owner; /* clouds_create's owner, or null */
};
#define CLOUDS(task) ((struct clouds *)(task)->motion.p)

/* Cloud sheet, puff textures and the UV strips used for the two-axis drift. */

static NJS_TEXNAME clouds_puff_textures[3] = {
#include "stg10/clouds/clouds_puff_textures.inc"
};

static NJS_TEXLIST clouds_puff_texlist = {
#include "stg10/clouds/clouds_puff_texlist.inc"
};

static NJS_TEXNAME clouds_layer_textures[1] = {
#include "stg10/clouds/clouds_layer_textures.inc"
};

static NJS_TEXLIST clouds_layer_texlist = {
#include "stg10/clouds/clouds_layer_texlist.inc"
};

static Sint16 clouds_layer_polygons[677] = {
#include "stg10/clouds/clouds_layer_polygons.inc"
};

static Sint32 clouds_layer_vertices[487] = {
#include "stg10/clouds/clouds_layer_vertices.inc"
};

static NJS_CNK_MODEL clouds_layer_model[1] = {
#include "stg10/clouds/clouds_layer_model.inc"
};

static NJS_CNK_OBJECT clouds_layer_object = {
#include "stg10/clouds/clouds_layer_object.inc"
};

static Sint16 clouds_layer_uvs[462] = {
#include "stg10/clouds/clouds_layer_uvs.inc"
};

static struct texture_anim clouds_layer_anim = {
#include "stg10/clouds/clouds_layer_anim.inc"
};

/* the half height of the band the clouds fill about their center */
static Float band_half_height = 2500.0F;
static Float dat_8c5e8858 = 3100.0F;
/* a puff's half width and half height: the least, and the most added at
 * random */
static Float puff_half_width_min = 100.0F;
static Float puff_half_height_min = 60.0F;
static Float puff_half_width_range = 200.0F;
static Float puff_half_height_range = 130.0F;
/* a puff keeps this far from the clouds' center, past its size. its size here
 * is the mean of its half width and half height */
static Float puff_clearance = 100.0F;
/* a puff's color is a random blend of these two */
static Uint32 puff_color_low = 0x40ffb0ff;
static Uint32 puff_color_high = 0x60d0ffb0;
/* what each step adds to the layers' and the puffs' heights */
static Float layer_drift = -10.0F;
static Float puff_drift = -40.0F;
NJS_TEXLIST *clouds_texlists[3] = {
#include "stg10/clouds/clouds_texlists.inc"
};

static void clouds_step(struct task *task);
static void clouds_draw(struct task *task);
static void clouds_destroy(struct task *task);

/* set *pos to camera 0's pos, raised to keep the band's bottom at or above y
 * -BAND_BOTTOM_DEPTH */
static void clouds_camera_pos(NJS_POINT3 *pos)
{
    *pos = camera_views[0]->pos;
    if (pos->y - band_half_height < -BAND_BOTTOM_DEPTH) {
        pos->y = band_half_height - BAND_BOTTOM_DEPTH;
    }
}

/* put puff at random about the clouds' center. it lies up to range from the
 * center across, and up to range above or below. a puff at_top lies instead up
 * to TOP_DEPTH below range. its color and size are random, and its alpha 0 */
/* @hack type: at_top a Sint8, where a Bool would do */
static void cloud_puff_place(struct cloud_puff *puff, Float range, Sint8 at_top)
{
    NJS_POINT3 pos;
    Angle ang;
    Float dist;

    ang = rand() * RAND_TURN;
    dist = range * rand() * RAND_SCALE;
    if (at_top) {
        pos.x = dist * njSin(ang);
        pos.y = range - (rand() * (TOP_DEPTH * RAND_SCALE));
        pos.z = dist * njCos(ang);
    } else {
        pos.x = dist * njSin(ang);
        pos.y = range * ((rand() * RAND_SCALE) - 0.5F) * 2.0F;
        pos.z = dist * njCos(ang);
    }
    puff->pos = pos;
    puff->color =
        color_blend(puff_color_low, puff_color_high, rand() * RAND_SCALE);
    puff->half_width =
        puff_half_width_min + (puff_half_width_range * rand() * RAND_SCALE);
    puff->half_height =
        puff_half_height_min + (puff_half_height_range * rand() * RAND_SCALE);
    puff->alpha = 0.0F;
}

struct task *clouds_create(int player, NJS_POINT3 *pos, struct task **owner)
{
    struct task *task;
    struct task_work *work;
    struct cloud_puff *puff;
    int i;
    Float spacing;

    /* @hack spelling: the body nested in the tests of the slot and the task,
     * where a return of null after each would do */
    if (player_work[player] != NULL) {
        task = task_create(TASK_WORK, CLOUDS_LEVEL, clouds_step, "CloudExec");
        if (task != NULL) {
            work = task->work;
            clouds_camera_pos(&work->pos);
            work->pos.x = pos->x;
            work->pos.z = pos->z;
            work->tick = player;
            task->motion.p = syCalloc(1, sizeof(struct clouds));
            if (task->motion.p == NULL) {
                task_destroy(task);
                return NULL;
            }
            CLOUDS(task)->layers = syCalloc(LAYERS, sizeof(struct cloud_layer));
            if (CLOUDS(task)->layers == NULL) {
                syFree(task->motion.p);
                task->motion.p = NULL;
                task_destroy(task);
                return NULL;
            }
            CLOUDS(task)->puffs = syCalloc(PUFFS, sizeof(struct cloud_puff));
            if (CLOUDS(task)->puffs == NULL) {
                syFree(CLOUDS(task)->layers);
                syFree(task->motion.p);
                task->motion.p = NULL;
                task_destroy(task);
                return NULL;
            }
            /* @hack spelling: the draw set before the step, where struct task's
             * order would set the step first */
            task->draw = clouds_draw;
            task->step = clouds_step;
            task->destructor = clouds_destroy;
            CLOUDS(task)->puff_count = PUFFS;
            puff = CLOUDS(task)->puffs;
            /* @hack spelling: puff = puff + 1, where puff++ would do */
            for (i = 0; i < PUFFS; i++, puff = puff + 1) {
                cloud_puff_place(puff, band_half_height, FALSE);
            }
            CLOUDS(task)->layer_count = LAYERS;
            spacing = band_half_height * LAYER_SPACING;
            for (i = 0; i < LAYERS; i++) {
                CLOUDS(task)->layers[i].pos.x = 0.0F;
                CLOUDS(task)->layers[i].pos.y =
                    ((i + 1) * spacing) - band_half_height;
                CLOUDS(task)->layers[i].pos.z = 0.0F;
                CLOUDS(task)->layers[i].ang =
                    ((rand() * RAND_SCALE) - 0.5F) * LAYER_TURN_SPAN;
                CLOUDS(task)->layers[i].spin =
                    ((rand() * RAND_SCALE) - 0.5F) * LAYER_SPIN_SPAN;
                CLOUDS(task)->layers[i].color = 0;
                CLOUDS(task)->layers[i].scroll_u =
                    ((rand() * RAND_SCALE) - 0.5F) * LAYER_SCROLL_SPAN;
                CLOUDS(task)->layers[i].scroll_v =
                    ((rand() * RAND_SCALE) - 0.5F) * LAYER_SCROLL_SPAN;
            }
            CLOUDS(task)->owner = owner;
            if (owner != NULL) {
                *owner = task;
            }
            return task;
        }
    }
    return NULL;
}

static void clouds_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 camera;
    Float alpha;
    Float scale;
    int i;
    int color;
    /* @hack redundant: puff register, where no storage class would do */
    register struct cloud_puff *puff;
    /* @hack redundant: count register, where no storage class would do */
    register int count;
    /* @hack redundant: rise, x, y, z, dist2 and reach register, where no
     * storage class would do */
    register Float rise;
    register Float x;
    register Float y;
    register Float z;
    register Float dist2;
    register Float reach;

    if (player_work[work->tick] == NULL) {
        task_destroy(task);
        return;
    }
    clouds_camera_pos(&camera);
    work->pos.y = camera.y;
    /* @hack spelling: task->work read again, where work would do */
    if (task->work->mode != MODE_RUN) {
        return;
    }
    work->timer += FADE_IN_STEP;
    if (work->timer > FADE_IN_END) {
        work->timer = FADE_IN_END;
    }
    rise = -camera.y + work->pos.y + layer_drift;
    /* @hack spelling: i = i + 1, where i++ would do */
    for (i = 0; i < CLOUDS(task)->layer_count; i = i + 1) {
        Float height;

        height = CLOUDS(task)->layers[i].pos.y + rise;
        alpha = 1.0F;
        if (height > band_half_height - FADE_SPAN) {
            if (height > band_half_height) {
                height = -band_half_height;
                alpha = 0.0F;
            } else {
                alpha = 1.0F - ((height - band_half_height + FADE_SPAN) *
                                (1.0F / FADE_SPAN));
            }
        }
        if (-height > band_half_height - FADE_SPAN) {
            if (-height > band_half_height) {
                height = band_half_height;
                alpha = 0.0F;
            } else {
                alpha = 1.0F - ((FADE_SPAN - band_half_height - height) *
                                (1.0F / FADE_SPAN));
            }
        }
        if (alpha < 0.0F) {
            alpha = 0.0F;
        }
        CLOUDS(task)->layers[i].pos.y = height;
        if (alpha == 0.0F) {
            CLOUDS(task)->layers[i].color = WHITE;
        } else {
            color = alpha * 255.0F;
            if (color > 255) {
                color = 255;
            }
            if (color < 0) {
                color = 0;
            }
            /* @hack spelling: the shift in a statement of its own, where one
             * expression would do */
            color <<= ALPHA_SHIFT;
            CLOUDS(task)->layers[i].color = color | WHITE;
        }
        CLOUDS(task)->layers[i].ang += CLOUDS(task)->layers[i].spin;
    }
    count = CLOUDS(task)->puff_count;
    puff = CLOUDS(task)->puffs;
    rise = -camera.y + work->pos.y + puff_drift;
    for (i = 0; i < count; i++, puff++) {
        x = puff->pos.x;
        y = puff->pos.y + rise;
        z = puff->pos.z;
        dist2 = (x * x) + (y * y) + (z * z);
        reach =
            ((puff->half_width + puff->half_height) * 0.5F) + puff_clearance;
        if (reach * reach > dist2) {
            if (dist2 < CENTER_DIST2_MIN) {
                cloud_puff_place(puff,
                                 band_half_height -
                                     (rand() * (RESPAWN_DEPTH * RAND_SCALE)),
                                 TRUE);
            } else {
                scale = reach / njSqrt(dist2);
                y *= scale;
                x *= scale;
                z *= scale;
                puff->pos.x = x;
                puff->pos.y = y;
                puff->pos.z = z;
            }
        } else {
            puff->pos.y = y;
            if (y > band_half_height) {
                cloud_puff_place(puff,
                                 band_half_height -
                                     (rand() * (RESPAWN_DEPTH * RAND_SCALE)),
                                 TRUE);
                puff->pos.y = WRAP_INSET - band_half_height;
            }
            if (y < -band_half_height) {
                cloud_puff_place(puff,
                                 band_half_height -
                                     (rand() * (RESPAWN_DEPTH * RAND_SCALE)),
                                 TRUE);
                puff->pos.y = band_half_height - WRAP_INSET;
            }
            alpha = 1.0F;
            if (y > band_half_height - FADE_SPAN) {
                alpha = 1.0F - ((y - band_half_height + FADE_SPAN) *
                                (1.0F / FADE_SPAN));
            }
            if (y < FADE_SPAN - band_half_height) {
                alpha = 1.0F - ((FADE_SPAN - band_half_height - y) *
                                (1.0F / FADE_SPAN));
            }
            if (alpha > 1.0F) {
                alpha = 1.0F;
            }
            if (alpha < 0.0F) {
                alpha = 0.0F;
            }
            puff->alpha = alpha;
        }
    }
}

static void clouds_draw(struct task *task)
{
    struct task_work *work = task->work;
    Angle ang;
    int share;
    int i;
    int texid;
    int alpha;
    Uint32 color;
    Float fade;

    /* @hack spelling: the mode tested as a truth value, where a test against
     * MODE_RUN would do */
    if (work->mode) {
        return;
    }
    njFogDisable();
    njPushMatrixEx();
    /* @hack spelling: task->work read again, where work would do */
    njTranslateV(NULL, &task->work->pos);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&clouds_puff_texlist);
    /* with PUFFS puffs, 4 / puff_count and puff_count / 0x3000 both come to
     * 0. every puff's quad then turns by PUFF_TURN. the matrix turns puff i
     * about y by -2 * i * game_state.steps */
    ang = PUFF_TURN;
    share = 4 / CLOUDS(task)->puff_count;
    fade = work->timer * (1.0F / FADE_IN_END);
    for (i = 0; i < CLOUDS(task)->puff_count; i++) {
        texid = i % PUFF_TEXTURES;
        ang += CLOUDS(task)->puff_count / 0x3000;
        alpha = CLOUDS(task)->puffs[i].alpha * fade * 255.0F;
        if (alpha > 255) {
            alpha = 255;
        }
        if (alpha > PUFF_ALPHA_MIN) {
            color = (alpha << ALPHA_SHIFT) + WHITE;
            njPushMatrixEx();
            njRotateY(NULL, (share - 2) * i * game_state.steps);
            particle_draw_quad(
                &CLOUDS(task)->puffs[i].pos, texid, PUFF_EDGE_ANGLE, ang,
                CLOUDS(task)->puffs[i].half_width,
                CLOUDS(task)->puffs[i].half_height,
                color_multiply(color, CLOUDS(task)->puffs[i].color), 0, 0.0F,
                0.0F);
            njPopMatrix(1);
        }
    }
    njSetTexture(&clouds_layer_texlist);
    for (i = 0; i < CLOUDS(task)->layer_count; i++) {
        njPushMatrixEx();
        njTranslateV(NULL, &CLOUDS(task)->layers[i].pos);
        njRotateY(NULL, CLOUDS(task)->layers[i].ang);
        texture_anim_scroll(clouds_layer_model, &clouds_layer_anim,
                            CLOUDS(task)->layers[i].scroll_u,
                            CLOUDS(task)->layers[i].scroll_v);
        model_draw_tinted(clouds_layer_model, CLOUDS(task)->layers[i].color);
        njPopMatrix(1);
    }
    njPopMatrix(1);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void clouds_destroy(struct task *task)
{
    if (CLOUDS(task)->owner != NULL && *CLOUDS(task)->owner == task) {
        *CLOUDS(task)->owner = NULL;
    }
    syFree(CLOUDS(task)->layers);
    syFree(CLOUDS(task)->puffs);
    syFree(task->motion.p);
    task->motion.p = NULL;
}
