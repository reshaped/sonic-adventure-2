/* the cart course's background: the sea's grid below the course and the sky's
 * dome above it. each is drawn at the camera's x and z. the grid's colors
 * scroll a row at a time, blending from each row into the next. no code calls
 * cart_background_stop */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "cart/background_art.h"
#include "cart/course.h"
#include "draw/chunk.h"
#include "effect/particle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the textures the grid steps through, and the steps each one shows */
#define FLOOR_FRAMES 20
#define FLOOR_FRAME_STEPS 2
#define FLOOR_SCALE 4

Float cart_course_lowest;
/* the steps each row of the grid's colors takes to scroll past a vertex */
static int color_row_steps = 50;
/* how far above cart_course_lowest the sky's dome stands */
static Float sky_height = 350;
static Bool stopping = 0;

static void draw_floor(void)
{
    Sint32 *base;
    Sint32 *src;
    Sint32 *dst;
    Sint32 *buffer;
    Sint32 *saved;
    Sint32 *first_color;
    Sint32 *next_color;
    Sint32 word;
    int count;
    int side;
    int frame;
    int row;
    int next_row;
    int first;
    int next;
    int i;
    int j;
    int flags;
    int texture;
    Float fraction;
    Uint32 color;
    NJS_CNK_MODEL *model;
    Sint16 *texture_word;

    buffer = (Sint32 *)WORK_AREA;
    base = cart_floor->model->vlist;
    src = base;
    dst = buffer;
    /* @hack redundant: the chunk's first word kept in word, which nothing
     * reads before the next store */
    word = *dst++ = *src++;
    word = *dst++ = *src++;
    /* @hack spelling: word read through its address, and cast to Sint16
     * first, where (Uint16)(word >> CHUNK_HIGH_SHIFT) would do */
    count = (Uint16)(Sint16)(*(&word) >> CHUNK_HIGH_SHIFT);
    side = njSqrt(count);
    frame = game_state.steps % (side * color_row_steps);
    fraction = (Float)(frame % color_row_steps) / color_row_steps;
    frame /= color_row_steps;
    next_row = frame;

    /* base moves to the first vertex's color */
    base += CHUNK_HEAD_WORDS + CHUNK_V_WORDS;
    /* @hack spelling: i set ahead of the rows, where the for's own start
     * would do */
    i = 0;
    /* @hack spelling: row taken from next_row as next_row steps, where row =
     * frame and next_row = frame + 1 would do */
    row = next_row++;
    for (; i < side; i++) {
        first = row + i;
        if (first >= side) {
            first -= side;
        }
        first *= side;
        next = next_row + i;
        if (next >= side) {
            next -= side;
        }
        next *= side;
        j = 0;
        first_color = base + (first * CHUNK_D8_WORDS);
        next_color = base + (next * CHUNK_D8_WORDS);
        /* @hack redundant: each coordinate's copy also stored in word, which
         * nothing reads */
        while (j < side) {
            word = *dst++ = *src++;
            word = *dst++ = *src++;
            word = *dst++ = *src++;
            src++;
            color = color_blend(*first_color, *next_color, fraction);
            *dst++ = color;
            j++;
            first_color += CHUNK_D8_WORDS;
            /* @hack spelling: next_color = next_color + CHUNK_D8_WORDS, where
             * += would do */
            next_color = next_color + CHUNK_D8_WORDS;
        }
    }
    *dst = *src;
    model = cart_floor->model;
    saved = model->vlist;
    model->vlist = buffer;
    njSetTexture(&cart_floor_textures);
    texture_word = &model->plist[1];
    /* @hack spelling: flags loaded in a statement of its own, where one
     * expression would do */
    flags = *texture_word;
    flags &= NJD_FFM_MASK;
    texture = flags | ((game_state.steps / FLOOR_FRAME_STEPS) % FLOOR_FRAMES);
    *texture_word = texture;
    njPushMatrixEx();
    njTranslate(NULL, camera_views[camera_current_index]->pos.x,
                cart_course_lowest, camera_views[camera_current_index]->pos.z);
    njScale(NULL, FLOOR_SCALE, FLOOR_SCALE, FLOOR_SCALE);
    njCnkSimpleDrawModel(model);
    njPopMatrix(1);
    model->vlist = saved;
}
static void draw_sky(void)
{
    njFogDisable();
    njSetTexture(&cart_sky_textures);
    njPushMatrixEx();
    njTranslate(NULL, camera_views[camera_current_index]->pos.x,
                sky_height + cart_course_lowest,
                camera_views[camera_current_index]->pos.z);
    draw_control_save();
    draw_control_clear(NJD_CONTROL_3D_MODEL_CLIP);
    njCnkEasyDrawObject(&cart_sky);
    draw_control_restore();
    njPopMatrix(1);
    njFogEnable();
}
static void background_step(struct task *task)
{
    if (stopping) {
        task_destroy(task);
    }
}
static void background_draw(struct task *task)
{
    draw_floor();
    draw_sky();
}
static void background_destroy(struct task *task)
{
    stopping = 0;
}
void cart_background_start(void)
{
    struct task *task;
    task = task_create(0, 2, background_step, "bgExec");
    stopping = 0;
    task->destructor = background_destroy;
    task->draw = background_draw;
}
void cart_background_stop(void)
{
    stopping = 1;
}
