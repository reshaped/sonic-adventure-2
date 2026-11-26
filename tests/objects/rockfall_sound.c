/* Run the Final Rush background's rockfall section with and without camera 0.
 */
#include <assert.h>
#include <math.h>
#include <stdlib.h>

/* Drive frames where the original random threshold admits a fragment sound. */
static int draw_number;

static int sound_roll(void)
{
    return draw_number++ == 0 ? RAND_MAX / 2 : 0;
}

#define rand sound_roll
#include "stg30/final_rush.c"
#undef rand

struct camera_place *camera_views[CAMERAS];
struct game_state game_state;
struct task *kill_areas_task;
struct task *player_tasks[PLAYER_SLOTS];
struct task_work *player_work[PLAYER_SLOTS];
struct player_motion *player_motion[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];
struct scenery_strip scenery_strips[SCENERY_STRIPS];
int rockfall_count = ROCKS_MAX;

static int sounds, rumbles, fog_updates;

Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *pos)
{
    const NJS_POINT3 *camera;

    assert(camera_views[0] != NULL);
    camera = &camera_views[0]->pos;
    assert(id == FIRST_FRAGMENT_SOUND && volume == fragment_sound_volumes[0]);
    assert(pos->x == camera->x - SOUND_SCATTER * 0.5F);
    assert(pos->y == camera->y - SOUND_SCATTER * 0.5F);
    assert(pos->z == camera->z - SOUND_SCATTER * 0.5F);
    sounds++;
    return 0;
}

void vibration_start(int player, int power, int freq, int time)
{
    assert(player == 0 && power == RUMBLE_POWER && time == RUMBLE_TIME);
    assert(freq >= RUMBLE_FREQ && freq < RUMBLE_FREQ + RUMBLE_FREQ_RANGE);
    rumbles++;
}

void fog_set_blend(struct fog_setting *from, struct fog_setting *to, Float mix)
{
    assert(from == base_fog && to == rockfall_fog);
    assert(mix == (fog_updates + 1) * (1.0F / FOG_STEPS));
    fog_updates++;
}

Float njSqrt(Float x)
{
    return (Float)sqrt(x);
}

Float njSin(Angle angle)
{
    return 0.0F;
}

Uint32 color_blend(Uint32 from, Uint32 to, Float mix)
{
    return from;
}

void texture_anim_scroll(NJS_CNK_MODEL *model, struct texture_anim *anim,
                         Sint16 u, Sint16 v)
{
    assert(model != NULL);
}

Sint32 vlist_set_color(Sint32 *vlist, Uint32 color)
{
    abort();
}

/* These service boundaries must stay idle: the player is midway along the
 * route, the shard cap is full, and no meteor roll or miss effect qualifies. */
struct task *task_create(Uint16 flags, int level, void (*step)(struct task *),
                         const char *name)
{
    abort();
}

void meteor_fall_init(struct task *task)
{
    abort();
}

void rockfall_init(struct task *task)
{
    abort();
}

int rockfall_attach(struct task *task)
{
    abort();
}

struct particle *burst_create_wobbling(NJS_POINT3 *pos, NJS_VECTOR *speed,
                                       Float scale)
{
    abort();
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    abort();
}

Bool njPushMatrixEx(void)
{
    abort();
}

Bool njPopMatrixEx(void)
{
    abort();
}

void njUnitMatrix(NJS_MATRIX *matrix)
{
    abort();
}

void njTranslateEx(NJS_VECTOR *position)
{
    abort();
}

void njRotateY(NJS_MATRIX *matrix, Angle angle)
{
    abort();
}

void njCalcPoint(NJS_MATRIX *matrix, NJS_POINT3 *point, NJS_POINT3 *result)
{
    abort();
}

int main(void)
{
    struct task background = {0}, player = {0};
    struct task_work work = {0}, player_data = {0};
    struct player_motion motion = {0};
    struct player record = {0};
    struct camera_place camera = {0};
    NJS_CNK_OBJECT strip = {0};
    NJS_CNK_MODEL model = {0};
    int i;

    strip.model = &model;
    for (i = 0; i < SCENERY_STRIPS; i++) {
        scenery_strips[i].object = &strip;
    }

    background.work = &work;
    player_data.pos.x = (rockfall_start.x + rockfall_end.x) * 0.5F;
    player_data.pos.y = (rockfall_start.y + rockfall_end.y) * 0.5F;
    player_data.pos.z = (rockfall_start.z + rockfall_end.z) * 0.5F;
    player.work = &player_data;
    player.motion.p = &motion;
    player_tasks[0] = &player;
    player_work[0] = &player_data;
    player_motion[0] = &motion;
    player_records[0] = &record;

    camera.pos.x = 1000.0F;
    camera.pos.y = 2000.0F;
    camera.pos.z = 3000.0F;
    game_state.steps = RUMBLE_PERIOD;

    camera_views[0] = &camera;
    draw_number = 0;
    background_step(&background);
    assert(sounds == 1 && rumbles == 1 && fog_updates == 1 && work.tick == 1);

    camera_views[0] = NULL;
    draw_number = 1;
    background_step(&background);
    assert(sounds == 1 && rumbles == 2 && fog_updates == 2 && work.tick == 2);

    camera_views[0] = &camera;
    draw_number = 0;
    background_step(&background);
    assert(sounds == 2 && rumbles == 3 && fog_updates == 3 && work.tick == 3);
    return 0;
}
