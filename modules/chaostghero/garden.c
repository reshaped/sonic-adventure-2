#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/area.h"
#include "chao/garden_save.h"
#include "chao/garden_scene.h"
#include "chao/messages.h"
#include "chao/race.h"
#include "chao/residents.h"
#include "chao/walk_screen.h"
#include "chaostghero/garden.h"
#include "chaostghero/land.h"
#include "chaostghero/props.h"
#include "collision/world.h"
#include "draw/light.h"
#include "object/set.h"
#ifdef NONMATCHING
/* garden_start_layout, by which this side's step installs the garden's
 * layout */
#include "shared/garden_layout.h"
#endif
#include "shared/garden_setup.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

static struct light hero_lights[2] = GARDEN_LIGHTS;
static struct set_head *hero_layout = NULL;
static struct garden_water hero_water[8] = {
#include "chaostghero/garden/hero_water.inc"
};

static const NJS_POINT3 held_sound_position = {93.0F, 13.0F, -90.0F};
#define HELD_SOUND_PRIORITY 1
#define HELD_SOUND_VOLUME 10
#define RANDOM_POINT_PRIORITY 1
#define RANDOM_POINT_VOLUME 0

/* the horizontal square in which RANDOM_POINT_SOUND picks its point: its
 * center and the length of its side */
#define SQUARE_X 0.0F
#define SQUARE_Y 0.0F
#define SQUARE_Z 0.0F
#define SQUARE_SIDE 150.0F
/* the duration RANDOM_POINT_SOUND gives sound_play_at_for: LEAST_DURATION
 * updates and a random part of DURATION_SPREAD more */
#define LEAST_DURATION 120.0F
#define DURATION_SPREAD 300.0F

/* play id for task, under GARDEN_AMBIENT_SOUND's test, at a random point of
 * the square. the macro writes the point to position */
/* @hack spelling: the random-point sound's test, point and play as a macro,
 * where a function would do */
#define RANDOM_POINT_SOUND(frame, interval, chance, id, task, position)        \
    do {                                                                       \
        if ((frame) % (interval) == 0 && (rand() * RAND_SCALE) < (chance)) {   \
            /* @hack redundant: adding SQUARE_X and SQUARE_Z, which are 0 */   \
            (position).x =                                                     \
                SQUARE_X + (SQUARE_SIDE * ((rand() * RAND_SCALE) - 0.5F));     \
            (position).y = SQUARE_Y;                                           \
            (position).z =                                                     \
                SQUARE_Z + (SQUARE_SIDE * ((rand() * RAND_SCALE) - 0.5F));     \
            sound_play_at_for(                                                 \
                (id), (task), RANDOM_POINT_PRIORITY, RANDOM_POINT_VOLUME,      \
                LEAST_DURATION + (rand() * (DURATION_SPREAD * RAND_SCALE)),    \
                &(position));                                                  \
        }                                                                      \
    } while (0)

static void hero_ambient(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 position;

    /* @hack redundant: a switch that raises work->mode from 0 to 1. nothing
     * reads the mode */
    switch (work->mode) {
    /* @hack spelling: the switch's one case ends without a break, where a
     * break would do */
    case 0:
        work->mode++;
    }
    position = held_sound_position;
    sound_play_held_at(2, task, HELD_SOUND_PRIORITY, HELD_SOUND_VOLUME,
                       &position);
    RANDOM_POINT_SOUND(work->timer, 900, 0.4F, 3, task, position);
    RANDOM_POINT_SOUND(work->timer, 60, 0.1F, 3, task, position);
    GARDEN_AMBIENT_SOUND(work->timer, 120, 0.1F, 5);
    GARDEN_AMBIENT_SOUND(work->timer, 150, 0.1F, 6);
    GARDEN_AMBIENT_SOUND(work->timer, 100, 0.1F, 7);
    GARDEN_AMBIENT_SOUND(work->timer, 180, 0.1F, 8);
    GARDEN_AMBIENT_SOUND(work->timer, 120, 0.03F, 4);
    work->timer++;
}

static void hero_garden_close(struct task *task)
{
    GARDEN_RELEASE_RESOURCES(hero_layout, &hero_garden_texlist);
    world_land_stop();
}

void chaostghero_step(struct task *task)
{
#ifdef NONMATCHING
    /* this side installs the layout before the land by garden_start_layout. it
     * tries again the next step where it can't, as garden_layout.h says */
    hero_layout = garden_start_layout("set_chao_hero.bin", &chaostghero_objects,
                                      &hero_land);
    if (hero_layout == NULL) {
        return;
    }
#else
    world_land_start(&hero_land);
    hero_layout = garden_file_load("set_chao_hero.bin");
    set_start(&chaostghero_objects, hero_layout);
#endif
    GARDEN_LOAD_TEXTURES("AL_HERO_OBJ_TEX", &hero_garden_texlist);
    GARDEN_SET_LIGHTING(hero_lights);
    world_build_land(&hero_land, (Uint32)CHAO_LAND_BUFFER(hero_land_buffer));
    GARDEN_START_VIEW("chao_g_bgm_h.adx");
    garden_control_start();
    garden_animals_start();
    task->step = hero_ambient;
    task->draw = NULL;
    task->destructor = hero_garden_close;
    walk_load_textures();
    garden_load_residents();
    garden_load_trees();
    garden_restore_carried();
    {
        NJS_POINT3 position = {100.0F, 0.0F, 85.0F};
        garden_transfer_machine_create(&position, 3 * EIGHTH_TURN);
    }
    if (garden_has_award(RACE_AWARD_HERO_BALL)) {
        NJS_POINT3 position = {60.0F, 0.0F, 40.0F};
        NJS_VECTOR velocity = {0.0F, 0.0F, 0.0F};
        garden_ball_create(&position, &velocity);
    }
    if (garden_has_award(RACE_AWARD_ROCKING_HORSE)) {
        NJS_POINT3 position = {-82.0F, 0.0F, 35.0F};
        garden_rocking_horse_create(&position, 0x3200);
    }
    /* @hack redundant: a test of RACE_AWARD_PIANO whose branch only
     * initializes the local position. nothing reads that local */
    if (garden_has_award(RACE_AWARD_PIANO)) {
        NJS_POINT3 position = {50.0F, 15.0F, -103.0F};
        (void)position;
    }
    garden_messages_load("msgChaoOsanpo");
    garden_water_create(hero_water);
}
