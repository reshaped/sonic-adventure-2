/* the course map in a corner of the screen, with a marker for each racer */
#include <ninja.h>
#include <shinobi.h>
#include "chao/race.h"
#include "chao/race_record.h"
#include "chaostgrace/banner_draw.h"
#include "chaostgrace/hud_art.h"
#include "chaostgrace/map.h"
#include "chaostgrace/race.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/task.h"

/* each racer's marker, from the map's corner */
static NJS_POINT2 map_positions[RACE_ENTRANTS];
/* entrant n's marker is HUD cell MAP_MARKERS + n. the entrant the map's
 * work->tick names, always entrant 0, has MAP_SELECTED_MARKERS + n instead */
enum { MAP_SELECTED_MARKERS = 53, MAP_MARKERS = 61 };
/* the map's task turns its ang.x by TURN_STEP a frame, for the selected
 * marker's njSin */
#define TURN_STEP (FULL_TURN / 32)
/* the map's corner on the screen. it stands at MAP_X in a race of one player,
 * and at MULTI_MAP_X in a multiplayer race */
#define MAP_X 20.0F
#define MULTI_MAP_X 240.0F
#define MAP_Y 30.0F
/* the depths of the track's map, the overview behind it and the markers. the
 * overview draws at OVERVIEW_TINT. each entrant's marker draws
 * MARKER_DEPTH_STEP further back than the last */
#define TRACK_DEPTH (-1.4F)
#define OVERVIEW_DEPTH (-1.5F)
#define OVERVIEW_TINT 0.5F
#define SELECTED_DEPTH (-1.1F)
#define MARKER_DEPTH (-1.3F)
#define MARKER_DEPTH_STEP 0.01F

static void map_child_step(struct task *task)
{
}

static void map_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_SPRITE sprite;
    int entrant;

    if (race_mode() == RACE_RUNNING || race_mode() == RACE_READY) {
        njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
        sprite.tlist = race_map_texlist;
        sprite.tanim = race_map_cells;
        sprite.ang = 0;
        sprite.sx = 1.0F;
        sprite.sy = 1.0F;
        sprite.p.x = work->pos.x;
        sprite.p.y = work->pos.y;
        njDrawSprite2D(&sprite, RACE_MAP_TRACK, TRACK_DEPTH, NJD_SPRITE_ALPHA);
        constant_material_set(OVERVIEW_TINT, OVERVIEW_TINT, OVERVIEW_TINT,
                              OVERVIEW_TINT);
        njDrawSprite2D(&sprite, RACE_MAP_OVERVIEW, OVERVIEW_DEPTH,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
        constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
        sprite.tlist = &race_hud_texlist;
        sprite.tanim = RACE_HUD_CELLS;
        for (entrant = 0; entrant < RACE_ENTRANTS; entrant++) {
            if (race_entrant(entrant)) {
                sprite.p.x = work->pos.x + map_positions[entrant].x;
                sprite.p.y = work->pos.y + map_positions[entrant].y;
                if (work->tick == entrant) {
                    /* @hack redundant: njSin of ang.x, whose result nothing
                     * reads */
                    njSin(work->ang.x);
                    sprite.sx = 1.0F;
                    sprite.sy = 1.0F;
                    njDrawSprite2D(&sprite, entrant + MAP_SELECTED_MARKERS,
                                   SELECTED_DEPTH, NJD_SPRITE_ALPHA);
                } else {
                    sprite.sx = 1.0F;
                    sprite.sy = 1.0F;
                    njDrawSprite2D(&sprite, entrant + MAP_MARKERS,
                                   MARKER_DEPTH - (entrant * MARKER_DEPTH_STEP),
                                   NJD_SPRITE_ALPHA);
                }
            }
        }
        njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    }
}

static void map_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *racer;
    struct task_work *racer_work;
    int entrant;

    work->ang.x += TURN_STEP;
    for (entrant = 0; entrant < RACE_ENTRANTS; entrant++) {
        racer = race_entrant(entrant);
        if (racer) {
            racer_work = racer->work;
            map_positions[entrant].x =
                ((racer_work->pos.x - race_map_origin_x) * race_map_scale) +
                race_map_offset_x;
            map_positions[entrant].y =
                ((racer_work->pos.z - race_map_origin_z) * race_map_scale) +
                race_map_offset_y;
        }
    }
}

static void map_destroy(struct task *task)
{
    task_destroy_children(task);
    if (race_map_texlist) {
        njReleaseTexture(race_map_texlist);
        if (race_map_texlist->textures) {
            syFree(race_map_texlist->textures);
        }
        syFree(race_map_texlist);
        race_map_texlist = NULL;
    }
}

void race_map_load(void)
{
    if (!race_map_texlist) {
        race_map_texlist = syCalloc(1, sizeof(NJS_TEXLIST));
        race_map_texlist->nbTexture = RACE_MAP_TEXTURES;
        race_map_texlist->textures =
            syCalloc(1, RACE_MAP_TEXTURES * sizeof(NJS_TEXNAME));
        njSetTextureName(&race_map_texlist->textures[RACE_MAP_TRACK],
                         race_map_tracks[garden_race_course()->track],
                         RACE_MAP_TRACK, 0);
        njSetTextureName(&race_map_texlist->textures[RACE_MAP_OVERVIEW],
                         race_map_overview[0], RACE_MAP_OVERVIEW, 0);
        njLoadTexture(race_map_texlist);
    }
}

void race_map_create(void)
{
    struct task *task;
    struct task *child;
    struct task_work *work;

    if (race_map_texlist) {
        task =
            task_create(TASK_WORK, 2, map_step, "ALR_ADV_MapManagerExecutor");
        task->destructor = map_destroy;
        /* @hack spelling: the child made and set in each branch, where one
         * made before a choice of x would do */
        if (garden_race_group() != RACE_MULTI) {
            child = task_create_child(TASK_WORK, map_child_step, task);
            child->draw = map_draw;
            work = child->work;
            work->pos.x = MAP_X;
            work->pos.y = MAP_Y;
            work->tick = 0;
        } else {
            child = task_create_child(TASK_WORK, map_child_step, task);
            child->draw = map_draw;
            work = child->work;
            work->pos.x = MULTI_MAP_X;
            work->pos.y = MAP_Y;
            work->tick = 0;
        }
    }
}
