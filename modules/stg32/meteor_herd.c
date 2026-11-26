/* Meteor Herd, stg32: the stage's init, exit and step, and its background. the
 * step makes parts of the land solid by player 0's height, and sends meteors
 * and fireballs down. the background tints the sky and turns its orbits */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/blast.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dirt.h"
#include "shared/emerald.h"
#include "shared/enemy_bomb.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/land_draw.h"
#include "shared/land_model_change.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/space_sky_textures.h"
#include "shared/spin_crates.h"
#include "shared/vertex_colors.h"
#include "shared/water_ripple.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "shared/emerald_hunt_data.h"
#include "stg32/routes.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg32/meteor_herd.h"
#include "stg32/scenery_shapes.h"
#include "stg32/sounds.h"
#include "stg32/object_archives.h"
#include "stg32/meteors.h"
#include "stg32/sky.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* the set file's block is at least SET_ENTRIES entries long, its head
 * counted as one */
#define SET_ENTRIES 2048
/* the task levels of the background's task and of the meteors' */
#define BACKGROUND_LEVEL 1
#define METEOR_LEVEL 2
/* in a one-player game, the step makes land entries solid by player 0's
 * height. it changes only the entries whose size class isn't 0. with player 0
 * above collision_altitude, the entries whose center lies above it are solid.
 * below it, the entries at or below it are, but inside the tower none is. the
 * tower lies within TOWER_HALF_WIDTH of x = 0 and z = 0 */
#define TOWER_HALF_WIDTH 100.0F
/* the chance in a step that a burst flares about player 0 while
 * kill_areas_task has a child. it has one while a player falls out of the
 * stage */
#define BURST_CHANCE 0.5F
/* the chance in a step that a meteor starts to fall. none starts while
 * MAX_FALLING_METEORS fall */
#define METEOR_CHANCE 0.1F
#define MAX_FALLING_METEORS 4
/* the chances in a step of a fireball at a surface and at a point of the
 * land. none flies at a surface while MAX_FIREBALLS fly */
#define SURFACE_FIREBALL_CHANCE 0.1F
#define POINT_FIREBALL_CHANCE 0.003F
#define MAX_FIREBALLS 3
/* a fireball flies from FIREBALL_HEIGHT above its target, FIREBALL_RADIUS out
 * from the y axis on the target's side. it stops FIREBALL_STANDOFF short of
 * the target */
#define FIREBALL_RADIUS 1300.0F
#define FIREBALL_HEIGHT 1000.0F
#define FIREBALL_STANDOFF 20.0F
/* the probe for a surface lies at random in a box about player 0. the box is
 * PROBE_SPREAD wide across x and z, and PROBE_HEIGHT tall. its center lies
 * PROBE_LIFT above player 0. the side the probe takes is a random whole number
 * below SIDE_PICK_RANGE, modulo SURFACE_SIDES */
#define PROBE_SPREAD 500.0F
#define PROBE_HEIGHT 100.0F
#define PROBE_LIFT 10.0F
#define SIDE_PICK_RANGE 55.0F
/* a fireball at a point of the land aims at a point of one entry's model. the
 * point is a random whole number below POINT_PICK_SCALE * nbPoint, modulo
 * nbPoint */
#define POINT_PICK_SCALE 3.0F
/* a target within AXIS_RADIUS of the y axis has no side to fly from. its
 * fireball flies from a random point instead, up to FIREBALL_RADIUS either way
 * along x and along z */
#define AXIS_RADIUS 1.0F
/* the first and second orbits lean ORBIT_TILT about x */
#define ORBIT_TILT 0x3000
/* the warning land shows its WARNING_FRAMES textures in turn, each for
 * 1 << WARNING_FRAME_SHIFT steps */
#define WARNING_FRAMES 13
#define WARNING_FRAME_SHIFT 1
/* a random Float from -1 up to 1 */
#define RANDOM_SIGNED() (((Float)rand() * (2.0F * RAND_SCALE)) - 1.0F)

static struct kill_area meteor_herd_kill_areas[2] = {
#include "stg32/meteor_herd/meteor_herd_kill_areas.inc"
};
static struct light main_light = {
    {0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F};
static struct light second_light = {
    {0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F};
static struct set_head *stage_set = NULL;
static struct task *background_task = NULL;
static Float burst_size = 3.0F;
static Float burst_size_range = 3.5F;
/* a falling meteor starts meteor_height above camera 0. it starts from
 * meteor_min_radius to meteor_min_radius + meteor_radius_range out across x
 * and z. each step it moves meteor_fall_speed along y, and up to
 * meteor_lateral_speed either way along x and along z. it lives meteor_travel /
 * |meteor_fall_speed| steps */
static Float meteor_fall_speed = -50.0F;
static Float meteor_lateral_speed = 40.0F;
static Float meteor_min_radius = 3000.0F;
static Float meteor_radius_range = 500.0F;
static Float meteor_height = 2500.0F;
static Float meteor_travel = 5000.0F;
static Float dat_8c55c654 = 0.05F;
static Float dat_8c55c658 = 10.0F;
/* the height that splits the land entries the step makes solid */
static Float collision_altitude = 200.0F;

/* the sky surfaces' uvs lists, laid out as draw/texture_anim.h says. each list
 * holds one key, which texture_anim_scroll scrolls. each scroll steps v by 1 a
 * frame and u by 0, and its frame lasts 1 */
static Sint16 first_uvs[60] = {
#include "stg32/meteor_herd/first_uvs.inc"
};
static struct texture_anim first_scroll = {
#include "stg32/meteor_herd/first_scroll.inc"
};
static Sint16 second_uvs[46] = {
#include "stg32/meteor_herd/second_uvs.inc"
};
static struct texture_anim second_scroll = {
#include "stg32/meteor_herd/second_scroll.inc"
};
static Sint16 third_uvs[58] = {
#include "stg32/meteor_herd/third_uvs.inc"
};
static struct texture_anim third_scroll = {
#include "stg32/meteor_herd/third_scroll.inc"
};
static Sint16 fourth_uvs[82] = {
#include "stg32/meteor_herd/fourth_uvs.inc"
};
static struct texture_anim fourth_scroll = {
#include "stg32/meteor_herd/fourth_scroll.inc"
};
static Sint16 fifth_uvs[102] = {
#include "stg32/meteor_herd/fifth_uvs.inc"
};
static struct texture_anim fifth_scroll = {
#include "stg32/meteor_herd/fifth_scroll.inc"
};

/* a surface of the sky. the background's step gives a tinted surface's
 * vertices the pulse's color, and scrolls any other by its speeds. every
 * surface is tinted, so none scrolls */
struct sky_surface {
    Bool tinted;
    NJS_CNK_OBJECT *object;
    struct texture_anim *anim;
    Sint32 v_speed;
    Sint32 u_speed;
};
static struct sky_surface surfaces[5] = {
#include "stg32/meteor_herd/surfaces.inc"
};
static Uint32 dim_color = 0xff101010;
static Uint32 bright_color = 0xffffffff;
static Angle pulse_rate = 0x200;
static Uint32 scroll_rate = 35;
/* the two orbits, then the meteor, turn this far about x and about y a step */
static Angle first_orbit_x = 1;
static Angle first_orbit_y = 2;
static Angle second_orbit_x = 2;
static Angle second_orbit_y = 4;
static Angle third_orbit_x = 0;
static Angle third_orbit_y = 2;
static Float dat_8c55ca58 = 1500.0F;
static Float dat_8c55ca5c = 1.6F;
static Float stars_height = 0.0F;
static Float stars_scale = 1.0F;
static Float sky_height = 3000.0F;
static Float meteor_orbit_height = -3700.0F;
static Float orbit_scale = 1.0F;
static Float meteor_scale = 1.0F;
/* whether background_build compiled every model of background_builds */
static Bool background_built = FALSE;
enum {
    BUILD_STARS,
    BUILD_FIRST_ORBIT,
    BUILD_SECOND_ORBIT,
    BUILD_METEOR,
    BACKGROUND_BUILDS
};
static struct model_build background_builds[BACKGROUND_BUILDS] = {
#include "stg32/meteor_herd/background_builds.inc"
};

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static Uint8 *background_build(Uint8 *buffer);
static void background_draw(struct task *task);

void meteor_herd_init(void)
{
    struct task *task;
    Uint8 *buffer;
    struct light *light;
    void (*set_light)(int index, const struct light *light);

    world_land_start(&meteor_herd_land);
    texture_load_shared(meteor_herd_pvms, meteor_herd_texture_groups);
    texture_await_shared(meteor_herd_pvms, meteor_herd_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    stage_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&meteor_herd_objects, stage_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    /* @hack spelling: light_set called through set_light, and main_light
     * passed through light, where calls with &main_light would do */
    set_light = light_set;
    light = &main_light;
    set_light(0, light);
    set_light(1, &second_light);
    set_light(2, light);
    set_light(3, light);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg32_light.bin");
    buffer = object_builds_run((Uint8 *)world_build_land(
        &meteor_herd_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    buffer = background_build(buffer);
    buffer = meteor_models_build(buffer);
    buffer = meteor_tank_build(buffer);
    buffer = spin_wood_build(buffer);
    buffer = spin_solid_build(buffer);
    buffer = spin_iron_build(buffer);
    buffer = meteor_platform_build(buffer);
    music_fade_in(0);
    music_play_or_defer("m_herd.adx");
    path_create_manager(meteor_herd_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
        meteor_footprints_create(0, task);
        if (player_work[1] != NULL) {
            meteor_footprints_create(1, task);
        }
    }
    kill_areas_start(meteor_herd_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.unk_04 == NULL) {
        player_hooks.unk_04 = dirt_create;
    }
    if (player_hooks.create_piece == NULL) {
        player_hooks.create_piece = emerald_create;
    }
    if (player_hooks.show_piece_sign == NULL) {
        player_hooks.show_piece_sign = emerald_sign_create;
    }
    if (player_hooks.ripple == NULL) {
        player_hooks.ripple = ripple_create;
    }
    hunt_set_hints(hunt_location_hints);
    hunt_stage_messages = hunt_load_hint_file("eh0032");
    emerald_set_paths(hunt_piece_routes, HUNT_PIECE_ROUTES);
    njFogEnable();
    messenger_messages_load("mh0032");
    hint_box_hints_load("bh0032");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage("se_ac_meh.mlt", &meteor_herd_sounds);
    object_builds_init();
    land_model_change_create(&meteor_herd_changing_land);
    blast_load_texture();
    enemy_bomb_load();
    debris_friction = 0.99F;
    debris_gravity = 0.03F;
    debris_life = 90;
    debris_life_spread = 30.0F;
}

void meteor_herd_exit(void)
{
    object_builds_exit();
    fog_stop();
    njFogDisable();
    emerald_clear_paths();
    syFree(hunt_stage_messages);
    hunt_stage_messages = NULL;
    if (player_hooks.ripple == ripple_create) {
        player_hooks.ripple = NULL;
    }
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.unk_04 == dirt_create) {
        player_hooks.unk_04 = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    if (player_hooks.create_piece == emerald_create) {
        player_hooks.create_piece = NULL;
    }
    if (player_hooks.show_piece_sign == emerald_sign_create) {
        player_hooks.show_piece_sign = NULL;
    }
    hint_box_hints_free();
    messenger_messages_free();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(meteor_herd_pvms, meteor_herd_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (stage_set != NULL) {
        loader_free(stage_set);
        stage_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

/* @hack spelling: a fireball's sums as inline walks of three Floats, where
 * three statements on the fields would do. fireball_scale and fireball_add
 * step their pointers past the first Float in statements of their own */
#pragma inline(fireball_delta, fireball_scale, fireball_add)
static void fireball_delta(const Float *origin, const Float *point,
                           Float *delta)
{
    *delta++ = *point++ - *origin++;
    *delta++ = *point++ - *origin++;
    *delta = *point - *origin;
}
static void fireball_scale(Float *coord, Float scale)
{
    *coord *= scale;
    coord = coord + 1;
    *coord++ *= scale;
    *coord *= scale;
}
static void fireball_add(const Float *origin, const Float *offset,
                         Float *result)
{
    *result = *offset + *origin;
    result++;
    offset++;
    origin = origin + 1;
    *result++ = *offset++ + *origin++;
    *result = *offset + *origin;
}

void meteor_herd_step(struct task *task)
{
    struct task_work *work;
    Sint32 block;
    struct land_entry *entry;
    struct task *child;
    NJS_OBJECT *object;
    NJS_POINT3 launch;
    NJS_POINT3 impact;
    Float length;
    Float scale;
    Uint32 flags;
    int count;
    int index;

    work = task->work;
    /* @hack redundant: block, the land blocks of the current camera, read
     * where nothing uses them */
    if (camera_current_index == 0) {
        block = game_state.land_blocks[0];
    } else {
        block = game_state.land_blocks[1];
    }
    (void)block;
    count = meteor_herd_land.count;
    entry = meteor_herd_land.entries;
    njPushMatrixEx();
    if (player_work[1] == NULL && player_work[0] != NULL) {
        if (player_work[0]->pos.y > collision_altitude) {
            while (count--) {
                /* @hack redundant: a block no path reaches, which stores count
                 * in collision_altitude */
                if (0) {
                    collision_altitude = count;
                }
                flags = entry->flags;
                if (flags & WORLD_SIZE_CLASS_BITS) {
                    if (entry->center.y <= collision_altitude) {
                        flags &= ~WORLD_SOLID;
                    } else {
                        flags |= WORLD_SOLID;
                    }
                    entry->flags = flags;
                }
                entry++;
            }
        } else if (njAbs(player_work[0]->pos.x) < TOWER_HALF_WIDTH &&
                   njAbs(player_work[0]->pos.z) < TOWER_HALF_WIDTH) {
            while (count--) {
                flags = entry->flags;
                if (flags & WORLD_SIZE_CLASS_BITS) {
                    /* @hack spelling: flags = flags & ~WORLD_SOLID, where
                     * flags &= ~WORLD_SOLID would do */
                    flags = flags & ~WORLD_SOLID;
                    entry->flags = flags;
                }
                entry++;
            }
        } else {
            while (count--) {
                flags = entry->flags;
                if (flags & WORLD_SIZE_CLASS_BITS) {
                    if (entry->center.y > collision_altitude) {
                        flags &= ~WORLD_SOLID;
                    } else {
                        flags |= WORLD_SOLID;
                    }
                    entry->flags = flags;
                }
                entry++;
            }
        }
    }
    njPopMatrixEx();
    if (player_work[0] != NULL && kill_areas_task != NULL &&
        kill_areas_task->child != NULL && RANDOM_UNIT() > BURST_CHANCE) {
        burst_create_wobbling(&player_work[0]->pos, &player_motion[0]->spd,
                              ((Float)rand() * burst_size_range * RAND_SCALE) +
                                  burst_size);
    }
    if (camera_views[0] != NULL && RANDOM_UNIT() < METEOR_CHANCE &&
        falling_meteor_count < MAX_FALLING_METEORS) {
        launch.x = ((Float)rand() * meteor_radius_range * RAND_SCALE) +
                   meteor_min_radius;
        launch.y = meteor_height;
        launch.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&camera_views[0]->pos);
        njRotateY(NULL, RANDOM_TWO_TURNS());
        njCalcPoint(NULL, &launch, &launch);
        njPopMatrixEx();
        child = task_create(TASK_WORK, METEOR_LEVEL, meteor_fall_init,
                            "ObjectMeteoFall");
        if (child != NULL) {
            child->work->pos = launch;
            child->work->scl.y = meteor_fall_speed;
            child->work->scl.z = meteor_lateral_speed * RANDOM_SIGNED();
            child->work->scl.x = meteor_lateral_speed * RANDOM_SIGNED();
            METEOR_LIFE(child->work) = njAbs(meteor_travel / meteor_fall_speed);
        }
    }
    if (player_work[1] == NULL && player_work[0] != NULL &&
        fireball_count < MAX_FIREBALLS &&
        RANDOM_UNIT() < SURFACE_FIREBALL_CHANCE) {
        struct surface_probe probe;
        NJS_VECTOR direction;
        int side;

        impact.y = player_work[0]->pos.y +
                   ((RANDOM_UNIT() - 0.5F) * PROBE_HEIGHT) + PROBE_LIFT;
        impact.x =
            player_work[0]->pos.x + ((RANDOM_UNIT() - 0.5F) * PROBE_SPREAD);
        impact.z =
            player_work[0]->pos.z + ((RANDOM_UNIT() - 0.5F) * PROBE_SPREAD);
        /* @hack redundant: a block no path reaches, which sets launch */
        if (0) {
            launch.x = PROBE_SPREAD;
            launch.z = 1.0F;
            launch.y = 1.0F;
        }
        probe.pos = impact;
        surface_probe_sides(&probe);
        side = (int)((Float)rand() * (SIDE_PICK_RANGE * RAND_SCALE)) %
               SURFACE_SIDES;
        if (probe.sides[side].found && probe.sides[side].normal.y >= 0.0F) {
            /* @hack redundant: a block no path reaches, which tests side */
            if (0) {
                if (side) {
                }
            }
            switch (side) {
            case SURFACE_POS_X:
            case SURFACE_NEG_X:
                impact.x = probe.sides[side].pos;
                break;
            case SURFACE_POS_Y:
            case SURFACE_NEG_Y:
            default:
                impact.y = probe.sides[side].pos;
                break;
            case SURFACE_POS_Z:
            case SURFACE_NEG_Z:
                impact.z = probe.sides[side].pos;
                break;
            }
            length = njSqrt((impact.x * impact.x) + (impact.z * impact.z));
            if (length > AXIS_RADIUS) {
                scale = FIREBALL_RADIUS / length;
                launch.x = impact.x * scale;
                launch.z = impact.z * scale;
                launch.y = impact.y + FIREBALL_HEIGHT;
            } else {
                launch.x = (RANDOM_UNIT() - 0.5F) * (2.0F * FIREBALL_RADIUS);
                launch.z = (RANDOM_UNIT() - 0.5F) * (2.0F * FIREBALL_RADIUS);
                launch.y = impact.y + FIREBALL_HEIGHT;
            }
            fireball_delta(&launch.x, &impact.x, &direction.x);
            length = njScalor(&direction);
            fireball_scale(&direction.x, (length - FIREBALL_STANDOFF) / length);
            fireball_add(&launch.x, &direction.x, &impact.x);
            child = task_create(TASK_WORK, METEOR_LEVEL, meteor_fireball_init,
                                "ObjectFireBall");
            if (child != NULL) {
                child->work->pos = launch;
                child->work->scl = impact;
                if (!(probe.sides[side].flags & WORLD_DUSTY)) {
                    child->work->submode = FIREBALL_SHATTERS;
                }
            }
        }
    }
    if (player_work[0] != NULL && player_work[1] == NULL &&
        RANDOM_UNIT() < POINT_FIREBALL_CHANCE) {
        count = meteor_herd_land.count;
        /* @hack spelling: count less drawn as a statement of its own, where
         * count = meteor_herd_land.count - meteor_herd_land.drawn would do */
        count = count - meteor_herd_land.drawn;
        index = game_state.steps % count;
        entry = &meteor_herd_land.entries[meteor_herd_land.drawn];
        entry += index;
        if (entry->flags & WORLD_SOLID) {
            /* @hack redundant: unused, an array of six words that nothing
             * reads */
            Uint32 unused[6];
            NJS_VECTOR direction;

            object = entry->object;
            index = (int)((Float)rand() * (Float)object->model->nbPoint *
                          (POINT_PICK_SCALE * RAND_SCALE)) %
                    object->model->nbPoint;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, 0);
            njCalcPoint(NULL, &object->model->points[index], &impact);
            njPopMatrixEx();

            length = njSqrt((impact.x * impact.x) + (impact.z * impact.z));
            if (length > AXIS_RADIUS) {
                scale = FIREBALL_RADIUS / length;
                launch.x = impact.x * scale;
                launch.z = impact.z * scale;
                launch.y = impact.y + FIREBALL_HEIGHT;
            } else {
                launch.x = (RANDOM_UNIT() - 0.5F) * (2.0F * FIREBALL_RADIUS);
                launch.z = (RANDOM_UNIT() - 0.5F) * (2.0F * FIREBALL_RADIUS);
                launch.y = impact.y + FIREBALL_HEIGHT;
            }
            fireball_delta(&launch.x, &impact.x, &direction.x);
            length = njScalor(&direction);
            fireball_scale(&direction.x, (length - FIREBALL_STANDOFF) / length);
            fireball_add(&launch.x, &direction.x, &impact.x);
            child = task_create(TASK_WORK, METEOR_LEVEL, meteor_fireball_init,
                                "ObjectFireBall");
            if (child != NULL) {
                child->work->pos = launch;
                child->work->scl = impact;
                if (!(entry->flags & WORLD_DUSTY)) {
                    child->work->submode = FIREBALL_SHATTERS;
                }
            }
        }
    }
    /* @hack redundant: a switch of one empty case, where nothing would do */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
    struct task_work *work;
    struct sky_surface *surface;
    Uint32 frame;
    Uint32 color;
    int step;
    unsigned int i;

    /* @hack redundant: a read of task->work that the step never uses */
    work = task->work;
    (void)work;
    /* @hack spelling: the assignment to frame inside the expression, where a
     * statement of its own would do */
    step = (Uint16)((frame = game_state.steps) * scroll_rate);
    surface = surfaces;
    color = color_blend(dim_color, bright_color,
                        (njSin((Angle)frame * pulse_rate) * 0.5F) + 0.5F);
    for (i = 0; i < sizeof(surfaces) / sizeof(surfaces[0]); i++, surface++) {
        if (!surface->tinted) {
            texture_anim_scroll(surface->object->model, surface->anim,
                                (surface->u_speed * step) & (SCROLL_WRAP - 1),
                                (surface->v_speed * step) & (SCROLL_WRAP - 1));
        } else {
            vlist_set_color(surface->object->model->vlist, color);
        }
    }
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

static Uint8 *background_build(Uint8 *buffer)
{
    njFogDisable();
    if (model_builds_compile(&buffer, background_builds, BACKGROUND_BUILDS) ==
        BACKGROUND_BUILDS) {
        background_built = TRUE;
    }
    njFogEnable();
    return buffer;
}

static void background_draw(struct task *task)
{
    struct task_work *work;
    NJS_TEXLIST frame_texlist;
    int frame;

    /* @hack redundant: a read of task->work that the draw never uses */
    work = task->work;
    (void)work;
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, sky_height, 0.0F);
    njSetTexture(&background_1_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, stars_height, 0.0F);
    njScale(NULL, stars_scale, 1.0F, stars_scale);
    if (background_built) {
        njTranslateEx((NJS_VECTOR *)meteor_sky_stars->pos);
        njDirectDrawObject(background_builds[BUILD_STARS].built);
    } else {
        njCnkEasyDrawObject(meteor_sky_stars);
    }
    njPopMatrixEx();

    njPushMatrixEx();
    njSetTexture(&background_2_texlist);
    njTranslateEx((NJS_VECTOR *)meteor_sky_first_orbit->pos);
    njTranslate(NULL, meteor_sky_first_orbit->model->center.x,
                meteor_sky_first_orbit->model->center.y,
                meteor_sky_first_orbit->model->center.z);
    njRotateX(NULL, (first_orbit_x * game_state.steps) + ORBIT_TILT);
    njRotateY(NULL, first_orbit_y * game_state.steps);
    njScale(NULL, orbit_scale, orbit_scale, orbit_scale);
    njTranslate(NULL, -meteor_sky_first_orbit->model->center.x,
                -meteor_sky_first_orbit->model->center.y,
                -meteor_sky_first_orbit->model->center.z);
    if (background_built) {
        njDirectDrawObject(background_builds[BUILD_FIRST_ORBIT].built);
    } else {
        njCnkEasyDrawModel(meteor_sky_first_orbit->model);
    }
    matrix_load_parent();

    njSetTexture(&background_3_texlist);
    njTranslateEx((NJS_VECTOR *)meteor_sky_second_orbit->pos);
    njTranslate(NULL, meteor_sky_second_orbit->model->center.x,
                meteor_sky_second_orbit->model->center.y,
                meteor_sky_second_orbit->model->center.z);
    njRotateX(NULL, (second_orbit_x * game_state.steps) + ORBIT_TILT);
    njRotateY(NULL, second_orbit_y * game_state.steps);
    njScale(NULL, orbit_scale, orbit_scale, orbit_scale);
    njTranslate(NULL, -meteor_sky_second_orbit->model->center.x,
                -meteor_sky_second_orbit->model->center.y,
                -meteor_sky_second_orbit->model->center.z);
    if (background_built) {
        njDirectDrawObject(background_builds[BUILD_SECOND_ORBIT].built);
    } else {
        njCnkEasyDrawModel(meteor_sky_second_orbit->model);
    }
    matrix_load_parent();

    njSetTexture(&meteor_sky_meteor_texlist);
    njTranslateEx((NJS_VECTOR *)meteor_sky_meteor->pos);
    njTranslate(NULL, meteor_sky_meteor->model->center.x,
                meteor_sky_meteor->model->center.y + meteor_orbit_height,
                meteor_sky_meteor->model->center.z);
    njRotateX(NULL, third_orbit_x * game_state.steps);
    njRotateY(NULL, third_orbit_y * game_state.steps);
    njScale(NULL, meteor_scale, meteor_scale, meteor_scale);
    njTranslate(NULL, -meteor_sky_meteor->model->center.x,
                -meteor_sky_meteor->model->center.y,
                -meteor_sky_meteor->model->center.z);
    if (background_built) {
        njDirectDrawObject(background_builds[BUILD_METEOR].built);
    } else {
        njCnkEasyDrawModel(meteor_sky_meteor->model);
    }
    matrix_load_parent();
    njPopMatrix(2);
    land_table_draw(&meteor_herd_background_land);

    frame_texlist = meteor_warning_texlist;
    frame = (game_state.steps >> WARNING_FRAME_SHIFT) % WARNING_FRAMES;
    frame_texlist.textures += frame;
    frame_texlist.nbTexture = 1;
    meteor_herd_warning_land.texlist = &frame_texlist;
    land_table_draw(&meteor_herd_warning_land);
    meteor_herd_warning_land.texlist = NULL;
}
