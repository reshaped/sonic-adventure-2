/* the glow lights: a glow that streaks as the camera moves. Mission Street's,
 * two-player Mission Street's and the Big Foot fight's object lists call them
 * GLIGHT and GLIGHT2 */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "draw/color.h"
#include "draw/trail.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/glow_light.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#include "shared/glow_light/text.inc"

/* a streak's points: each is where the glow lay in the view at one step */
#define STREAK_POINTS 9
/* the glow and its streak draw on each of two players' screens, which
 * camera_current_index numbers */
#define SCREENS 2
#define GLOW_TEXTURE 0
#define WHITE_EDGE_ANGLE 0x5000
#define COLORED_EDGE_ANGLE 0x4000
/* the depth particle_draw_quad takes: the glow draws GLOW_DEPTH nearer the
 * camera than it lies */
#define GLOW_DEPTH 10.0F
/* nothing draws where the glow lies more than BEHIND behind the camera. the
 * streak draws only while the glow lies less than STREAK_DEPTH ahead */
#define BEHIND 100.0F
#define STREAK_DEPTH 350.0F
/* the glow's quad turns as the glow lies off the line of sight. its first
 * edge lies glow_turn * r / (DEPTH_PART * |z| + NEAR_DEPTH) degrees from the
 * screen's x axis. r is the glow's distance from the line of sight, and z its
 * depth */
#define DEPTH_PART 0.1F
#define NEAR_DEPTH 5.0F

/* GLIGHT2's step packs its glow's color into the work's ang.x, and its
 * streak's into ang.y */
#define GLOW_COLOR(work) ((work)->ang.x)
#define STREAK_COLOR(work) ((work)->ang.y)
#define CHANNEL_MAX 255
#define RGB_BITS 0xffffff
#define GLOW_ALPHA 0xff000000
#define STREAK_ALPHA 0x50000000

/* the record task->motion holds: a streak for each screen */
struct glow_light_streaks {
    struct trail *trails[SCREENS];
};

#define STREAKS(task) ((struct glow_light_streaks *)(task)->motion.p)

static NJS_TEXNAME glow_texnames[2] = {
#include "shared/glow_light/glow_texnames.inc"
};
NJS_TEXLIST glow_texlist = {
#include "shared/glow_light/glow_texlist.inc"
};
NJS_TEXLIST *glow_texlists[2] = {
#include "shared/glow_light/glow_texlists.inc"
};

/* the white glow's streak. GLIGHT2 draws a copy in its work's colors */
static struct trail_style glow_trail_style = {
#include "shared/glow_light/glow_trail_style.inc"
};
static Float glow_size = 15.0F; /* half the glow's width and height */
static Uint32 glow_color = 0xffffffff;
static Float glow_turn = 35.0F; /* the quad's turn, as DEPTH_PART says */

static void glow_light_destroy(struct task *task);
static void glow_light_draw(struct task *task);
static void glow_light_colored_draw(struct task *task);
static void glow_light_step(struct task *task);
static void glow_light_colored_step(struct task *task);

void glow_light_init(struct task *task)
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

void glow_light_colored_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct glow_light_streaks));
    if (task->motion.p == NULL) {
        return;
    }
    task->step = glow_light_colored_step;
    task->draw = glow_light_colored_draw;
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
    ang = njSqrt((view.x * view.x) + (view.y * view.y)) * glow_turn *
          ANGLE_PER_DEGREE;
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&glow_texlist);
    particle_draw_quad(&work->pos, GLOW_TEXTURE, WHITE_EDGE_ANGLE, ang,
                       glow_size, glow_size, glow_color, PARTICLE_BLEND_ADD,
                       GLOW_DEPTH, 0.0F);
    if (!game_state.mini_event_running) {
        trail_draw(depth < STREAK_DEPTH ? &glow_trail_style : NULL,
                   STREAKS(task)->trails[camera_current_index], &work->pos);
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void glow_light_colored_draw(struct task *task)
{
    struct task_work *work = task->work;
    Angle ang;
    NJS_POINT3 view;
    struct trail_style style;
    Float depth;

    njCalcPoint(NULL, &work->pos, &view);
    if (view.z < -BEHIND) {
        return;
    }
    depth = view.z;
    view.z = njAbs(view.z * DEPTH_PART);
    view.x = view.x / (view.z + NEAR_DEPTH);
    view.y = view.y / (view.z + NEAR_DEPTH);
    ang = njSqrt((view.x * view.x) + (view.y * view.y)) * glow_turn *
          ANGLE_PER_DEGREE;
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&glow_texlist);
    particle_draw_quad(&work->pos, GLOW_TEXTURE, COLORED_EDGE_ANGLE, ang,
                       glow_size, glow_size, GLOW_COLOR(work),
                       PARTICLE_BLEND_ADD, GLOW_DEPTH, 0.0F);
    style = glow_trail_style;
    style.new_color = STREAK_COLOR(work);
    style.old_color = GLOW_COLOR(work) & RGB_BITS;
    if (!game_state.mini_event_running) {
        trail_draw(depth < STREAK_DEPTH ? &style : NULL,
                   STREAKS(task)->trails[camera_current_index], &work->pos);
    }
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

static void glow_light_colored_step(struct task *task)
{
    struct task_work *work = task->work;
    int channel;
    /* @hack redundant: unused, 4 bytes that nothing reads, which size the
     * frame */
    int unused[1]; /* NOLINT(clang-diagnostic-unused-variable) */

    /* @hack redundant: a block no path reaches. the image holds its three
     * strings, but none of its code */
    if (0) {
        (void)"<- COL 0-1 (RED)";
        (void)"<- COL 0-1 (GREEN)";
        (void)"<- COL 0-1 (BLUE)";
    }
    if (!placement_try_leave(task)) {
        GLOW_COLOR(work) = 0;
        channel = work->scl.x * CHANNEL_MAX;
        if (channel > CHANNEL_MAX) {
            channel = CHANNEL_MAX;
        }
        if (channel < 0) {
            channel = 0;
        }
        GLOW_COLOR(work) |= channel << RED_SHIFT;
        channel = work->scl.y * CHANNEL_MAX;
        if (channel > CHANNEL_MAX) {
            channel = CHANNEL_MAX;
        }
        if (channel < 0) {
            channel = 0;
        }
        GLOW_COLOR(work) |= channel << GREEN_SHIFT;
        channel = work->scl.z * CHANNEL_MAX;
        if (channel > CHANNEL_MAX) {
            channel = CHANNEL_MAX;
        }
        if (channel < 0) {
            channel = 0;
        }
        GLOW_COLOR(work) |= channel;
        STREAK_COLOR(work) = GLOW_COLOR(work) | STREAK_ALPHA;
        GLOW_COLOR(work) |= GLOW_ALPHA;
    }
}
