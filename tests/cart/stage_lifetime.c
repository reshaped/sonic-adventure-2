/* Exercise the real CART entry points with the stage services supplied here. */
#include <assert.h>
#include <string.h>

#include "cart/stage.c"

struct game_state game_state;
struct stage_hooks stage_hooks;
struct cart_setup cart_setup;
union cart_set_buffer cart_set;
struct object_list cart_objects;
struct download_entry download_chosen;
const struct sound_list cart_sounds = {0};
struct texture_pvm cart_pvms[1];
NJS_TEXLIST **cart_texture_groups[1];
NJS_TEXLIST cart_object_textures, cart_story_textures;

struct course_case {
    int stage, character, mission, selection;
    const char *filename;
};
static const struct course_case courses[] = {
    {STAGE_CART_1, PLAYER_TAILS_MECH, 0, 0, "setCartTails.bin"},
    {STAGE_CART_1, PLAYER_TAILS_MECH, MISSION_HD, 0, "setCartTails_hd.bin"},
    {STAGE_CART_1, PLAYER_ROUGE, 0, 0, "setCartRouge.bin"},
    {STAGE_CART_1, PLAYER_ROUGE, MISSION_HD, 0, "setCartRouge_hd.bin"},
    {STAGE_CART_2, PLAYER_SONIC, 0, 0, "setCartMini1.bin"},
    {STAGE_CART_2, PLAYER_SONIC, 0, 1, "setCartMini2.bin"},
    {STAGE_CART_2, PLAYER_SONIC, 0, 2, "setCartMini3.bin"},
};
static const struct course_case *selected;
static Bool available, models, course;
static int set_starts, course_loads, course_unloads, early_releases;
static int stage_textures;
static Bool shared_textures;

int stage_current_id(void)
{
    return selected->stage;
}
int stage_character(int slot)
{
    assert(slot == 0);
    return selected->character;
}
Bool set_read_into(const char *name, struct set_head *head, Uint32 capacity)
{
    assert(strcmp(name, selected->filename) == 0);
    assert(head == &cart_set.file.head && capacity == 32768);
    assert(head->count == 0 && cart_set.file.entries[0].id == 0);
    if (available) {
        head->count = 1;
        cart_set.file.entries[0].id = 3;
    }
    return available;
}
Bool set_start(struct object_list *list, struct set_head *head)
{
    assert(list == &cart_objects && head == &cart_set.file.head);
    assert(head->count == (available ? 1U : 0U));
    assert(cart_set.file.entries[0].id == (available ? 3 : 0));
    set_starts++;
    return TRUE;
}
void cart_init_models(void)
{
    assert(!models && !course);
    models = TRUE;
}
void cart_release_models(void)
{
    assert(models && !course);
    models = FALSE;
    early_releases++;
}
void cart_load_course(void)
{
    assert(models && !course);
    course = TRUE;
    course_loads++;
}
void cart_unload_course(int mode)
{
    assert(models && course);
    assert(mode == (selected->stage == STAGE_CART_1));
    models = FALSE;
    course = FALSE;
    course_unloads++;
}
Sint32 njLoadTexturePvmFile(const char *filename, NJS_TEXLIST *list)
{
    assert(list == &cart_object_textures || list == &cart_story_textures);
    stage_textures++;
    return 0;
}
Sint32 njReleaseTexture(NJS_TEXLIST *list)
{
    assert(stage_textures > 0);
    stage_textures--;
    return 0;
}

int main(void)
{
    int i;
    game_state.hud_on = 1;
    for (i = 0; i < (int)(sizeof(courses) / sizeof(courses[0])); i++) {
        selected = &courses[i];
        game_state.mission = selected->mission;
        game_state.kart_course = selected->selection;
        available = TRUE;
        cart_init();
        assert(models && !course && game_state.hud_on == 0);
        cart_step(NULL);
        cart_step(NULL);
        assert(course_loads == i + 1);
        cart_exit();
        assert(!models && !course && game_state.hud_on == 1);
        assert(stage_textures == 0 && course_unloads == i + 1);
    }

    available = FALSE;
    cart_init();
    cart_exit();
    cart_step(NULL);
    assert(!models && !course && stage_textures == 0);
    assert(early_releases == 1 && course_loads == i);
    available = TRUE;
    cart_init();
    cart_step(NULL);
    cart_exit();
    assert(course_loads == i + 1 && course_unloads == i + 1);
    assert(set_starts == i + 2);
    return 0;
}

/* These services do not own the deferred course or the SET buffer. */
void frame_pacing_init(int interval, int steps)
{
}
void sound_load_cart(const char *name, const struct sound_list *list)
{
}
void lives_add(int player, Sint16 count)
{
}
int world_land_start(struct land_table *land)
{
    return 1;
}
void loader_read_camera_files(void)
{
}
void light_set(int index, const struct light *light)
{
}
void light_select(int index, int player)
{
}
void light_select_camera(Sint8 index)
{
}
void light_select_after_draw(Sint8 index)
{
}
struct task *message_window_open_unowned(int kind, const char *text, int time,
                                         int language)
{
    return NULL;
}
struct trail *trail_alloc(struct trail_style *style, int count)
{
    return NULL;
}
void trail_draw(struct trail_style *style, struct trail *trail, NJS_POINT3 *pos)
{
}
void trail_free(struct trail_style *style, struct trail *trail)
{
}
void cart_action_icon(struct icon_rect *rect)
{
}
void cart_number_cell(struct hud_number_cell *cell)
{
}
void particle_core_start(void)
{
}
void particle_core_stop(void)
{
}
Bool texture_load_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups)
{
    assert(!shared_textures);
    shared_textures = TRUE;
    return TRUE;
}
Bool texture_await_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups)
{
    return shared_textures;
}
void texture_release_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups)
{
    assert(shared_textures);
    shared_textures = FALSE;
}
int cart_download_load(void)
{
    assert(0);
    return FALSE;
}
void cart_download_copy_set(void *to, Uint32 capacity)
{
    assert(0);
}
void world_set_near_radius(Float radius)
{
}
void world_reset_near_radius(void)
{
}
void cart_boost_windows_start(void)
{
}
void cart_boost_windows_stop(void)
{
}
void sound_release_fx_level(void)
{
}
int world_land_stop(void)
{
    return 1;
}
void camera_stop(void)
{
}
void sound_task_kill(void)
{
}
