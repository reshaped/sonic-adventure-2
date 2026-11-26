/* the electric spark, BIRIBIRI in Death Chamber's object list: a sprite
 * of crackling electricity that faces the camera. player 0 hears it buzz
 * near it. BIRIBIRI is Japanese for the tingle of a shock */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "draw/chunk.h"
#include "draw/chunk_polygon.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg25/biribiri.h"
#include "stg25/chamber_util.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

/* a spark shows its BIRIBIRI_FRAMES frames in turn, a frame a step.
 * PHASE(task) is the frame it shows where game_state.steps is 0 */
#define BIRIBIRI_FRAMES 64
#define FRAME_MASK (BIRIBIRI_FRAMES - 1)
#define PHASE(task) ((task)->unk_3c.l)

#define BUZZ_SOUND 7
#define BUZZ_SOUND_PRIORITY 0
#define BUZZ_SOUND_STEPS 15

#include "stg25/biribiri/text.inc"

struct chamber_biribiri_frame {
    NJS_TEXNAME texture_name;
    NJS_TEXLIST texlist;
    struct {
        Uint16 material, material_words;
        struct part_color diffuse, ambient;
        struct part_texture texture;
        struct part_strip_header strips;
        Sint16 count;
        struct part_uv strip[4];
        Uint16 end;
    } polygons;
    struct {
        Sint32 head, count;
        struct chunk_vertex_d8 corners[4];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL model;
    NJS_CNK_OBJECT object;
};

static struct chamber_biribiri_frame chamber_biribiri_frames[BIRIBIRI_FRAMES] =
    {
#include "stg25/biribiri/chamber_biribiri_frames.inc"
};

static void chamber_biribiri_destroy(struct task *task);
static void chamber_biribiri_step(struct task *task);
static void chamber_biribiri_draw(struct task *task);

KEPT static NJS_CNK_MODEL *chamber_biribiri_models[BIRIBIRI_FRAMES] = {
#include "stg25/biribiri/chamber_biribiri_models.inc"
};
NJS_TEXLIST *chamber_biribiri_texlists[BIRIBIRI_FRAMES + 1] = {
#include "stg25/biribiri/chamber_biribiri_texlists.inc"
};
static Sint32 chamber_biribiri_buzz_volume = 15;
static Float chamber_biribiri_buzz_range = 120.0F;
static Uint32 chamber_biribiri_tint = 0xa0c0a0a0;

void chamber_biribiri_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = chamber_biribiri_draw;
        task->step = chamber_biribiri_step;
        task->destructor = chamber_biribiri_destroy;
        PHASE(task) = (Float)rand() * (BIRIBIRI_FRAMES * RAND_SCALE);
    }
}

static void chamber_biribiri_destroy(struct task *task)
{
}

static void chamber_biribiri_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        if (njDistanceP2P(&player_work[0]->pos, &work->pos) <
            chamber_biribiri_buzz_range) {
            sound_play_task_at_for(BUZZ_SOUND, task, BUZZ_SOUND_PRIORITY,
                                   chamber_biribiri_buzz_volume,
                                   BUZZ_SOUND_STEPS);
        }
    }
}

static void chamber_biribiri_draw(struct task *task)
{
    /* @hack redundant: pad, a local that only sizes the frame */
    Uint8 pad[12];
    struct task_work *work = task->work;
    int frame = (PHASE(task) + game_state.steps) & FRAME_MASK;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, camera_views[camera_current_index]->ang.y);
    njSetTexture(chamber_biribiri_texlists[frame]);
    chamber_model_draw_tinted_additive(chamber_biribiri_models[frame],
                                       chamber_biribiri_tint);
    njPopMatrixEx();
}

void chamber_biribiri_draw_at(NJS_POINT3 *pos)
{
    int frame = game_state.steps & FRAME_MASK;

    njPushMatrixEx();
    njTranslateEx(pos);
    njRotateY(NULL, camera_views[camera_current_index]->ang.y);
    njSetTexture(chamber_biribiri_texlists[frame]);
    chamber_model_draw_tinted_additive(chamber_biribiri_models[frame],
                                       chamber_biribiri_tint);
    njPopMatrixEx();
}
