/* Linked with the actual pickup, scheduler, collision and garden registry. */
#include <assert.h>
#include <math.h>
#include <string.h>
#include "chao/world_slots.h"
#include "object/mover.h"

struct chao_tuning chao_tuning;
Sint32 tree_fruit_counts[2], tree_fruit_node_index;
Angle tree_fruit_phases[4];
NJS_TEXLIST egg_texlist, neutral_garden_texlist, hero_garden_texlist,
    dark_garden_texlist;
NJS_CNK_OBJECT garden_tree_neutral, garden_tree_hero, garden_tree_dark,
    garden_tree_dark_crown;
NJS_CNK_OBJECT *garden_tree_models[GARDEN_SEED_TYPES][3];
NJS_MATRIX _nj_unit_matrix_;

static int blocks, samples, drop_sounds;
/* Initial shakes, then the six draws for each attempted drop, in retail
 * order: shakes, growth, x drift, z drift, reverse direction, heading. */
static const int draws[] = {
    3000,         8000,         14000,        10000,        RAND_MAX / 4,
    RAND_MAX / 2, RAND_MAX / 3, RAND_MAX / 8, 1000,         20000,
    RAND_MAX / 2, RAND_MAX / 4, RAND_MAX / 8, RAND_MAX / 3, 29000};

int rand(void)
{
    assert(samples < (int)(sizeof(draws) / sizeof(*draws)));
    return draws[samples++];
}
void *syMalloc(Uint32 size)
{
    void *memory = malloc(size);
    assert(memory);
    ++blocks;
    return memory;
}
void *syCalloc(Uint32 count, Uint32 size)
{
    void *memory = syMalloc(count * size);
    memset(memory, 0, count * size);
    return memory;
}
void syFree(void *memory)
{
    assert(memory && blocks > 0);
    free(memory);
    --blocks;
}
void njMemCopy(void *dst, const void *src, Sint32 size)
{
    memcpy(dst, src, (size_t)size);
}
Float njScalor(NJS_VECTOR *v)
{
    return (Float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}
Float njSqrt(Float value)
{
    return (Float)sqrt(value);
}
Float njSin(Angle angle)
{
    return (Float)sin(angle / ANGLE_PER_RADIAN);
}
Float njCos(Angle angle)
{
    return (Float)cos(angle / ANGLE_PER_RADIAN);
}
Angle angle_approach(Angle from, Angle to, Angle step)
{
    assert(from == to);
    return to;
}
int chao_current_area(void)
{
    return CHAO_AREA_HERO;
}
void sound_detach_task(const struct task *task)
{
}
struct task *garden_hit_kind(struct task *task, int shape, Uint8 kind)
{
    return NULL;
}
Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *position)
{
    assert(id == 0x1009);
    ++drop_sounds;
    return 0;
}
Sint8 sound_play_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                        Sint16 duration, const NJS_POINT3 *position)
{
    assert(id == 0x1008); /* the second half of a shake */
    return 0;
}

/* Permanent Hero Garden trees have a fixed drop height. Only planted trees
 * need a private mesh, and only the other gardens traverse fruit nodes. */
NJS_CNK_OBJECT *garden_clone_object(NJS_CNK_OBJECT *obj)
{
    abort();
}
void garden_free_object(NJS_CNK_OBJECT *obj)
{
    abort();
}
void garden_blend_models(const NJS_CNK_OBJECT *a, const NJS_CNK_OBJECT *b,
                         NJS_CNK_OBJECT *out, Float share)
{
    abort();
}
Bool njPushMatrix(NJS_MATRIX *m)
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
void njTranslateEx(NJS_VECTOR *v)
{
    abort();
}
void njTranslateV(NJS_MATRIX *m, NJS_VECTOR *v)
{
    abort();
}
void njRotateX(NJS_MATRIX *m, Angle x)
{
    abort();
}
void njRotateY(NJS_MATRIX *m, Angle y)
{
    abort();
}
void njRotateXYZ(NJS_MATRIX *m, Angle x, Angle y, Angle z)
{
    abort();
}
void njGetTranslation(NJS_MATRIX *m, NJS_POINT3 *v)
{
    abort();
}

int main(void)
{
    /* A fully ripe fruit is saved as 200 percent in a signed byte. */
    struct garden_tree_save save = {
        TREE_KIND_PERMANENT, TREE_STAGE_PERMANENT, 0, 100, {-56, -20, -25}, 3};
    struct garden_tree_save ripe;
    struct task owners[GARDEN_ROW_SLOTS] = {{0}};
    NJS_POINT3 pos = {3.0F, 7.0F, -1.5F};
    struct garden_tree *work;
    struct task *tree, *fruit;
    struct mover *mover;
    Float growth;
    int slot, before;

    task_init();
    tree = garden_plant_tree(&pos, &save);
    assert(tree && samples == TREE_FRUITS);
    work = TREE_WORK(tree);
    assert(work->save == &save && work->stage == TREE_STAGE_PERMANENT);
    chao_tuning.tree_life_interval = 600;
    chao_tuning.tree_fruit_growth = 0.001F;
    chao_tuning.tree_fruit_limit = 6;
    /* A full row forces the real constructor to decline the first shake. */
    for (slot = 0; slot < (int)garden_object_capacities[GARDEN_OBJECT_FRUIT];
         ++slot) {
        assert(garden_register_object(GARDEN_OBJECT_FRUIT, &owners[slot], 0));
    }
    work->fruit[0].shakes_left = 1;
    TASK_MOTION_OF(tree)->acc.x = 1.0F;
    garden_pack_tree(tree);
    ripe = save;
    before = blocks;
    tree->step(tree);
    assert(samples == 9 && !drop_sounds && !task_levels[4] && before == blocks);
    assert(work->fruit[0].growth == 2.0F &&
           work->fruit[0].target_growth == 2.0F);
    garden_pack_tree(tree);
    assert(!memcmp(&save, &ripe, sizeof(save)));
    assert(garden_unregister_object(&owners[--slot]));

    /* Releasing the tree completes the shake and retries the same fruit. */
    tree->step(tree);
    assert(samples == 15 && drop_sounds == 1);
    fruit = task_levels[4];
    assert(fruit && fruit->next == fruit && !fruit->draw);
    assert(garden_fruit_kind(fruit) == GARDEN_FRUIT_HERO_GARDEN);
    assert(fruit->work->ang.y == (Angle)(draws[14] * 6.0349703e-5F));
    assert(fruit->work->pos.x == pos.x && fruit->work->pos.z == pos.z);
    assert(fruit->work->pos.y == work->base.pos.y + 29.0F);
    mover = fruit->motion.p;
    assert(fabs(mover->spd.x + (njSin(work->tilt_direction) * 0.03F +
                                (0.5F - draws[11] * RAND_SCALE) * 0.01F)) <
           0.000001F);
    assert(fabs(mover->spd.z + (njCos(work->tilt_direction) * 0.03F +
                                (0.5F - draws[12] * RAND_SCALE) * 0.01F)) <
           0.000001F);
    growth = -(draws[10] * (0.3F * RAND_SCALE));
    assert(work->fruit[0].growth == growth &&
           work->fruit[0].target_growth == growth);
    assert(work->fruit[0].shakes_left == (int)(draws[9] * 4.2724606e-4F) + 11);
    garden_pack_tree(tree);
    assert(save.fruit[0] == (Sint8)(growth * 100.0F));
    assert(save.fruit[1] == ripe.fruit[1] && save.fruit[2] == ripe.fruit[2]);
    assert(!garden_find_object(GARDEN_OBJECT_FRUIT, slot));
    fruit->step(fruit);
    assert(garden_find_object(GARDEN_OBJECT_FRUIT, slot) == fruit);
    tree->step(tree);
    assert(samples == 15 && drop_sounds == 1 && fruit->next == fruit);
    task_destroy(fruit);
    task_destroy(tree);
    while (slot) {
        assert(garden_unregister_object(&owners[--slot]));
    }
    assert(!blocks);
    return 0;
}
