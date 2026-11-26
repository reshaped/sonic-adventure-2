/* the glow light, GLIGHT in Radical Highway's object list: a glow that
 * streaks as the camera moves. it is shared/glow_light.c's white glow with a
 * streak style of its own. it draws its streak during a mini event too */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "draw/trail.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg14/glow_light.h"
#include "system/angle.h"
#include "system/task.h"

#include "stg14/glow_light/text.inc"

/* a streak's points: each is where the glow lay in the view at one step */
#define STREAK_POINTS 9
/* the glow and its streak draw on each of two players' screens, which
 * camera_current_index numbers */
#define SCREENS 2
#define GLOW_TEXTURE 0
#define STREAK_TEXTURE 1
#define EDGE_ANGLE 0x5000
/* the depth particle_draw_quad takes: the glow draws GLOW_DEPTH nearer the
 * camera than it lies */
#define GLOW_DEPTH 10.0F
/* nothing draws where the glow lies more than BEHIND behind the camera. the
 * streak draws only while the glow lies less than STREAK_DEPTH ahead */
#define BEHIND 100.0F
#define STREAK_DEPTH 350.0F
/* the glow turns GLOW_TURN * r / (DEPTH_PART * |z| + NEAR_DEPTH) degrees. r
 * is its distance from the line of sight, and z its depth */
#define GLOW_TURN 35.0F
#define DEPTH_PART 0.1F
#define NEAR_DEPTH 5.0F

/* the record task->motion holds: a streak for each screen */
struct glow_light_streaks {
    struct trail *trails[SCREENS];
};

#define STREAKS(task) ((struct glow_light_streaks *)(task)->motion.p)

/* The glare and streak texture slots. */

static NJS_TEXNAME highway_glow_light_textures[2] = {
#include "stg14/glow_light/highway_glow_light_textures.inc"
};

static NJS_TEXLIST highway_glow_light_texlist = {
#include "stg14/glow_light/highway_glow_light_texlist.inc"
};

NJS_TEXLIST *highway_glow_light_texlists[2] = {
#include "stg14/glow_light/highway_glow_light_texlists.inc"
};
static struct trail_style glow_trail_style = {
#include "stg14/glow_light/glow_trail_style.inc"
};
static Float glow_size = 15.0F; /* half the glow's width and height */
static Uint32 glow_color = 0xffffffff;

static void glow_light_destroy(struct task *task);
static void glow_light_draw(struct task *task);
static void glow_light_step(struct task *task);

void highway_glow_light_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct glow_light_streaks));
    if (task->motion.p == NULL) {
        return;
    }
    task->step = glow_light_step;
    task->draw = glow_light_draw;
    task->destructor = glow_light_destroy;
    STREAKS(task)->trails[0] = trail_alloc(&glow_trail_style, STREAK_POINTS);
    STREAKS(task)->trails[1] = trail_alloc(&glow_trail_style, STREAK_POINTS);
}

static void glow_light_destroy(struct task *task)
{
    trail_free(&glow_trail_style, STREAKS(task)->trails[0]);
    trail_free(&glow_trail_style, STREAKS(task)->trails[1]);
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void glow_light_draw(struct task *task)
{
    struct task_work *work = task->work;
    Angle ang;
    NJS_POINT3 view;
    Float depth;

    njCalcPoint(NULL, &work->pos, &view);
    if (view.z < -BEHIND) {
        return;
    }
    depth = view.z;
    view.z = njAbs(view.z * DEPTH_PART);
    view.x = view.x / (view.z + NEAR_DEPTH);
    view.y = view.y / (view.z + NEAR_DEPTH);
    ang = njSqrt((view.x * view.x) + (view.y * view.y)) *
          (GLOW_TURN * ANGLE_PER_DEGREE);
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&highway_glow_light_texlist);
    particle_draw_quad(&work->pos, GLOW_TEXTURE, EDGE_ANGLE, ang, glow_size,
                       glow_size, glow_color, PARTICLE_BLEND_ADD, GLOW_DEPTH,
                       0.0F);
    trail_draw(depth < STREAK_DEPTH ? &glow_trail_style : NULL,
               STREAKS(task)->trails[camera_current_index], &work->pos);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void glow_light_step(struct task *task)
{
    /* @hack redundant: the result tested before an empty return, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}
