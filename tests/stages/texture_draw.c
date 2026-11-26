/* Gameplay runs against a Ninja recorder and a one-step demo scheduler. */
#include <assert.h>
#include <string.h>

#include "camera/camera.h"
#include "camera/viewport.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "draw/fade.h"
#include "hud/hud.h"
#include "hud/item_stock.h"
#include "object/set.h"
#include "player/player.h"
#include "save/save.h"
#include "shared/jungle_prop.h"
#include "stage/stage.h"
#include "stg13/poster.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/timer.h"
#include "system/util.h"

struct game_state game_state;
int frame_step_count, frame_step;
Sint32 camera_active_count;
PDS_PERIPHERAL player_pad_records[PAD_PLAYERS];
const PDS_PERIPHERAL *player_pads[PAD_PORTS];
struct task *player_tasks[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];

static NJS_TEXNAME prompt_slot;
static NJS_TEXLIST press_start_texlist = {&prompt_slot, 1};
static NJS_TEXTURE_VTX press_start_quad[4];
static Sint16 stage_current, stage_mode, banner_timer;
static Sint8 pal_frames;
static unsigned int pads, task_ticks, world_draws, ends, stock_resets, prompts;
static unsigned int state_calls, vertices, selected, depth;
static Uint32 color;
static Int transparent;
static NJS_TEXLIST *bound;
static NJS_CNK_MODEL *drawn_model;
static char transforms[16];
static int transform_count;

static void banner_draw(void)
{
    assert(0);
}
/* the frame functions offer each step to the port's split frame, which
 * this fixture leaves off */
#define PLAY_SPLITS() FALSE
static unsigned int offered;

static void solo_draw_step(void)
{
}

static void play_draw_step(void)
{
}

static void present_step(int done, void (*draw)(void))
{
    assert(draw == NULL);
    offered += done > 0;
}

#include "demo_frame.inc"

Bool placement_try_leave(struct task *task)
{
    return FALSE;
}
Bool placement_try_leave_by(struct task *task, Float range)
{
    return FALSE;
}
void njCnkEasyDrawModel(NJS_CNK_MODEL *model)
{
    assert(depth == 1 && bound != NULL && model != NULL);
    drawn_model = model;
}
void draw_control_set(Uint32 bits)
{
    assert(bits == DRAW_CONTROL_SHADOW);
    ++state_calls;
}
void draw_control_clear(Uint32 bits)
{
    assert(bits == DRAW_CONTROL_SHADOW);
    ++state_calls;
}
Sint32 njSetTexture(NJS_TEXLIST *list)
{
    ++state_calls;
    bound = list;
    return 0;
}
Sint32 njSetTextureNum(Uint32 index)
{
    ++state_calls;
    selected = index;
    return 0;
}
Bool njPushMatrixEx(void)
{
    assert(!depth);
    ++depth;
    ++state_calls;
    return TRUE;
}
Bool njPopMatrixEx(void)
{
    assert(depth == 1);
    --depth;
    ++state_calls;
    return TRUE;
}
void njTranslateEx(NJS_VECTOR *pos)
{
    assert(depth);
    transforms[transform_count++] = 'T';
}
void njRotateX(NJS_MATRIX *m, Angle a)
{
    assert(depth);
    transforms[transform_count++] = 'X';
}
void njRotateY(NJS_MATRIX *m, Angle a)
{
    assert(depth);
    transforms[transform_count++] = 'Y';
}
void njRotateZ(NJS_MATRIX *m, Angle a)
{
    assert(depth);
    transforms[transform_count++] = 'Z';
}
void njDrawTexture3DEx(NJS_TEXTURE_VTX *p, Int count, Int trans)
{
    int i;
    assert(depth == 1 && bound != NULL && count == 4);
    for (i = 0; i < count; i++) {
        assert(p[i].x == (i < 2 ? -2.0F : 2.0F));
        assert(p[i].y == (i % 2 ? 3.0F : -3.0F));
        assert(p[i].col == color);
    }
    vertices += count;
    transparent = trans;
}
void njDrawTexture(NJS_TEXTURE_VTX *quad, Int count, Int texture, Int flags)
{
    assert(bound == &press_start_texlist && quad == press_start_quad &&
           count == 4);
    assert(texture == 100000310 && flags == 1);
    ++prompts;
}
void pad_scan(void)
{
    ++pads;
}
void task_step_level(int level)
{
    assert(level == 0);
}
void task_step_all(void)
{
    ++task_ticks;
}
static void player_step(struct task *player)
{
    assert(player == player_tasks[0]);
    ++task_ticks;
}
void task_draw_all(void)
{
    ++world_draws;
}
Bool collision_mesh_update(void)
{
    return TRUE;
}
void collision_collide_all(void)
{
}
void fade_idle_step(void)
{
}
void stage_clock_step(void)
{
}
void hud_draw(void)
{
}
void viewport_set_whole(void)
{
}
void timer_mark(void)
{
}
Float timer_lap(void)
{
    return 1.0F;
}
Float njRoundUp(Float n)
{
    return n > 0.0F && n <= 1.0F ? 1.0F : n;
}
void stage_end(int end)
{
    assert(end == STAGE_END_CLEAR);
    ++ends;
}
void item_stock_clear_all(void)
{
    ++stock_resets;
}
void frame_wait_step(int left)
{
    assert(0);
}
Sint16 ring_count(int player)
{
    return 0;
}
void stage_clear(int player)
{
    assert(0);
}
void save_count_time(void)
{
}

static void poster(void (*init)(struct task *), NJS_TEXLIST *list, int index,
                   Uint32 tint, const char *rotation, Int trans)
{
    struct task task;
    struct task_work work;
    memset(&task, 0, sizeof(task));
    memset(&work, 0, sizeof(work));
    task.work = &work;
    work.scl.x = 2.0F;
    work.scl.y = 3.0F;
    work.scl.z = (Float)index;
    work.ang.z = 0x81;
    init(&task);
    assert(task.draw && list->textures);
    state_calls = vertices = 0;
    transform_count = 0;
    color = tint;
    bound = NULL;
    list->nbTexture = 0;
    task.draw(&task);
    assert(!state_calls && !vertices && !transform_count);
    list->nbTexture = (Uint32)(index + 1);
    /* A loaded neighboring slot must not authorize the selected slot. */
    if (index) {
        list->textures[0].texaddr = 31;
    }
    list->textures[index].texaddr = 0;
    task.draw(&task);
    assert(!state_calls && !vertices && !transform_count);
    list->textures[index].texaddr = 44;
    task.draw(&task);
    transforms[transform_count] = '\0';
    assert(bound == list && selected == (unsigned int)index &&
           strcmp(transforms, rotation) == 0);
    assert(!depth && state_calls == 6 && vertices == 4 && transparent == trans);
    list->textures[index].texaddr = 0;
    task.draw(&task);
    assert(state_calls == 6 && vertices == 4);
}

static void jungle_props(void)
{
    struct task optional = {0}, neighbor = {0};
    struct task_work optional_work = {0}, neighbor_work = {0};
    NJS_TEXLIST *optional_list = jungle_prop_texlists[17];
    NJS_TEXLIST *neighbor_list = jungle_prop_texlists[2];

    optional.work = &optional_work;
    optional_work.scl.x = 17.0F;
    neighbor.work = &neighbor_work;
    neighbor_work.scl.x = 2.0F;
    jungle_prop_init(&optional);
    jungle_prop_far_init(&neighbor);
    assert(optional.force.p == optional_list &&
           neighbor.force.p == neighbor_list);
    assert(optional.motion.p && neighbor.motion.p &&
           optional.motion.p != neighbor.motion.p);
    optional_list->nbTexture = neighbor_list->nbTexture = 1;
    neighbor_list->textures[0].texaddr = 31;
    optional_list->textures[0].texaddr = 0;
    state_calls = transform_count = 0;
    drawn_model = NULL;

    optional.draw(&optional);
    assert(!state_calls && !transform_count && !drawn_model);
    neighbor.draw(&neighbor);
    assert(bound == neighbor_list && drawn_model == neighbor.motion.p &&
           !depth);
    optional_list->textures[0].texaddr = 44;
    optional.draw(&optional);
    assert(bound == optional_list && drawn_model == optional.motion.p &&
           !depth);

    optional_list->textures[0].texaddr = 0;
    drawn_model = NULL;
    state_calls = transform_count = 0;
    optional.draw(&optional);
    assert(!state_calls && !transform_count && !drawn_model);
    optional.step(&optional);
    optional.destructor(&optional);
    assert(!optional.motion.p && !optional.force.p);
}

static void demo(int solo, Uint32 steps, int show, Uint32 count, Uint32 handle,
                 int start, int expect)
{
    static struct task player;
    static PDS_PERIPHERAL pad;
    int result;
    memset(&game_state, 0, sizeof(game_state));
    game_state.solo_player = solo;
    game_state.demo = 1;
    game_state.unk_0b = show;
    game_state.total_steps = steps;
    game_state.render_time = 1.0F;
    player.step = player_step;
    player_tasks[0] = &player;
    player_pads[0] = player_pads[1] = &pad;
    pad.press = start ? PDD_DGT_ST : 0;
    frame_step_count = 1;
    press_start_texlist.nbTexture = count;
    prompt_slot.texaddr = handle;
    pads = task_ticks = world_draws = prompts = ends = stock_resets =
        state_calls = 0;
    bound = NULL;
    result = stage_run();
    assert(result == 0 && pads == 1 && task_ticks == 1 && world_draws == 1);
    assert(prompts == (unsigned int)expect &&
           state_calls == (unsigned int)expect);
    assert(ends == (unsigned int)start &&
           stock_resets == (unsigned int)(start && !solo));
    assert(game_state.draw_time == 1.0F);
}
int main(void)
{
    int solo;
    jungle_props();
    poster(poster_init, poster_texlists[0], 8, 0xffffffff, "TY", TRUE);
    poster(poster_m_init, poster_texlists[0], 3, 0xff818181, "TY", FALSE);
    poster(poster_3_init, poster_texlists[1], 0, 0xffffffff, "TZXY", TRUE);
    for (solo = 0; solo <= 1; ++solo) {
        demo(solo, 0x40, 1, 0, 44, 0, 0);
        demo(solo, 0x40, 1, 1, 0, 1, 0);
        demo(solo, 0x40, 1, 1, 44, 1, 1);
        demo(solo, 0x00, 1, 1, 44, 0, 0);
        demo(solo, 0x40, 0, 1, 44, 0, 0);
    }
    return 0;
}
