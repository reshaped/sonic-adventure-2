/* a racer's cheers, which spend its stamina, and the gauge that shows the
 * stamina. a cheer lowers race_target_stamina by chao_tuning.race_cheer_cost.
 * it lasts until race_stamina drains down to the target */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "chao/behavior.h"
#include "chao/face.h"
#include "chao/icon.h"
#include "chao/race.h"
#include "chao/tuning.h"
#include "chao/ui.h"
#include "chao/work.h"
#include "chaostgrace/banner_draw.h"
#include "chaostgrace/race.h"
#include "draw/lines.h"
#include "sound/sound.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

/* the gauge stacks up to STAMINA_BARS bars, each STAMINA_PER_BAR of stamina
 * across STAMINA_BAR_WIDTH pixels */
enum { FIRST_BAR, SECOND_BAR, THIRD_BAR, STAMINA_BARS };
enum { STAMINA_BAR_WIDTH = 160, STAMINA_PER_BAR = 20000 };
/* the bars' colors as ARGB, from the first up. the drain covers the top bar's
 * fill above the target in DRAIN_COLOR. the racer's name shows in NAME_COLOR */
#define FIRST_BAR_COLOR 0xffff9f00
#define SECOND_BAR_COLOR 0xffffff00
#define THIRD_BAR_COLOR 0xff00ff00
#define DRAIN_COLOR 0xffff0000
#define NAME_COLOR 0xff5555ff
/* a frame's drain is chao_tuning.race_stamina_step, and one more for each
 * GAP_PER_DRAIN the stamina stands above its target */
#define GAP_PER_DRAIN 1000
/* the volume of a cheer's sound, and the steps its mark shows */
#define CHEER_VOLUME 40
#define CHEER_MARK_STEPS 90
/* a Chao without a name shows four NAMELESS_GLYPHs in its place */
#define NAMELESS_GLYPH 31

enum {
    CELL_STAMINA = 17,
    CELL_GAUGE_LEFT = 42,
    CELL_GAUGE_MIDDLE = 43,
    CELL_GAUGE_RIGHT = 44
};
/* a cheer's sound, by its entrant's number modulo CHEER_SOUNDS */
#define CHEER_SOUNDS 4
enum {
    CHEER_FIRST = 0x4009,
    CHEER_SECOND = 0x4008,
    CHEER_THIRD = 0x400d,
    CHEER_FOURTH = 0x400c
};

struct stamina_label {
    NJS_POINT2 screen;
    Float depth;
};

static void stamina_step(struct task *task)
{
    struct task_work *work = task->work;
    Uint8 entrant = work->tick;
    struct task *chao = race_entrant(entrant);
    struct chao_parameters *params;
    struct chao_tuning *tuning;
    struct chao_work *chao_work;
    Bool pressed;
    int step;
    chao_behavior_fn behavior;
    const PDS_PERIPHERAL *pad;

    if (!chao) {
        return;
    }
    chao_work = CHAO_WORK(chao);
    params = &chao_work->profile.parameters;
    tuning = &chao_tuning;
    if (race_mode() == RACE_RUNNING) {
        if (work->timer > 0) {
            work->timer--;
        } else {
            pressed = FALSE;
            /* in a multiplayer race two racers share each pad. L cheers the
             * even one, and R the odd */
            if (garden_race_group() == RACE_MULTI) {
                if (entrant & 1) {
                    if (player_pads[entrant >> 1]->press & PDD_DGT_TR) {
                        pressed = TRUE;
                    }
                } else if (player_pads[entrant >> 1]->press & PDD_DGT_TL) {
                    pressed = TRUE;
                }
            } else if (((pad = player_pads[0])->press & PDD_DGT_TL) ||
                       (pad->press & PDD_DGT_TR)) {
                pressed = TRUE;
            }
            if (pressed) {
                behavior = chao_current_behavior(chao);
                if (behavior != race_cross_finish &&
                    behavior != race_end_finish &&
                    behavior != race_victory_pose) {
                    if (params->race_stamina) {
                        work->timer = tuning->race_cheer_interval;
                        if ((Uint32)params->race_target_stamina >
                            tuning->race_cheer_cost) {
                            params->race_target_stamina -=
                                tuning->race_cheer_cost;
                        } else {
                            params->race_target_stamina = 0;
                        }
                        chao_show_mark(chao, CHAO_MARK_EXCLAMATION,
                                       CHEER_MARK_STEPS);
                        chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED,
                                           CHAO_FACE_HOLD);
                    }
                    switch (entrant % CHEER_SOUNDS) {
                    case 0:
                        sound_play(CHEER_FIRST, NULL, 0, CHEER_VOLUME);
                        break;
                    case 1:
                        sound_play(CHEER_SECOND, NULL, 0, CHEER_VOLUME);
                        break;
                    case 2:
                        sound_play(CHEER_THIRD, NULL, 0, CHEER_VOLUME);
                        break;
                    case 3:
                        sound_play(CHEER_FOURTH, NULL, 0, CHEER_VOLUME);
                        break;
                    }
                    race_camera_cheer_target = entrant;
                    race_start_cheer(entrant);
                }
            }
        }
    }
    if (params->race_target_stamina > params->race_stamina) {
        params->race_target_stamina = params->race_stamina;
    }
    /* @hack redundant: a test of the target against the stamina, which the
     * clamp above makes always hold */
    if (params->race_target_stamina <= params->race_stamina) {
        step = params->race_stamina - params->race_target_stamina;
        step = (step / GAP_PER_DRAIN) + tuning->race_stamina_step;
        if (params->race_stamina && params->race_stamina <= step) {
            params->race_stamina = 0;
            race_end_cheer(entrant);
            if ((Float)rand() * RAND_SCALE < 0.5F) {
                chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, CHAO_FACE_HOLD);
            } else {
                chao_set_timed_eye(chao, 3, CHAO_FACE_HOLD);
            }
            chao_show_mark(chao, CHAO_MARK_ANGER, -1);
        } else {
            params->race_stamina -= step;
            if (params->race_target_stamina > params->race_stamina) {
                params->race_stamina = params->race_target_stamina;
            }
        }
    }
    /* @bug logic: a racer out of stamina keeps its default eye. the eye set
     * above as the stamina runs out, at 0x0c9c63f0, shows for one step at
     * most. this call, at 0x0c9c6434, restores the default on each step the
     * target equals the stamina. once the stamina is 0, that is the step that
     * sets the eye or the next */
    if (params->race_target_stamina == params->race_stamina) {
        race_end_cheer(entrant);
    }
}

/* pixels is the stamina in the gauge's pixels. level counts the full bars
 * under the top one, and width is the top one's fill. target is where the
 * target stamina stands in its own bar */
#define STAMINA_PARTS(params, pixels, width, level, target)                    \
    do {                                                                       \
        (pixels) =                                                             \
            (params)->race_stamina * STAMINA_BAR_WIDTH / STAMINA_PER_BAR;      \
        (width) = (pixels) % STAMINA_BAR_WIDTH;                                \
        (level) = (pixels) / STAMINA_BAR_WIDTH;                                \
        (target) = ((params)->race_target_stamina * STAMINA_BAR_WIDTH /        \
                    STAMINA_PER_BAR) %                                         \
                   STAMINA_BAR_WIDTH;                                          \
    } while (0)

#define NAMELESS(name)                                                         \
    do {                                                                       \
        (name)[0] = NAMELESS_GLYPH;                                            \
        (name)[1] = NAMELESS_GLYPH;                                            \
        (name)[2] = NAMELESS_GLYPH;                                            \
        (name)[3] = NAMELESS_GLYPH;                                            \
        (name)[4] = 0;                                                         \
    } while (0)

/* the depths of the gauge's cells, the top bar's fill, the bar under it and
 * the drain */
#define GAUGE_DEPTH (-0.1F)
#define FILL_DEPTH 0.859F
#define UNDER_DEPTH 0.858F
#define DRAIN_DEPTH 0.86F
#define STAMINA_GAUGE(sprite)                                                  \
    do {                                                                       \
        BANNER_DRAW_CELL(sprite, CELL_GAUGE_LEFT, GAUGE_DEPTH);                \
        BANNER_DRAW_CELL(sprite, CELL_GAUGE_MIDDLE, GAUGE_DEPTH);              \
        BANNER_DRAW_CELL(sprite, CELL_GAUGE_RIGHT, GAUGE_DEPTH);               \
    } while (0)

/* the gauge's frame: blue, or yellow while the racer cheers. a black copy
 * follows at shadow_x and shadow_y */
#define STAMINA_RIM(chao, sprite, shadow_x, shadow_y)                          \
    do {                                                                       \
        if (!race_is_cheering(chao)) {                                         \
            constant_material_set(1.0F, 0.2F, 0.6F, 1.0F);                     \
        } else {                                                               \
            constant_material_set(1.0F, 1.0F, 1.0F, 0.0F);                     \
        }                                                                      \
        STAMINA_GAUGE(sprite);                                                 \
        (sprite).p.x = (shadow_x);                                             \
        (sprite).p.y = (shadow_y);                                             \
        constant_material_set(1.0F, 0.0F, 0.0F, 0.0F);                         \
        STAMINA_GAUGE(sprite);                                                 \
    } while (0)

/* draw the top bar, level, filled to progress. the bar below it, where there
 * is one, draws whole. a level past THIRD_BAR draws no fill */
#define STAMINA_FILL(level, left, top, progress, end, bottom)                  \
    switch (level) {                                                           \
    case THIRD_BAR:                                                            \
        garden_draw_rect(left, top, progress, bottom, FILL_DEPTH,              \
                         THIRD_BAR_COLOR);                                     \
        garden_draw_rect(left, top, end, bottom, UNDER_DEPTH,                  \
                         SECOND_BAR_COLOR);                                    \
        break;                                                                 \
    case SECOND_BAR:                                                           \
        garden_draw_rect(left, top, progress, bottom, FILL_DEPTH,              \
                         SECOND_BAR_COLOR);                                    \
        garden_draw_rect(left, top, end, bottom, UNDER_DEPTH,                  \
                         FIRST_BAR_COLOR);                                     \
        break;                                                                 \
    case FIRST_BAR:                                                            \
        garden_draw_rect(left, top, progress, bottom, FILL_DEPTH,              \
                         FIRST_BAR_COLOR);                                     \
        break;                                                                 \
    }

/* the player's gauge: its name, the STAMINA label, then the gauge, from
 * GAUGE_X */
#define NAME_X 405.0F
#define NAME_Y 87.0F
#define LABEL_Y 102.0F
#define LABEL_SCALE 1.5F
#define GAUGE_X 396.0F
#define GAUGE_Y 115.0F
/* a gauge's bars run from BAR_LEFT right of its x, from BAR_TOP above its y
 * to BAR_BOTTOM below. its shadow lies SHADOW_OFFSET right and down */
#define BAR_LEFT 1.0F
#define BAR_TOP 5.0F
#define BAR_BOTTOM 6.0F
#define SHADOW_OFFSET 1.0F
/* a multiplayer gauge lies GAUGE_RIGHT right of its place's
 * race_hud_positions entry and GAUGE_DOWN below it. the name lies NAME_RIGHT
 * right of the gauge and NAME_UP above it */
#define GAUGE_RIGHT 24.0F
#define GAUGE_DOWN 8.0F
#define NAME_RIGHT 9.0F
#define NAME_UP 17.0F

static void stamina_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint8 entrant = work->tick;
    struct task *chao = race_entrant(entrant);
    struct chao_work *chao_work;
    struct chao_parameters *params;
    Uint32 pixels;
    Uint32 width;
    Uint32 level;
    Uint32 target;
    NJS_SPRITE sprite;
    struct stamina_label label;
    Float x;
    char name[5];

    if (race_mode() != RACE_RUNNING) {
        return;
    }
    if (!chao) {
        return;
    }
    chao_work = CHAO_WORK(chao);
    params = &chao_work->profile.parameters;
    STAMINA_PARTS(params, pixels, width, level, target);
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    BANNER_MATERIAL(sprite, 1.0F);
    x = GAUGE_X;
    /* @hack redundant: name, a nameless Chao's name that nothing draws */
    NAMELESS(name);
    /* @hack redundant: label.depth, which garden_draw_name doesn't read */
    label.depth = -1.0F;
    label.screen.x = NAME_X;
    label.screen.y = NAME_Y;
    /* @hack spelling: the name reached through chao_work, in the test and the
     * call, where params->name would do */
    if (chao_work->profile.parameters.name[0] == 0) {
        char fallback[5];
        NAMELESS(fallback);
        garden_draw_name(&label.screen, fallback, NAME_COLOR, 1.0F, 0);
    } else {
        /* @hack type: the label's address cast to NJS_POINT2 *, where
         * &label.screen would do */
        garden_draw_name((NJS_POINT2 *)&label,
                         chao_work->profile.parameters.name, NAME_COLOR, 1.0F,
                         0);
    }
    sprite.p.x = x;
    sprite.p.y = LABEL_Y;
    sprite.sx = LABEL_SCALE;
    sprite.sy = LABEL_SCALE;
    BANNER_DRAW_CELL(sprite, CELL_STAMINA, GAUGE_DEPTH);
    sprite.p.x = x;
    sprite.p.y = GAUGE_Y;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    STAMINA_RIM(chao, sprite, GAUGE_X + SHADOW_OFFSET, GAUGE_Y + SHADOW_OFFSET);
    STAMINA_FILL(level, GAUGE_X + BAR_LEFT, GAUGE_Y - BAR_TOP,
                 width + x + BAR_LEFT, GAUGE_X + BAR_LEFT + STAMINA_BAR_WIDTH,
                 GAUGE_Y + BAR_BOTTOM);
    if (target > width) {
        target = 0;
    }
    if (pixels) {
        garden_draw_rect(target + x + BAR_LEFT, GAUGE_Y - BAR_TOP,
                         width + x + BAR_LEFT, GAUGE_Y + BAR_BOTTOM,
                         DRAIN_DEPTH, DRAIN_COLOR);
    }
    njFogEnable();
}

static void multiplayer_stamina_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint8 entrant = work->tick;
    struct task *chao = race_entrant(entrant);
    struct chao_work *chao_work;
    struct chao_parameters *params;
    Uint32 pixels;
    Uint32 width;
    Uint32 level;
    Uint32 target;
    NJS_SPRITE sprite;
    /* @hack type: label an NJS_POINT3, whose z garden_draw_name doesn't read,
     * where an NJS_POINT2 would do */
    NJS_POINT3 label;
    Float x;
    Float y;
    char name[5];

    if (race_mode() != RACE_RUNNING && race_mode() != RACE_READY) {
        return;
    }
    if (!chao) {
        return;
    }
    chao_work = CHAO_WORK(chao);
    params = &chao_work->profile.parameters;
    STAMINA_PARTS(params, pixels, width, level, target);
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    BANNER_MATERIAL(sprite, 1.0F);
    x = race_hud_positions[entrant].x + GAUGE_RIGHT;
    y = race_hud_positions[entrant].y + GAUGE_DOWN;
    /* @hack redundant: name, a nameless Chao's name that nothing draws */
    NAMELESS(name);
    label.x = x + NAME_RIGHT;
    label.y = y - NAME_UP;
    label.z = -1.0F;
    /* @hack spelling: the name reached through chao_work, in the test and the
     * call, where params->name would do */
    if (chao_work->profile.parameters.name[0] == 0) {
        char fallback[5];
        NAMELESS(fallback);
        garden_draw_name((NJS_POINT2 *)&label, fallback, NAME_COLOR, 1.0F, 0);
    } else {
        garden_draw_name((NJS_POINT2 *)&label,
                         chao_work->profile.parameters.name, NAME_COLOR, 1.0F,
                         0);
    }
    sprite.p.x = x;
    sprite.p.y = y;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    STAMINA_RIM(chao, sprite, x + SHADOW_OFFSET, y + SHADOW_OFFSET);
    STAMINA_FILL(level, x + BAR_LEFT, y - BAR_TOP, width + x + BAR_LEFT,
                 x + (BAR_LEFT + STAMINA_BAR_WIDTH), y + BAR_BOTTOM);
    if (target > width) {
        target = 0;
    }
    garden_draw_rect(target + x + BAR_LEFT, y - BAR_TOP, width + x + BAR_LEFT,
                     y + BAR_BOTTOM, DRAIN_DEPTH, DRAIN_COLOR);
    njFogEnable();
}

static void stamina_destroy(struct task *task)
{
}

void race_stamina_create(Uint8 entrant)
{
    /* @hack spelling: task, the entrant's Chao and then the new task, where a
     * test of race_entrant would do */
    struct task *task = race_entrant(entrant);
    if (task) {
        task = task_create(TASK_WORK, 2, stamina_step,
                           "ALR_ADV_StaminaManagerExecutor");
        task->draw = stamina_draw;
        task->destructor = stamina_destroy;
        task->work->tick = entrant;
    }
}

void race_multiplayer_stamina_create(Uint8 entrant)
{
    /* @hack spelling: task, the entrant's Chao and then the new task, where a
     * test of race_entrant would do */
    struct task *task = race_entrant(entrant);
    if (task) {
        task = task_create(TASK_WORK, 2, stamina_step,
                           "ALR_ADV_StaminaManagerExecutor");
        task->draw = multiplayer_stamina_draw;
        task->destructor = stamina_destroy;
        task->work->tick = entrant;
    }
}
