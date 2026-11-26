/* the hint box, HINTBOX in a stage's object list. a player near the box can
 * ask it for one of the stage's hints. the box shows the hint in a message
 * window */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/hint_box.h"
#include "shared/table_file.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "shared/hint_box/text.inc"

/* the box's modes, in its work's mode */
enum hint_box_mode {
    /* it takes MODE_WAITING at its next step */
    MODE_START,
    /* it waits for an ask */
    MODE_WAITING,
    /* it lights up until it is fully lit and no message window is open */
    MODE_LIGHTING,
    /* it shows its hint until no message window is open */
    MODE_SHOWING
};
/* the slot of the player who last asked. before any has, it is slot 0, as a
 * work starts zeroed */
#define ASKER(work) ((work)->tick)

#define BRIGHTNESS_STEP 0.05F
#define NEAR_RANGE 50.0F
/* the most the box turns in a step, in Angle units */
#define TURN_STEP 0x200
/* the box bobs up to BOB_HEIGHT above and below the work's pos. bob_ang
 * advances BOB_STEP a step while the box lights up or shows its hint */
#define BOB_STEP 0x300
#define BOB_HEIGHT 0.7F
/* the steps the box rests after a hint, taking no ask */
#define REST_STEPS 30
/* the steps each page of a hint shows, where the page has no W code */
#define HINT_PAGE_STEPS 180

/* a box that hasn't found its floor probes for it every FLOOR_STEPS steps, a
 * power of 2 */
#define FLOOR_STEPS 4
/* the steps a player's look at the box lasts. the fully lit box renews the
 * look each step */
#define LOOK_STEPS 1
/* a color channel's greatest value */
#define CHANNEL_MAX 255
/* a packed ARGB color: alpha, then gray in red, green and blue */
#define ARGB_GRAY(alpha, gray)                                                 \
    (((alpha) << 24) + ((gray) << 16) + ((gray) << 8) + (gray))

#define HINT_SOUND 0x8013
#define HINT_PRIORITY 0
#define HINT_VOLUME 0
#define SHADOW_LIFT 0.2F
#define SHADOW_SCALE 3.0F

/* the text the box shows for a hint it lacks: a giggle, then "(note: there
 * is no text)" */
#define NO_TEXT                                                                \
    "\x82\xde\x82\xd3\x82\xd3\x82\xa3\x81\x69\x89\xf0\x90\xe0\x81\x46"         \
    "\b80ffff\x83\x65\x83\x4c\x83\x58\x83\x67\br"                              \
    "\x82\xaa\x82\xc8\x82\xa2\x82\xb6\x82\xa5\x81\x6a"

/* a box's record, in its task's motion */
struct hint_box {
    /* the box's opacity, from 0 to 1. only hint_box_init writes it, with 1,
     * so no box draws faded */
    Float opacity;
    Float brightness; /* how bright the box's panel is, from 0 to 1 */
    /* where the box faces. the work's ang.y is where it faces at rest */
    Angle ang_y;
    /* the point on the floor below the work's pos, where the shadow falls.
     * it is the work's pos until the step finds a floor. no shadow draws
     * until then */
    NJS_POINT3 floor;
    Sint32 unk_18;
    Angle bob_ang;
    Float bob;
    struct task *message; /* the window of the box's hint, or null for none */
    Sint32 unk_28;
};

#define BOX(task) ((struct hint_box *)(task)->motion.p)

/* an entry of the hint file's table */
struct hint {
    char *text;
};

#define HINT_BOX_TEXTURES 5
static NJS_TEXNAME hint_box_texnames[HINT_BOX_TEXTURES] = {
#include "shared/hint_box/hint_box_texnames.inc"
};
static NJS_TEXLIST hint_box_texlist = {
#include "shared/hint_box/hint_box_texlist.inc"
};
/* the box's model: a chunk model of BOX_POINTS points, each with its
 * normal */
#define BOX_POINTS 77
static Sint16 hint_box_polygons[687] = {
#include "shared/hint_box/hint_box_polygons.inc"
};
static Sint32 hint_box_vertices[465] = {
#include "shared/hint_box/hint_box_vertices.inc"
};
static NJS_CNK_MODEL hint_box_model = {
#include "shared/hint_box/hint_box_model.inc"
};
static NJS_CNK_OBJECT hint_box_object = {
#include "shared/hint_box/hint_box_object.inc"
};
#define PANEL_TEXTURES 1
static NJS_TEXNAME hint_box_panel_texname[PANEL_TEXTURES] = {
#include "shared/hint_box/hint_box_panel_texname.inc"
};
static NJS_TEXLIST hint_box_panel_texlist = {
#include "shared/hint_box/hint_box_panel_texlist.inc"
};
/* the panel's model: a chunk model of PANEL_POINTS points, each with its
 * color. the draw sets every color from the box's brightness */
#define PANEL_POINTS 4
static Sint16 hint_box_panel_polygons[25] = {
#include "shared/hint_box/hint_box_panel_polygons.inc"
};
struct hint_box_panel_vertex_list {
    Sint32 head[2];
    struct chunk_vertex_d8 points[PANEL_POINTS];
    Sint32 end;
};
static struct hint_box_panel_vertex_list hint_box_panel_vertices = {
#include "shared/hint_box/hint_box_panel_vertices.inc"
};
static NJS_CNK_MODEL hint_box_panel_model = {
#include "shared/hint_box/hint_box_panel_model.inc"
};
static NJS_CNK_OBJECT hint_box_panel_object = {
#include "shared/hint_box/hint_box_panel_object.inc"
};

NJS_TEXLIST *hint_box_texlists[3] = {
#include "shared/hint_box/hint_box_texlists.inc"
};

static void hint_box_destroy(struct task *task);
static void hint_box_step(struct task *task);
static void hint_box_draw(struct task *task);

#define HINT_BOX_SHAPES 1
/* clang-format off */
static struct collision_shape hint_box_shapes[HINT_BOX_SHAPES] = {
#include "shared/hint_box/hint_box_shapes.inc"
};
/* clang-format on */
/* the stage's hints, or null while none are loaded */
static struct hint *hint_box_hints = NULL;
/* how many hints there are, or -1 while none are loaded */
static Sint32 hint_box_hint_count = -1;

void hint_box_hints_free(void)
{
    if (hint_box_hints != NULL) {
        syFree(hint_box_hints);
        hint_box_hints = NULL;
        hint_box_hint_count = -1;
    }
}

Sint32 hint_box_hints_load(const char *name)
{
    if (hint_box_hints == NULL) {
        hint_box_hints = table_file_read_language(name, &hint_box_hint_count);
        if (hint_box_hints != NULL) {
            return hint_box_hint_count;
        }
    }
    return -1;
}

static void hint_box_draw_shadow(struct task *task)
{
    njPushMatrixEx();
    njTranslate(NULL, BOX(task)->floor.x, BOX(task)->floor.y + SHADOW_LIFT,
                BOX(task)->floor.z);
    njScale(NULL, SHADOW_SCALE, 1.0F, SHADOW_SCALE);
    njCnkModDrawObject(&shadow_object);
    njPopMatrix(1);
}

void hint_box_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct hint_box));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = hint_box_draw;
    task->step = hint_box_step;
    task->destructor = hint_box_destroy;
    work->mode = MODE_START;
    work->submode = 0;
    work->timer = 0;
    collision_init(task, hint_box_shapes, HINT_BOX_SHAPES, COLLISION_OBJECT);
    BOX(task)->opacity = 1.0F;
    BOX(task)->brightness = 0.0F;
    BOX(task)->ang_y = work->ang.y;
    BOX(task)->floor = work->pos;
    BOX(task)->bob = 0.0F;
    BOX(task)->bob_ang = 0;
    BOX(task)->message = NULL;
    BOX(task)->unk_28 = -1;
}

static void hint_box_destroy(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its three strings,
     * which SHC still places in this source's constants. the middle string
     * is a stand-in text that nothing shows */
    if (0) {
        (void)"<- MSG NUM";
        (void)"\x82\xde\x82\xd3\x82\xd3\x82\xa3\x81\x69\x89\xf0\x90\xe0\x81\x46"
              "\bff8080\x83\x65\x83\x4c\x83\x58\x83\x67\br"
              "\x82\xaa\x82\xc8\x82\xa2\b80ff80\x83\x66\br"
              "\x83\x41\x83\x8b\x81\x6a";
        (void)"<- ANGLE Y";
    }
    if (BOX(task)->message != NULL) {
        task_destroy(BOX(task)->message);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* @hack redundant: task declared register, where a plain parameter would do */
static void hint_box_step(register struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: state, a local for &game_state, where game_state
     * itself would do */
    struct game_state *state;
    int player;
    int hint;
    /* @hack spelling: steps, a local for state->steps, where state->steps
     * read at its use would do */
    Uint32 steps;
    Float floor_y;

    if (placement_try_leave(task)) {
        return;
    }
    state = &game_state;
    steps = state->steps;
    if (BOX(task)->floor.y == work->pos.y && (steps & (FLOOR_STEPS - 1)) == 0) {
        struct angles tilt;

        floor_y =
            surface_probe_floor(work->pos.x, work->pos.y, work->pos.z, &tilt);
        if (floor_y != NO_FLOOR) {
            BOX(task)->floor.x = work->pos.x;
            BOX(task)->floor.y = floor_y;
            BOX(task)->floor.z = work->pos.z;
        }
    }
    BOX(task)->bob = njSin(BOX(task)->bob_ang) * BOB_HEIGHT;
    switch (work->mode) {
    case MODE_START:
        work->mode = MODE_WAITING;
        break;
    case MODE_WAITING:
        player = player_near(&work->pos, NEAR_RANGE);
        if (player != 0 &&
            player_records[player - 1]->action_taken != PLAYER_MODE_ASK_HINT) {
            player_records[player - 1]->action = PLAYER_MODE_ASK_HINT;
            player = 0;
        }
        if (work->timer != 0 || player <= 0) {
            break;
        }
        player--;
        /* @hack redundant: a test of player that always holds */
        if (player >= 0 && pad_player_is_enabled(player) != 0) {
            player_records[player]->action_taken = 0;
            ASKER(work) = player;
            work->mode = MODE_LIGHTING;
        }
        break;
    case MODE_LIGHTING:
        BOX(task)->brightness += BRIGHTNESS_STEP;
        if (message_window_current() == NULL && BOX(task)->brightness >= 1.0F) {
            BOX(task)->brightness = 1.0F;
            if (BOX(task)->message != NULL) {
                task_destroy(BOX(task)->message);
            }
            /* @hack spelling: the hint's index worked out again for the
             * result, where a local set once would do */
            hint = hint_box_hint_count >= 0 &&
                           (int)njAbs(work->scl.x) < hint_box_hint_count
                       ? (int)njAbs(work->scl.x)
                       : -1;
            work->mode = MODE_SHOWING;
            pad_disable_player(ASKER(work));
            message_window_open(MESSAGE_WINDOW_FADE,
                                hint >= 0 ? hint_box_hints[hint].text : NO_TEXT,
                                HINT_PAGE_STEPS, state->text_language,
                                &BOX(task)->message);
            sound_play(HINT_SOUND, NULL, HINT_PRIORITY, HINT_VOLUME);
        }
        BOX(task)->bob_ang += BOB_STEP;
        break;
    case MODE_SHOWING:
        if (message_window_current() == NULL) {
            pad_enable_player(ASKER(work));
            work->timer = REST_STEPS;
            work->mode = MODE_START;
        } else {
            BOX(task)->bob_ang += BOB_STEP;
        }
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((player = player_near(&work->pos, NEAR_RANGE)) > 0) {
        /* @hack spelling: slot, a local for player - 1, where player - 1 in
         * the call would do */
        int slot = player - 1;

        BOX(task)->brightness += BRIGHTNESS_STEP;
        if (BOX(task)->brightness >= 1.0F) {
            NJS_VECTOR to;
            NJS_VECTOR offset;
            /* @hack type: turn an int, read through (Sint16) at each use,
             * where a Sint16 would do */
            int turn;

            offset.x = 0.0F;
            offset.y = 0.0F;
            offset.z = 0.0F;
            BOX(task)->brightness = 1.0F;
            player_look_at_task(slot, task, &offset, LOOK_STEPS);
            to.x = player_work[ASKER(work)]->pos.x - work->pos.x;
            to.y = player_work[ASKER(work)]->pos.y - work->pos.y;
            to.z = player_work[ASKER(work)]->pos.z - work->pos.z;
            /* @hack spelling: the turn worked out in two statements, where
             * one expression would do */
            turn = (Angle)(atan2f(to.x, to.z) * ANGLE_PER_RADIAN);
            turn -= BOX(task)->ang_y;
            if ((Sint16)turn > 0) {
                if ((Sint16)turn > TURN_STEP) {
                    turn = TURN_STEP;
                }
            } else if ((Sint16)turn < -TURN_STEP) {
                turn = -TURN_STEP;
            }
            BOX(task)->ang_y += (Sint16)turn;
        }
    } else if (BOX(task)->brightness > 0.0F) {
        BOX(task)->brightness -= BRIGHTNESS_STEP;
        if (BOX(task)->brightness < 0.0F) {
            BOX(task)->brightness = 0.0F;
        }
    } else {
        /* @hack type: turn an int, read through (Sint16) at each use, where a
         * Sint16 would do */
        int turn;

        turn = work->ang.y - BOX(task)->ang_y;
        if ((Sint16)turn > 0) {
            if ((Sint16)turn > TURN_STEP) {
                turn = TURN_STEP;
            }
        } else if ((Sint16)turn < -TURN_STEP) {
            turn = -TURN_STEP;
        }
        BOX(task)->ang_y += (Sint16)turn;
    }
    if (work->timer > 0 && !(work->collision->flags & COLLISION_TOUCHED)) {
        work->timer--;
        /* @hack redundant: an empty else */
    } else {
    }
    collision_register(task);
}

/* vlist must be an NJD_CV_D8 chunk of count vertices or more */
static void hint_box_vlist_set_colors(Sint32 *vlist, int count, Uint32 color)
{
    /* @hack type: vlist stepped from color to color, where a
     * struct chunk_vertex_d8 pointer stepped from vertex to vertex would do */
    vlist += CHUNK_HEAD_WORDS + CHUNK_V_WORDS;
    while (count--) {
        *vlist = color;
        vlist += CHUNK_D8_WORDS;
    }
}

static void hint_box_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object, a local for &hint_box_object, where
     * hint_box_object.model at each use would do */
    NJS_CNK_OBJECT *object;
    Float opacity = BOX(task)->opacity;
    Uint32 gray;
    Uint32 alpha;

    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + BOX(task)->bob, work->pos.z);
    njRotateY(NULL, BOX(task)->ang_y);
    object = &hint_box_object;
    gray = (Uint32)(BOX(task)->brightness * CHANNEL_MAX);
    if (gray > CHANNEL_MAX) {
        gray = CHANNEL_MAX;
    }
    if (opacity == 1.0F) {
        njSetTexture(&hint_box_texlist);
        njCnkSimpleDrawModel(object->model);
        alpha = CHANNEL_MAX;
        njSetTexture(&hint_box_panel_texlist);
        hint_box_vlist_set_colors((Sint32 *)&hint_box_panel_vertices,
                                  PANEL_POINTS, ARGB_GRAY(alpha, gray));
        njCnkEasyDrawObject(&hint_box_panel_object);
    } else {
        alpha = (Uint32)(opacity * CHANNEL_MAX);
        if (alpha > CHANNEL_MAX) {
            alpha = CHANNEL_MAX;
        }
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        constant_material_set(opacity, 1.0F, 1.0F, 1.0F);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        njSetTexture(&hint_box_texlist);
        njCnkSimpleDrawModel(object->model);
        njSetTexture(&hint_box_panel_texlist);
        hint_box_vlist_set_colors((Sint32 *)&hint_box_panel_vertices,
                                  PANEL_POINTS, ARGB_GRAY(alpha, gray));
        njCnkEasyDrawObject(&hint_box_panel_object);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    njPopMatrix(1);
    if (BOX(task)->floor.y != work->pos.y) {
        hint_box_draw_shadow(task);
    }
}
