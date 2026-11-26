/* the place displays: the player's, or in a multiplayer race each entrant's
 * at its race_hud_positions entry. a place's cell is the place less 1 */
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/camera.h"
#include "chao/race.h"
#include "chao/race_record.h"
#include "chaostgrace/banner_draw.h"
#include "chaostgrace/race.h"
#include "system/task.h"

/* two copies of a pair of screen points */
static NJS_POINT2 screen_pairs[2][2] = {
#include "chaostgrace/rank/screen_pairs.inc"
};

NJS_POINT2 race_hud_positions[RACE_ENTRANTS] = {
#include "chaostgrace/rank/race_hud_positions.inc"
};

enum { RANK_LABEL = 8 };
/* the screen points of the player's place and of the label beside it */
#define PLACE_X 270.0F
#define PLACE_Y 60.0F
#define LABEL_X 294.0F
#define LABEL_Y 67.0F
/* the scale of each entrant's place in a multiplayer race */
#define ENTRANT_SCALE 0.7F
#define RANK_DEPTH (-0.1F)

static void rank_step(struct task *task)
{
}

static void rank_draw(struct task *task)
{
    struct task_work *work = task->work;
    int rank = race_shown_place(work->tick);
    NJS_SPRITE sprite;
    Float x;
    Float y;
    int cell;
    Bool show_label;

    if (camera_current_index == 0 && race_mode() == RACE_RUNNING) {
        sprite.p.z = 0.0F;
        sprite.ang = 0;
        sprite.sx = 1.0F;
        sprite.sy = 1.0F;
        BANNER_MATERIAL(sprite, 1.0F);
        x = PLACE_X;
        y = PLACE_Y;
        sprite.p.x = x;
        sprite.p.y = y;
        cell = rank - 1;
        BANNER_DRAW_CELL(sprite, cell, RANK_DEPTH);

        show_label = TRUE;
        switch (garden_race_group()) {
        case RACE_CHALLENGE:
            switch (garden_race_level()) {
            case RACE_CHALLENGE_LEVELS - 2:
            case RACE_CHALLENGE_LEVELS - 1:
                show_label = FALSE;
                break;
            }
            break;
        /* @hack spelling: a case each for the dark and hero courses, where
         * one case for both would do */
        case RACE_DARK_COURSES:
            if (garden_race_level() == RACE_ALIGNMENT_LEVELS - 1) {
                show_label = FALSE;
            }
            break;
        case RACE_HERO_COURSES:
            if (garden_race_level() == RACE_ALIGNMENT_LEVELS - 1) {
                show_label = FALSE;
            }
            break;
        }
        if (show_label) {
            sprite.p.x = LABEL_X;
            sprite.p.y = LABEL_Y;
            BANNER_DRAW_CELL(sprite, RANK_LABEL, RANK_DEPTH);
        }
        njFogEnable();
    }
}

static void multiplayer_rank_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: entrant volatile, where a plain Uint8 would do */
    volatile Uint8 entrant = work->tick;
    int rank = race_shown_place(entrant);
    NJS_SPRITE sprite;
    /* @hack redundant: x declared register, where a plain Float would do */
    register Float x;
    Float y;
    NJS_POINT2 *position;
    int cell;

    if (race_mode() == RACE_RUNNING || race_mode() == RACE_READY) {
        sprite.p.z = 0.0F;
        sprite.ang = 0;
        BANNER_MATERIAL(sprite, 1.0F);
        position = &race_hud_positions[entrant];
        x = position->x;
        y = position->y;
        sprite.p.x = x;
        sprite.p.y = y;
        sprite.sx = ENTRANT_SCALE;
        sprite.sy = ENTRANT_SCALE;
        cell = rank - 1;
        BANNER_DRAW_CELL(sprite, cell, RANK_DEPTH);
        njFogEnable();
    }
}

static void rank_destroy(struct task *task)
{
}

void race_rank_create(Uint8 entrant)
{
    struct task *task =
        task_create(TASK_WORK, 2, rank_step, "ALR_ADV_RankManagerExecutor");
    task->draw = rank_draw;
    task->destructor = rank_destroy;
    task->work->tick = entrant;
}

void race_multiplayer_rank_create(Uint8 entrant)
{
    struct task *task =
        task_create(TASK_WORK, 2, rank_step, "ALR_ADV_RankManagerExecutor");
    task->draw = multiplayer_rank_draw;
    task->destructor = rank_destroy;
    task->work->tick = entrant;
}
