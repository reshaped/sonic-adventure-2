/* the search boxes, SCHBOX and SGSCHBOX in a stage's object list. a player
 * near a box can ask it for a hint to the hunt's current piece. the box
 * opens, shows the hint, then rises and goes. after each hint the boxes rest
 * a while: none opens, and each fades. the pause screen shows the current
 * piece's hints given so far */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/search_box.h"
#include "shared/vertex_colors.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "shared/search_box/text.inc"

/* the task level of the rest's timer and of the pause hints */
#define HINT_LEVEL 3

#define HINT_FREEZE_BUTTONS (PDD_DGT_TX | PDD_DGT_TY)
#define HINT_STEP_BUTTONS (PDD_DGT_TL | PDD_DGT_TR)

/* the draws for camera 0 that the pause screen shows each hint for */
#define PAUSE_HINT_DRAWS 180
/* the steps the boxes rest after a hint's window ends */
#define REST_STEPS 301

/* a box's modes, in its work's mode */
enum search_box_mode {
    /* it closes, then turns back to the work's ang.y before it waits */
    MODE_CLOSING,
    /* it waits for an ask */
    MODE_WAITING,
    /* it opens, turning toward the asker, and gives its hint once open */
    MODE_OPENING,
    /* it shows its hint, turning toward the asker, until the window ends */
    MODE_SHOWING
};
/* the slot of the player who asked */
#define ASKER(work) ((work)->tick)
/* the submode that marks an SGSCHBOX. a SCHBOX's is 0 */
#define SUBMODE_SG 6

#define BOX_RADIUS 8.0F
#define ASK_RANGE 50.0F
/* an SGSCHBOX puts each of its players nearer than HOLD_RANGE back out to
 * HOLD_RANGE. one nearer than HOLD_MIN stays, so the move never divides by
 * 0 */
#define HOLD_RANGE 12.0F
#define HOLD_MIN 0.01F

#define OPEN_STEP 0.05F
/* while the boxes rest, a box without a hint's window fades by OPACITY_STEP
 * a step, down to OPACITY_RESTING. otherwise it comes back up to 1 at the
 * same rate */
#define OPACITY_STEP 0.09F
#define OPACITY_RESTING 0.4F
/* the most the box turns in a step, in Angle units */
#define TURN_STEP 0x200
/* the box bobs up to BOB_HEIGHT above and below the work's pos. bob_ang
 * advances BOB_STEP a step while the box opens or shows its hint */
#define BOB_STEP 0x300
#define BOB_HEIGHT 0.7F
/* the top turns about y by SPIN_STEP Angle units for each count of
 * game_state.steps */
#define SPIN_STEP 0x200
/* the box leaves over LEAVE_STEPS steps once its hint's window ends. on each
 * step but the last, the box rises LEAVE_RISE and turns LEAVE_TURN */
#define LEAVE_STEPS 60
#define LEAVE_RISE 1.0F
#define LEAVE_TURN 0x111
/* 0 until the box's hint's window ends. it then counts the steps of the
 * box's leave */
#define LEAVE_TIMER(work) ((work)->timer)
/* the steps each page of a hint shows, where the page has no W code */
#define HINT_PAGE_STEPS 120
#define HINT_SOUND 0x8013
#define HINT_PRIORITY 0
#define HINT_VOLUME 0
/* a box that hasn't found its floor probes for it every FLOOR_STEPS steps, a
 * power of 2 */
#define FLOOR_STEPS 4

/* the box draws only where a ball of VIEW_RADIUS about the work's pos isn't
 * wholly behind the camera. an opaque box draws its panel only within
 * PANEL_DEPTH ahead. a faded box draws its panel within FADED_PANEL_DEPTH,
 * and its top within FADED_TOP_DEPTH */
#define VIEW_RADIUS 20.0F
#define PANEL_DEPTH 200.0F
#define FADED_PANEL_DEPTH 100.0F
#define FADED_TOP_DEPTH 300.0F
#define SHADOW_LIFT 0.2F
#define SHADOW_SCALE 3.0F
/* a color channel's greatest value */
#define CHANNEL_MAX 255
/* a packed ARGB color: alpha, then gray in red, green and blue */
#define ARGB_GRAY(alpha, gray)                                                 \
    (((alpha) << 24) + ((gray) << 16) + ((gray) << 8) + (gray))

/* a box's record, in its task's motion */
struct search_box {
    Float opacity; /* the box's opacity, from OPACITY_RESTING to 1 */
    Float open; /* how far open the box is, from 0 to 1 */
    /* where the box faces. the work's ang.y is where it faces at rest */
    Angle ang_y;
    /* the point on the floor below the work's pos, where the shadow falls.
     * it is the work's pos until the step finds a floor. the shadow draws
     * only while its y isn't the work's pos.y */
    NJS_POINT3 floor;
    Angle spin; /* the top's turn about y */
    Angle bob_ang;
    Float bob;
    struct task *message; /* the window of the box's hint, or null for none */
};

#define BOX(task) ((struct search_box *)(task)->motion.p)
/* pause_hint_task's window, which it keeps in its motion: the window's task,
 * or null */
#define WINDOW(task) ((task)->motion.task)

#define SEARCH_BOX_TEXTURES 7
static NJS_TEXNAME search_box_texnames[SEARCH_BOX_TEXTURES] = {
#include "shared/search_box/search_box_texnames.inc"
};
static NJS_TEXLIST search_box_texlist = {
#include "shared/search_box/search_box_texlist.inc"
};
/* the top's model: a chunk model of TOP_POINTS points, each with its
 * normal */
#define TOP_POINTS 52
static Sint16 search_box_top_polygons[519] = {
#include "shared/search_box/search_box_top_polygons.inc"
};
static Sint32 search_box_top_vertices[315] = {
#include "shared/search_box/search_box_top_vertices.inc"
};
static NJS_CNK_MODEL search_box_top_model = {
#include "shared/search_box/search_box_top_model.inc"
};
/* the top's node, the box's child. the draws read only its model and its
 * pos */
static NJS_CNK_OBJECT search_box_top_object = {
#include "shared/search_box/search_box_top_object.inc"
};
/* the box's model: a chunk model of BOX_POINTS points, each with its
 * normal */
#define BOX_POINTS 71
static Sint16 search_box_polygons[647] = {
#include "shared/search_box/search_box_polygons.inc"
};
static Sint32 search_box_vertices[429] = {
#include "shared/search_box/search_box_vertices.inc"
};
static NJS_CNK_MODEL search_box_model = {
#include "shared/search_box/search_box_model.inc"
};
/* the box's node. the draws read only its model and its child */
static NJS_CNK_OBJECT search_box_object = {
#include "shared/search_box/search_box_object.inc"
};
#define PANEL_TEXTURES 1
static NJS_TEXNAME search_box_panel_texname[PANEL_TEXTURES] = {
#include "shared/search_box/search_box_panel_texname.inc"
};
static NJS_TEXLIST search_box_panel_texlist = {
#include "shared/search_box/search_box_panel_texlist.inc"
};
static Sint16 search_box_panel_polygons[25] = {
#include "shared/search_box/search_box_panel_polygons.inc"
};
/* the panel's model: a chunk model of PANEL_POINTS points, each with its
 * color. the draws set every color from how far the box is open */
#define PANEL_POINTS 4
struct search_box_panel_vertex_list {
    Sint32 head[2];
    struct chunk_vertex_d8 points[PANEL_POINTS];
    Sint32 end;
};
static struct search_box_panel_vertex_list search_box_panel_vertices = {
#include "shared/search_box/search_box_panel_vertices.inc"
};
static NJS_CNK_MODEL search_box_panel_model = {
#include "shared/search_box/search_box_panel_model.inc"
};
static NJS_CNK_OBJECT search_box_panel_object = {
#include "shared/search_box/search_box_panel_object.inc"
};
/* @hack type: the panel's vertex list taken as its head array, where
 * (Sint32 *)&search_box_panel_vertices would do */
#define PANEL_VLIST (search_box_panel_vertices.head)

NJS_TEXLIST *search_box_texlists[3] = {
#include "shared/search_box/search_box_texlists.inc"
};

static void pause_hint_draw(struct task *task);
static void pause_hint_step(struct task *task);
static void pause_hint_destroy(struct task *task);
static void search_box_destroy(struct task *task);
static void search_box_step(struct task *task);
static void search_box_draw(struct task *task);
static void search_box_draw_compiled(struct task *task);

/* a box's builds in search_box_builds and search_box_faded_builds. no draw
 * reads BUILD_PANEL's: the panel draws from its model, whose colors each draw
 * sets */
#define BUILD_BOX 0
#define BUILD_TOP 1
#define BUILD_PANEL 2
#define SEARCH_BOX_BUILDS 3
/* TRUE where the last call of search_box_build compiled all six
 * builds. a box set up while it is TRUE draws from them */
static Bool search_box_compiled = FALSE;
/* the builds an opaque box draws from */
static struct model_build search_box_builds[SEARCH_BOX_BUILDS] = {
#include "shared/search_box/search_box_builds.inc"
};
/* the builds a box at OPACITY_RESTING draws from */
static struct model_build search_box_faded_builds[SEARCH_BOX_BUILDS] = {
#include "shared/search_box/search_box_faded_builds.inc"
};
#define SEARCH_BOX_SHAPES 1
/* clang-format off */
static struct collision_shape search_box_shapes[SEARCH_BOX_SHAPES] = {
#include "shared/search_box/search_box_shapes.inc"
};
/* clang-format on */
/* the task that times the boxes' rest, or null while none runs */
static struct task *search_box_rest_task = NULL;
/* TRUE where the last hint a box gave was its piece's last */
static Bool search_box_gave_last_hint = FALSE;
/* the current piece's hints, in the order given, for the pause screen. the
 * first null ends them */
/* @hack type: pause_hints declared extern here with no size, and defined
 * after search_box_step, where a static array of HUNT_HINTS defined here
 * would do */
extern char *pause_hints[];
/* the task that shows pause_hints, or null */
static struct task *pause_hint_task;

Uint8 *search_box_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, search_box_builds, SEARCH_BOX_BUILDS) ==
        SEARCH_BOX_BUILDS) {
        search_box_compiled = TRUE;
    }
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    constant_material_set(OPACITY_RESTING, 1.0F, 1.0F, 1.0F);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
    if (search_box_compiled &&
        model_builds_compile(&buf, search_box_faded_builds,
                             SEARCH_BOX_BUILDS) == SEARCH_BOX_BUILDS) {
        search_box_compiled = TRUE;
    } else {
        search_box_compiled = FALSE;
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    return buf;
}

static void search_box_rest_destroy(struct task *task)
{
    if (search_box_rest_task == task) {
        search_box_rest_task = NULL;
    }
}

static void search_box_rest_step(struct task *task)
{
    struct task_work *work = task->work;

    work->timer++;
    if (work->timer > REST_STEPS) {
        task_destroy(task);
    }
}

/* start the boxes' rest, or start it over where it runs */
static void search_box_rest_start(void)
{
    struct task *task;

    if (search_box_rest_task == NULL) {
        task = task_create(TASK_WORK, HINT_LEVEL, search_box_rest_step,
                           "SearchBoxManExec");
        if (task != NULL) {
            /* @hack redundant: the step stored again after task_create
             * stored it */
            task->step = search_box_rest_step;
            task->destructor = search_box_rest_destroy;
            search_box_rest_task = task;
        }
    }
    if (search_box_rest_task != NULL) {
        search_box_rest_task->work->timer = 1;
    }
}

/* nonzero while the boxes rest. a rest lasts REST_STEPS steps from the end
 * of a hint's window. the boxes also rest after a box gives its piece's last
 * hint. that rest lasts while hunt_current->hints is HUNT_HINTS - 1 or more. it
 * ends when a player takes the piece, which sets hints back to 0 */
static int search_box_resting(void)
{
    struct task *task = search_box_rest_task;

    /* @hack spelling: the else after each return, where the statements after
     * each if would do */
    if (task != NULL && task->work->timer != 0) {
        return task->work->timer;
    } else if (search_box_gave_last_hint && hunt_current != NULL &&
               hunt_current->hints >= HUNT_HINTS - 1) {
        return 1;
    } else {
        return 0;
    }
}

static void pause_hint_start(void)
{
    struct task *task;

    if (pause_hint_task == NULL) {
        task = task_create(TASK_WORK, HINT_LEVEL, pause_hint_step,
                           "PauseHintExec");
        if (task != NULL) {
            pause_hint_task = task;
            task->draw = pause_hint_draw;
            /* @hack redundant: the step stored again after task_create
             * stored it */
            task->step = pause_hint_step;
            task->destructor = pause_hint_destroy;
        }
    }
}

/* on the pause screen, show pause_hints in a message window, each in turn
 * for PAUSE_HINT_DRAWS draws.
 * L on the pad of the player who paused steps back a hint, and R steps on.
 * L and R held together on that pad hold the shown hint. so do X and Y held
 * on player 0's pad */
static void pause_hint_draw(struct task *task)
{
    struct task_work *work = task->work;
    int count;
    char **text;
    char **texts;

    if (stage_get_mode() != STAGE_PAUSE ||
        (player_pads[0]->on & HINT_FREEZE_BUTTONS) == HINT_FREEZE_BUTTONS ||
        camera_current_index != 0) {
        return;
    }
    if (hunt_current == NULL || hunt_current->hints <= 0) {
        return;
    }
    if (message_window_current() == NULL && WINDOW(task) == NULL) {
        work->timer = 0;
    }
    if (work->timer % PAUSE_HINT_DRAWS == 0 &&
        (message_window_current() == NULL ||
         message_window_current() == WINDOW(task))) {
        /* @hack spelling: texts, a copy of pause_hints, where pause_hints
         * itself would do */
        texts = pause_hints;
        /* @hack spelling: text stepped through the hints beside count, where
         * count alone would index them */
        for (count = 0, text = texts; text < pause_hints + HUNT_HINTS;
             text++, count++) {
            if (*text == NULL) {
                break;
            }
        }
        if (count > 0) {
            /* @hack spelling: a switch on count, where an if of count == 1
             * would do */
            switch (count) {
            case 1:
                if (message_window_current() == NULL && WINDOW(task) == NULL) {
                    message_window_open(MESSAGE_WINDOW_INSTANT, texts[0], 0,
                                        game_state.text_language,
                                        &WINDOW(task));
                }
                break;
            default:
                if (WINDOW(task) != NULL) {
                    task_destroy(WINDOW(task));
                }
                /* @hack spelling: count reused for the index of the hint to
                 * show, where a local of its own would do */
                count = work->timer / PAUSE_HINT_DRAWS % count;
                message_window_open(MESSAGE_WINDOW_INSTANT, texts[count], 0,
                                    game_state.text_language, &WINDOW(task));
            }
        }
    }
    if ((game_state.paused == PAUSED_BY_PLAYER_0 &&
         (player_pads[0]->on & HINT_STEP_BUTTONS) == HINT_STEP_BUTTONS) ||
        (game_state.paused == PAUSED_BY_PLAYER_1 &&
         (player_pads[1]->on & HINT_STEP_BUTTONS) == HINT_STEP_BUTTONS)) {
        return;
    } else if ((game_state.paused == PAUSED_BY_PLAYER_0 &&
                (player_pads[0]->press & HINT_STEP_BUTTONS) == PDD_DGT_TL) ||
               (game_state.paused == PAUSED_BY_PLAYER_1 &&
                (player_pads[1]->press & HINT_STEP_BUTTONS) == PDD_DGT_TL)) {
        /* @bug arithmetic: L during the first hint sets the timer, a Uint16,
         * to 65356, which isn't a multiple of PAUSE_HINT_DRAWS. the first
         * hint then stays 164 draws more, until the timer reaches 65520.
         * the hint at index 364 % count shows next, for 16 draws, until the
         * timer wraps to 0. the multiply is at 0x3c6 of the object's code */
        work->timer = ((work->timer / PAUSE_HINT_DRAWS) - 1) * PAUSE_HINT_DRAWS;
    } else if ((game_state.paused == PAUSED_BY_PLAYER_0 &&
                (player_pads[0]->press & HINT_STEP_BUTTONS) == PDD_DGT_TR) ||
               (game_state.paused == PAUSED_BY_PLAYER_1 &&
                (player_pads[1]->press & HINT_STEP_BUTTONS) == PDD_DGT_TR)) {
        work->timer = ((work->timer / PAUSE_HINT_DRAWS) + 1) * PAUSE_HINT_DRAWS;
    } else {
        work->timer++;
    }
}

/* close the window pause_hint_draw opens on the pause screen */
static void pause_hint_step(struct task *task)
{
    if (WINDOW(task) != NULL) {
        task_destroy(WINDOW(task));
    }
}

static void pause_hint_destroy(struct task *task)
{
    if (WINDOW(task) != NULL) {
        task_destroy(WINDOW(task));
    }
    if (pause_hint_task == task) {
        pause_hint_task = NULL;
    }
    WINDOW(task) = NULL;
}

static void search_box_draw_shadow(struct task *task)
{
    njPushMatrixEx();
    njTranslate(NULL, BOX(task)->floor.x, BOX(task)->floor.y + SHADOW_LIFT,
                BOX(task)->floor.z);
    njScale(NULL, SHADOW_SCALE, 1.0F, SHADOW_SCALE);
    njCnkModDrawObject(&shadow_object);
    njPopMatrixEx();
}

void search_box_init(struct task *task)
{
    struct task_work *work = task->work;

    if (game_state.mission == MISSION_RINGS ||
        game_state.mission == MISSION_CHAO) {
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        return;
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct search_box));
    if (task->motion.p == NULL) {
        return;
    }
    if (search_box_compiled) {
        task->draw = search_box_draw_compiled;
    } else {
        task->draw = search_box_draw;
    }
    task->step = search_box_step;
    task->destructor = search_box_destroy;
    work->mode = MODE_CLOSING;
    work->submode = 0;
    LEAVE_TIMER(work) = 0;
    pause_hint_start();
    collision_init_in_place(task, search_box_shapes, SEARCH_BOX_SHAPES,
                            COLLISION_OBJECT);
    if (work->collision != NULL) {
        work->collision->radius = BOX_RADIUS;
    }
    BOX(task)->opacity = 1.0F;
    BOX(task)->open = 0.0F;
    BOX(task)->ang_y = work->ang.y;
    BOX(task)->floor = work->pos;
    BOX(task)->bob = 0.0F;
    BOX(task)->bob_ang = 0;
    BOX(task)->message = NULL;
}

void search_box_sg_init(struct task *task)
{
    struct task_work *work = task->work;

    if (game_state.mission == MISSION_RINGS ||
        game_state.mission == MISSION_CHAO) {
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        return;
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct search_box));
    if (task->motion.p == NULL) {
        return;
    }
    if (search_box_compiled) {
        task->draw = search_box_draw_compiled;
    } else {
        task->draw = search_box_draw;
    }
    task->step = search_box_step;
    task->destructor = search_box_destroy;
    work->mode = MODE_CLOSING;
    work->submode = SUBMODE_SG;
    LEAVE_TIMER(work) = 0;
    pause_hint_start();
    BOX(task)->opacity = 1.0F;
    BOX(task)->open = 0.0F;
    BOX(task)->ang_y = work->ang.y;
    BOX(task)->floor = work->pos;
    BOX(task)->bob = 0.0F;
    BOX(task)->bob_ang = 0;
    BOX(task)->message = NULL;
}

static void search_box_destroy(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    if (BOX(task)->message != NULL) {
        task_destroy(BOX(task)->message);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void search_box_step(struct task *task)
{
    struct task_work *work = task->work;
    Float floor_y;
    Float distance;
    Uint32 steps;
    Angle turn;
    /* @hack spelling: (Sint16)turn kept in short_turn, where the cast at each
     * test would do */
    int short_turn;
    int player;
    int hint;
    char *text;
    char **later;

    /* @hack spelling: the leave's timer tested with !, where == 0 would do */
    if (!LEAVE_TIMER(work) && placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a test of floor.y that no path reaches */
    if (0) {
        if (BOX(task)->floor.y) {
        }
    }
    /* @hack type: game_state's steps read as game_state_steps, where
     * game_state.steps would do */
    steps = game_state_steps;
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
    /* @hack spelling: to declared in a block from here to the end, where a
     * declaration among the locals would do */
    {
        NJS_POINT3 to;

        BOX(task)->spin = steps * SPIN_STEP;
        BOX(task)->bob = njSin(BOX(task)->bob_ang) * BOB_HEIGHT;
        switch (work->mode) {
        case MODE_CLOSING:
            if (BOX(task)->open > 0.0F) {
                BOX(task)->open -= OPEN_STEP;
                if (BOX(task)->open < 0.0F) {
                    BOX(task)->open = 0.0F;
                }
            } else {
                turn = work->ang.y - BOX(task)->ang_y;
                if ((short_turn = (Sint16)turn) > 0) {
                    if (short_turn > TURN_STEP) {
                        turn = TURN_STEP;
                    }
                } else if (short_turn < -TURN_STEP) {
                    turn = -TURN_STEP;
                }
                BOX(task)->ang_y += (Sint16)turn;
                if ((Sint16)turn == 0) {
                    work->mode = MODE_WAITING;
                }
            }
            break;
        case MODE_WAITING:
            player = player_near(&work->pos, ASK_RANGE);
            if (player != 0) {
                if (work->submode == SUBMODE_SG &&
                    !(game_state.sg_players & (1 << (player - 1)))) {
                    player = 0;
                }
                if (player != 0 &&
                    player_records[player - 1]->action_taken !=
                        PLAYER_MODE_ASK_HINT &&
                    message_window_current() == NULL) {
                    njPushMatrixEx();
                    njUnitMatrix(NULL);
                    njTranslateEx(&player_work[player - 1]->pos);
                    njRotateZ(NULL, player_work[player - 1]->ang.z);
                    njRotateX(NULL, player_work[player - 1]->ang.x);
                    njRotateY(NULL, HALF_TURN - player_work[player - 1]->ang.y);
                    njInvertMatrix(NULL);
                    njCalcPoint(NULL, &work->pos, &to);
                    njPopMatrixEx();
                    if (to.x < 0.0F) {
                        player_records[player - 1]->action =
                            PLAYER_MODE_ASK_HINT;
                    }
                    player = 0;
                }
            }
            if (!search_box_resting() && player > 0 &&
                message_window_current() == NULL &&
                BOX(task)->message == NULL) {
                player--;
                /* @hack redundant: a test of player that always holds */
                if (player >= 0) {
                    player_records[player]->action_taken = 0;
                    ASKER(work) = player;
                    work->mode = MODE_OPENING;
                }
            }
            break;
        case MODE_OPENING:
            BOX(task)->open += OPEN_STEP;
            if (BOX(task)->open >= 1.0F) {
                hint = HUNT_HINTS;
                if (hunt_current != NULL) {
                    hint = hunt_current->hints;
                }
                if (hint < 0) {
                    hint = 0;
                }
                if (hint >= HUNT_HINTS) {
                    hint = HUNT_HINTS - 1;
                }
                if (hunt_current != NULL &&
                    hunt_current->hints >= HUNT_HINTS - 1) {
                    search_box_gave_last_hint = TRUE;
                } else {
                    search_box_gave_last_hint = FALSE;
                }
                text = hunt_next_hint();
                BOX(task)->open = 1.0F;
                work->mode = MODE_SHOWING;
                if (text == NULL) {
                    text = "NoText";
                }
                pause_hints[hint] = text;
                for (later = &pause_hints[hint + 1];
                     later < pause_hints + HUNT_HINTS; later++) {
                    *later = NULL;
                }
                /* @hack type: game_state's text_language read as
                 * game_state_text_language, where game_state.text_language
                 * would do */
                message_window_open(MESSAGE_WINDOW_FADE, text, HINT_PAGE_STEPS,
                                    game_state_text_language,
                                    &BOX(task)->message);
                sound_play(HINT_SOUND, NULL, HINT_PRIORITY, HINT_VOLUME);
            }
            to.x = player_work[ASKER(work)]->pos.x - work->pos.x;
            to.y = player_work[ASKER(work)]->pos.y - work->pos.y;
            to.z = player_work[ASKER(work)]->pos.z - work->pos.z;
            /* @hack spelling: the turn worked out in two statements, where
             * one expression would do */
            turn = (Angle)(atan2f(to.x, to.z) * ANGLE_PER_RADIAN);
            turn -= BOX(task)->ang_y;
            if ((short_turn = (Sint16)turn) > 0) {
                if (short_turn > TURN_STEP) {
                    turn = TURN_STEP;
                }
            } else if (short_turn < -TURN_STEP) {
                turn = -TURN_STEP;
            }
            BOX(task)->bob_ang += BOB_STEP;
            BOX(task)->ang_y += (Sint16)turn;
            break;
        case MODE_SHOWING:
            if (BOX(task)->message == NULL) {
                work->mode = MODE_CLOSING;
                search_box_rest_start();
                LEAVE_TIMER(work) = 1;
            } else {
                to.x = player_work[ASKER(work)]->pos.x - work->pos.x;
                to.y = player_work[ASKER(work)]->pos.y - work->pos.y;
                to.z = player_work[ASKER(work)]->pos.z - work->pos.z;
                turn = (Angle)(atan2f(to.x, to.z) * ANGLE_PER_RADIAN);
                /* @hack spelling: the box's angle subtracted inside the
                 * test, where one expression for turn would do */
                if ((short_turn = (Sint16)(turn -= BOX(task)->ang_y)) > 0) {
                    if (short_turn > TURN_STEP) {
                        turn = TURN_STEP;
                    }
                } else if (short_turn < -TURN_STEP) {
                    turn = -TURN_STEP;
                }
                BOX(task)->ang_y += (Sint16)turn;
                BOX(task)->bob_ang += BOB_STEP;
            }
            break;
        }
        if (BOX(task)->message == NULL && search_box_resting()) {
            if (BOX(task)->opacity > OPACITY_RESTING) {
                BOX(task)->opacity -= OPACITY_STEP;
                if (BOX(task)->opacity < OPACITY_RESTING) {
                    BOX(task)->opacity = OPACITY_RESTING;
                }
            }
        } else if (BOX(task)->opacity < 1.0F) {
            BOX(task)->opacity += OPACITY_STEP;
            if (BOX(task)->opacity > 1.0F) {
                BOX(task)->opacity = 1.0F;
            }
        }
        if (LEAVE_TIMER(work) > 0) {
            LEAVE_TIMER(work)++;
            /* @hack spelling: the else after the if's return, where the
             * statements after the if would do */
            if (LEAVE_TIMER(work) > LEAVE_STEPS) {
                if (task->placement != NULL) {
                    placement_kill(task);
                } else {
                    task_kill(task);
                }
                return;
            } else {
                work->pos.y += LEAVE_RISE;
                BOX(task)->ang_y += LEAVE_TURN;
            }
        }
        if (work->submode != SUBMODE_SG) {
            collision_register(task);
        } else {
            /* @hack spelling: player = player + 1, where player++ would do */
            for (player = 0;
                 player < GAME_PLAYERS && player_work[player] != NULL;
                 player = player + 1) {
                if (game_state.sg_players & (1 << player)) {
                    to.x = player_work[player]->pos.x - work->pos.x;
                    to.y = player_work[player]->pos.y - work->pos.y;
                    to.z = player_work[player]->pos.z - work->pos.z;
                    distance = njScalor(&to);
                    if (distance > HOLD_MIN && distance < HOLD_RANGE) {
                        /* @hack redundant: a store to to.x that no path
                         * reaches */
                        if (0) {
                            to.x = HOLD_RANGE;
                        }
                        /* @hack spelling: distance reused for the ratio,
                         * where a local of its own would do */
                        distance = HOLD_RANGE / distance;
                        to.x *= distance;
                        to.y *= distance;
                        to.z *= distance;
                        player_set_pos(player, to.x + work->pos.x,
                                       to.y + work->pos.y, to.z + work->pos.z);
                    }
                }
            }
        }
    }
}

/* pause_hint_task is defined after pause_hints, so that its word lies after
 * the array's, where the image holds it */
char *pause_hints[HUNT_HINTS] = {NULL, NULL, NULL};
static struct task *pause_hint_task = NULL;

static void search_box_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *top;
    Float depth;
    Float opacity;
    Angle spin;
    Uint32 alpha;
    Uint32 gray;

    if (work->submode == SUBMODE_SG) {
        /* @hack redundant: an if of its own whose test ends in && 1, where
         * one if of both tests would do */
        if (!(game_state.sg_players & (1 << camera_current_index)) && 1) {
            return;
        }
    }
    if (!sphere_in_front(&work->pos, VIEW_RADIUS, &depth)) {
        return;
    }
    opacity = BOX(task)->opacity;
    spin = BOX(task)->spin;
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + BOX(task)->bob, work->pos.z);
    njRotateY(NULL, BOX(task)->ang_y);
    object = &search_box_object;
    gray = (Uint32)(BOX(task)->open * CHANNEL_MAX);
    if (gray > CHANNEL_MAX) {
        gray = CHANNEL_MAX;
    }
    if (opacity == 1.0F) {
        njSetTexture(&search_box_texlist);
        njCnkSimpleDrawModel(object->model);
        alpha = CHANNEL_MAX;
        if (depth < PANEL_DEPTH) {
            njSetTexture(&search_box_panel_texlist);
            vlist_set_color(PANEL_VLIST, ARGB_GRAY(alpha, gray));
            njCnkEasyDrawObject(&search_box_panel_object);
        }
        top = object->child;
        njSetTexture(&search_box_texlist);
        njTranslateEx((NJS_VECTOR *)top->pos);
        njRotateY(NULL, spin);
        njCnkSimpleDrawModel(top->model);
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
        njSetTexture(&search_box_texlist);
        njCnkSimpleDrawModel(object->model);
        if (depth < FADED_PANEL_DEPTH) {
            njSetTexture(&search_box_panel_texlist);
            vlist_set_color(PANEL_VLIST, ARGB_GRAY(alpha, gray));
            njCnkEasyDrawObject(&search_box_panel_object);
        }
        if (depth < FADED_TOP_DEPTH) {
            top = object->child;
            njSetTexture(&search_box_texlist);
            njTranslateEx((NJS_VECTOR *)top->pos);
            njRotateY(NULL, spin);
            njCnkSimpleDrawModel(top->model);
        }
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    njPopMatrixEx();
    if (BOX(task)->floor.y != work->pos.y) {
        search_box_draw_shadow(task);
    }
}

static void search_box_draw_compiled(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *top;
    Float depth;
    Float opacity;
    Angle spin;
    Uint32 alpha;
    Uint32 gray;

    if (work->submode == SUBMODE_SG) {
        /* @hack redundant: an if of its own whose test ends in && 1, where
         * one if of both tests would do */
        if (!(game_state.sg_players & (1 << camera_current_index)) && 1) {
            return;
        }
    }
    if (!sphere_in_front(&work->pos, VIEW_RADIUS, &depth)) {
        return;
    }
    opacity = BOX(task)->opacity;
    spin = BOX(task)->spin;
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + BOX(task)->bob, work->pos.z);
    njRotateY(NULL, BOX(task)->ang_y);
    object = &search_box_object;
    gray = (Uint32)(BOX(task)->open * CHANNEL_MAX);
    if (gray > CHANNEL_MAX) {
        gray = CHANNEL_MAX;
    }
    if (opacity == 1.0F) {
        njSetTexture(&search_box_texlist);
        njDirectDrawObject(search_box_builds[BUILD_BOX].built);
        alpha = CHANNEL_MAX;
        if (depth < PANEL_DEPTH) {
            njSetTexture(&search_box_panel_texlist);
            vlist_set_color(PANEL_VLIST, ARGB_GRAY(alpha, gray));
            njCnkEasyDrawObject(&search_box_panel_object);
        }
        top = object->child;
        njSetTexture(&search_box_texlist);
        njTranslateEx((NJS_VECTOR *)top->pos);
        njRotateY(NULL, spin);
        njDirectDrawObject(search_box_builds[BUILD_TOP].built);
    } else if (opacity == OPACITY_RESTING) {
        alpha = (Uint32)(opacity * CHANNEL_MAX);
        if (alpha > CHANNEL_MAX) {
            alpha = CHANNEL_MAX;
        }
        njSetTexture(&search_box_texlist);
        njDirectDrawObject(search_box_faded_builds[BUILD_BOX].built);
        if (depth < FADED_PANEL_DEPTH) {
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            constant_material_set(opacity, 1.0F, 1.0F, 1.0F);
            njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
            njSetTexture(&search_box_panel_texlist);
            vlist_set_color(PANEL_VLIST, ARGB_GRAY(alpha, gray));
            njCnkEasyDrawObject(&search_box_panel_object);
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        }
        if (depth < FADED_TOP_DEPTH) {
            top = object->child;
            njSetTexture(&search_box_texlist);
            njTranslateEx((NJS_VECTOR *)top->pos);
            njRotateY(NULL, spin);
            njDirectDrawObject(search_box_faded_builds[BUILD_TOP].built);
        }
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
        njSetTexture(&search_box_texlist);
        njCnkSimpleDrawModel(object->model);
        if (depth < FADED_PANEL_DEPTH) {
            njSetTexture(&search_box_panel_texlist);
            vlist_set_color(PANEL_VLIST, ARGB_GRAY(alpha, gray));
            njCnkEasyDrawObject(&search_box_panel_object);
        }
        if (depth < FADED_TOP_DEPTH) {
            top = object->child;
            njSetTexture(&search_box_texlist);
            njTranslateEx((NJS_VECTOR *)top->pos);
            njRotateY(NULL, spin);
            njCnkSimpleDrawModel(top->model);
        }
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    njPopMatrixEx();
    if (BOX(task)->floor.y != work->pos.y) {
        search_box_draw_shadow(task);
    }
}
