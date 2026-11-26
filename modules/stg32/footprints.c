/* the footprints, ObjectAsiatoExec: the last MAX_PRINTS prints that one
 * player's feet left on GROUND_FOOTSTEPS ground. only Knuckles, Tical and
 * Rouge leave prints */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "effect/particle.h"
#include "player/player.h"
#include "stg32/meteors.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the task level of a footprints task made without a parent */
#define FOOTPRINT_LEVEL 3
#define MAX_PRINTS 32
#define QUAD_VERTICES 4
/* on level ground a print lies SURFACE_OFFSET above the player's pos. the
 * offset keeps it off the ground beneath */
#define SURFACE_OFFSET 0.01F
/* a print's pulse runs from pulse_blue to pulse_yellow and back with the sine
 * of game_state.steps * PULSE_RATE. each print's sine runs PULSE_SPACING ahead
 * of the next older print's */
#define PULSE_RATE 0x300
#define PULSE_SPACING 0x80
/* the player's slot, and where each foot last left a print. the first foot is
 * at unk_20c of the player's record, and the second at unk_200 */
#define PLAYER(work) ((work)->submode)
#define FIRST_FOOT_PRINTED(work) ((work)->pos)
#define SECOND_FOOT_PRINTED(work) ((work)->scl)

/* a print: the foot that left it, and its quad in the world. side is 0 for
 * the first foot, and 1 for the second */
struct footprint {
    Sint32 side;
    NJS_TEXTURE_VTX vertices[QUAD_VERTICES];
};
/* the prints at task->motion: count of them, the oldest at head */
struct footprint_ring {
    Sint32 count;
    Sint32 head;
    struct footprint prints[MAX_PRINTS];
};
#define RING(task) ((struct footprint_ring *)(task)->motion.p)

#include "stg32/footprints/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg32/footprints/textures.inc"
};
static NJS_TEXLIST footprint_texlist = {textures, 1};
NJS_TEXLIST *meteor_footprint_textures[] = {&footprint_texlist, NULL};

/* each foot's quad in the player's frame. Knuckles' treads take the texture's
 * right half, and Rouge's the left. the second foot's tread is the first's,
 * mirrored in u */
static NJS_TEXTURE_VTX knuckles_first[QUAD_VERTICES] = {
#include "stg32/footprints/knuckles_first.inc"
};
static NJS_TEXTURE_VTX knuckles_second[QUAD_VERTICES] = {
#include "stg32/footprints/knuckles_second.inc"
};
static NJS_TEXTURE_VTX rouge_first[QUAD_VERTICES] = {
#include "stg32/footprints/rouge_first.inc"
};
static NJS_TEXTURE_VTX rouge_second[QUAD_VERTICES] = {
#include "stg32/footprints/rouge_second.inc"
};
/* a foot is down where its -y axis and unk_2cc have an inner product above
 * foot_down_level. the first foot's -y axis is unk_2a8, and the second's
 * unk_29c. unk_2cc is the x axis of a node of the player's model */
static Float foot_down_level = 0.65F;
/* a print fades from newest_color to oldest_color with its age, and its pulse
 * multiplies that color */
static Uint32 pulse_blue = 0xa080e0ff;
static Uint32 pulse_yellow = 0xffffff80;
static Uint32 oldest_color = 0x05ffffff;
static Uint32 newest_color = 0xffffe0ff;

static void footprints_step(struct task *task);
static void footprints_destroy(struct task *task);
static void footprints_draw(struct task *task);

struct task *meteor_footprints_create(int player, struct task *parent)
{
    struct task *task;
    struct task_work *work;

    if (player >= GAME_PLAYERS || player_work[player] == NULL) {
        return NULL;
    }
    if (parent) {
        task = task_create_child(TASK_WORK, footprints_step, parent);
    } else {
        task = task_create(TASK_WORK, FOOTPRINT_LEVEL, footprints_step,
                           "ObjectAsiatoExec");
    }
    if (task) {
        work = task->work;
        task->motion.p = syCalloc(1, sizeof(struct footprint_ring));
        if (task->motion.p) {
            task->draw = footprints_draw;
            task->step = footprints_step;
            task->destructor = footprints_destroy;
            SECOND_FOOT_PRINTED(work) = player_records[player]->unk_200;
            FIRST_FOOT_PRINTED(work) = player_records[player]->unk_20c;
            PLAYER(work) = player;
        } else {
            task_destroy(task);
            task = NULL;
        }
    }
    return task;
}

#define PLACE_TREAD(print, tread, offset, i)                                   \
    for ((i) = 0; (i) < QUAD_VERTICES; (i)++) {                                \
        (print)->vertices[i].x = (tread)[i].x + (offset).x;                    \
        (print)->vertices[i].y = (tread)[i].y + (offset).y;                    \
        (print)->vertices[i].z = (tread)[i].z + (offset).z;                    \
        (print)->vertices[i].u = (tread)[i].u;                                 \
        (print)->vertices[i].v = (tread)[i].v;                                 \
        (print)->vertices[i].col = (tread)[i].col;                             \
    }

static void footprint_place(int player, struct footprint *print, int side)
{
    NJS_TEXTURE_VTX *first;
    NJS_TEXTURE_VTX *second;
    NJS_POINT3 offset;
    int i;

    switch (player_character(player)) {
    case PLAYER_ROUGE:
        second = rouge_second;
        first = rouge_first;
        break;
    case PLAYER_KNUCKLES:
    case PLAYER_KIND_TICAL:
        second = knuckles_second;
        first = knuckles_first;
        break;
    default:
        second = NULL;
        first = NULL;
        break;
    }
    if (!second || !first) {
        return;
    }
    if (side & 1) {
        offset = player_records[player]->unk_200;
    } else {
        offset = player_records[player]->unk_20c;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, player_work[player]->ang.z);
    njRotateX(NULL, player_work[player]->ang.x);
    njRotateY(NULL, HALF_TURN - player_work[player]->ang.y);
    njTranslateEx(&player_work[player]->pos);
    njPushMatrixEx();
    njInvertMatrix(NULL);
    njCalcPoint(NULL, &offset, &offset);
    offset.y = SURFACE_OFFSET;
    print->side = side;
    /* @hack spelling: a loop for each foot's tread, where one loop over the
     * tread that side chooses would do */
    if (side & 1) {
        PLACE_TREAD(print, second, offset, i);
    } else {
        PLACE_TREAD(print, first, offset, i);
    }
    njPopMatrixEx();
    for (i = 0; i < QUAD_VERTICES; i++) {
        /* @hack type: each vertex's x, y and z read in place as an
         * NJS_POINT3, where a copy to an NJS_POINT3 and back would do */
        njCalcPoint(NULL, (NJS_POINT3 *)&print->vertices[i].x,
                    (NJS_POINT3 *)&print->vertices[i].x);
    }
    njPopMatrixEx();
}

/* print is the ring's next slot. a full ring gives up its oldest print */
#define APPEND_PRINT(task, print)                                              \
    do {                                                                       \
        (print) = &RING(task)->prints[(RING(task)->head + RING(task)->count) % \
                                      MAX_PRINTS];                             \
        if (RING(task)->count < MAX_PRINTS) {                                  \
            RING(task)->count++;                                               \
        } else {                                                               \
            RING(task)->head = (RING(task)->head + 1) % MAX_PRINTS;            \
        }                                                                      \
    } while (0)

static void footprints_step(struct task *task)
{
    struct task_work *work = task->work;
    struct footprint *print;
    Float level;

    if (!player_work[PLAYER(work)]) {
        return;
    }
    if (!(player_work[PLAYER(work)]->flags &
          (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE))) {
        return;
    }
    if (!(player_records[PLAYER(work)]->ground_flags & GROUND_FOOTSTEPS)) {
        return;
    }
    level = njInnerProduct(&player_records[PLAYER(work)]->unk_2cc,
                           &player_records[PLAYER(work)]->unk_2a8);
    if (level > foot_down_level) {
        if (njDistanceP2P(&FIRST_FOOT_PRINTED(work),
                          &player_records[PLAYER(work)]->unk_20c) >
            player_records[PLAYER(work)]->consts.radius) {
            APPEND_PRINT(task, print);
            footprint_place(PLAYER(work), print, 0);
            FIRST_FOOT_PRINTED(work) = player_records[PLAYER(work)]->unk_20c;
        }
    }
    level = njInnerProduct(&player_records[PLAYER(work)]->unk_2cc,
                           &player_records[PLAYER(work)]->unk_29c);
    if (level > foot_down_level) {
        if (njDistanceP2P(&SECOND_FOOT_PRINTED(work),
                          &player_records[PLAYER(work)]->unk_200) >
            player_records[PLAYER(work)]->consts.radius) {
            APPEND_PRINT(task, print);
            footprint_place(PLAYER(work), print, 1);
            SECOND_FOOT_PRINTED(work) = player_records[PLAYER(work)]->unk_200;
        }
    }
}

static void footprints_destroy(struct task *task)
{
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void footprints_draw(struct task *task)
{
    /* @hack redundant: a read of task->work that the draw never uses */
    struct task_work *work = task->work;
    int count = RING(task)->count;
    int index = RING(task)->head;
    Angle angle;
    int i;
    Uint32 fade;
    Uint32 pulse;
    Uint32 color;
    NJS_TEXTURE_VTX *vertices;

    (void)work;
    njSetTexture(&footprint_texlist);
    njSetTextureNum(0);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    angle = game_state.steps * PULSE_RATE;
    njDrawTexture3DExStart(TRUE);
    for (i = 0; i < count; i++) {
        angle += PULSE_SPACING;
        fade = color_blend(newest_color, oldest_color,
                           (Float)(count - i) * (1.0F / MAX_PRINTS));
        pulse =
            color_blend(pulse_blue, pulse_yellow, (njSin(angle) + 1.0F) * 0.5F);
        color = color_multiply(fade, pulse);
        vertices = RING(task)->prints[index].vertices;
        vertices[0].col = color;
        vertices[1].col = color;
        vertices[2].col = color;
        vertices[3].col = color;
        njDrawTexture3DExSetData(vertices, QUAD_VERTICES);
        index = (index + 1) % MAX_PRINTS;
    }
    njDrawTexture3DExEnd();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}
